#include "clang.h"

ID id_presumed;
ID id_expanded;
ID id_spelling;
ID id_file;

static VALUE location_null(VALUE klass)
{
    CXSourceLocation *loc = ALLOC(CXSourceLocation);
    *loc = clang_getNullLocation();
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, loc);
}

static VALUE location_equals(VALUE self, VALUE other)
{
    if (CLASS_OF(self) != CLASS_OF(other))
        return Qfalse;

    CXSourceLocation *loc1 = DATA_PTR(self), *loc2 = DATA_PTR(other);
    return RB_BOOL(clang_equalLocations(*loc1, *loc2));
}

static VALUE location_initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE tu, file, line, column;
    rb_scan_args(argc, argv, "31", &tu, &file, &line, &column);

    if (!rb_obj_is_kind_of(tu, rb_cCXTranslationUnit) != Qtrue)
        rb_raise(rb_eTypeError, "");

    rb_assert_type(tu, rb_cCXTranslationUnit);
    rb_assert_type(file, rb_cCXFile);

    CXTranslationUnit unit = DATA_PTR(tu);
    CXFile f = DATA_PTR(file);

    CXSourceLocation location;
    // Offset
    if (argc == 3)
    {
        location = clang_getLocationForOffset(unit, f, NUM2UINT(line));
    }
    // Line/Column
    else
    {
        location = clang_getLocation(unit, f, NUM2UINT(line), NUM2UINT(column));
    }

    memcpy(DATA_PTR(self), &location, sizeof(CXSourceLocation));
    return self;
}

static VALUE location_is_system(VALUE self)
{
    CXSourceLocation *loc = DATA_PTR(self);
    return RB_BOOL(clang_Location_isInSystemHeader(*loc));
}

static VALUE location_is_main_file(VALUE self)
{
    CXSourceLocation *loc = DATA_PTR(self);
    return RB_BOOL(clang_Location_isFromMainFile(*loc));
}

static VALUE location_file(int argc, VALUE *argv, VALUE self)
{
    VALUE type;
    rb_scan_args(argc, argv, "01", &type);
    CXSourceLocation *loc = DATA_PTR(self);

    CXFile file;
    ID id = SYMBOL_P(type) ? SYM2ID(type) : id_file;

    if (id == id_file)
    {
        clang_getFileLocation(*loc, &file, NULL, NULL, NULL);
    }
    else if (id == id_expanded)
    {
        clang_getExpansionLocation(*loc, &file, NULL, NULL, NULL);
    }
    else if (id == id_presumed)
    {
        CXString str;
        clang_getPresumedLocation(*loc, &str, NULL, NULL);
        return RUBYSTR(str);
    }
    else if (id == id_spelling)
    {
        clang_getSpellingLocation(*loc, &file, NULL, NULL, NULL);
    }
    else 
    {
        rb_raise(rb_eArgError, "invalid type argument specified");
    }
    
    return Data_Wrap_Struct(rb_cCXFile, NULL, RUBY_NEVER_FREE, file);
}

static VALUE location_line(int argc, VALUE *argv, VALUE self)
{
    VALUE type;
    rb_scan_args(argc, argv, "01", &type);
    CXSourceLocation *loc = DATA_PTR(self);
    unsigned int value;

    ID id = SYMBOL_P(type) ? SYM2ID(type) : id_file;
    if (id == id_file)
    {
        clang_getFileLocation(*loc, NULL, &value, NULL, NULL);
    }
    else if (id == id_expanded)
    {
        clang_getExpansionLocation(*loc, NULL, &value, NULL, NULL);
    }
    else if (id == id_presumed)
    {
        clang_getPresumedLocation(*loc, NULL, &value, NULL);
    }
    else if (id == id_spelling)
    {
        clang_getSpellingLocation(*loc, NULL, &value, NULL, NULL);
    }
    else 
    {
        rb_raise(rb_eArgError, "invalid type argument specified");
    }

    return UINT2NUM(value);
}

static VALUE location_column(int argc, VALUE *argv, VALUE self)
{
    VALUE type;
    rb_scan_args(argc, argv, "01", &type);
    CXSourceLocation *loc = DATA_PTR(self);
    unsigned int value;

    ID id = SYMBOL_P(type) ? SYM2ID(type) : id_file;
    if (id == id_file)
    {
        clang_getFileLocation(*loc, NULL, NULL, &value, NULL);
    }
    else if (id == id_expanded)
    {
        clang_getExpansionLocation(*loc, NULL, NULL, &value, NULL);
    }
    else if (id == id_presumed)
    {
        clang_getPresumedLocation(*loc, NULL, NULL, &value);
    }
    else if (id == id_spelling)
    {
        clang_getSpellingLocation(*loc, NULL, NULL, &value, NULL);
    }
    else 
    {
        rb_raise(rb_eArgError, "invalid type argument specified");
    }

    return UINT2NUM(value);
}

static VALUE location_offset(int argc, VALUE *argv, VALUE self)
{
    VALUE type;
    rb_scan_args(argc, argv, "01", &type);
    CXSourceLocation *loc = DATA_PTR(self);
    unsigned int value;

    ID id = SYMBOL_P(type) ? SYM2ID(type) : id_file;
    if (id == id_file)
    {
        clang_getFileLocation(*loc, NULL, NULL, NULL, &value);
    }
    else if (id == id_expanded)
    {
        clang_getExpansionLocation(*loc, NULL, NULL, NULL, &value);
    }
    else if (id == id_spelling)
    {
        clang_getSpellingLocation(*loc, NULL, NULL, NULL, &value);
    }
    else 
    {
        rb_raise(rb_eArgError, "invalid type argument specified");
    }
    return UINT2NUM(value);
}

void Init_clang_source_location(void)
{
    rb_define_singleton_method0(rb_cCXSourceLocation, "null", location_null, 0);

    rb_define_methodm1(rb_cCXSourceLocation, "initialize", location_initialize, -1);
    rb_define_method0(rb_cCXSourceLocation, "system_header?", location_is_system, 0);
    rb_define_method0(rb_cCXSourceLocation, "main_file?", location_is_main_file, 0);

    rb_define_methodm1(rb_cCXSourceLocation, "file", location_file, -1);
    rb_define_methodm1(rb_cCXSourceLocation, "line", location_line, -1);
    rb_define_methodm1(rb_cCXSourceLocation, "column", location_column, -1);
    rb_define_methodm1(rb_cCXSourceLocation, "offset", location_offset, -1);

    rb_define_method1(rb_cCXSourceLocation, "==", location_equals, 1);
    rb_define_method1(rb_cCXSourceLocation, "eql?", location_equals, 1);

    id_spelling = rb_intern("spelling");
    id_expanded = rb_intern("expanded");
    id_presumed = rb_intern("presumed");
    id_file = rb_intern("file");
}