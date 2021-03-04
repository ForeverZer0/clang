#include "clang.h"


static VALUE file_alloc(VALUE klass)
{
    return Data_Wrap_Struct(klass, NULL, RUBY_NEVER_FREE, NULL);
}

static VALUE file_time(VALUE self)
{
    time_t t = clang_getFileTime(DATA_PTR(self));
    return rb_time_new(t, 0);
}

static VALUE file_equal(VALUE self, VALUE other)
{
    if (CLASS_OF(self) != CLASS_OF(other))
        return Qfalse;

    return RB_BOOL(clang_File_isEqual(DATA_PTR(self), DATA_PTR(other)));
}

static VALUE file_include_guarded(VALUE self, VALUE unit)
{
    rb_assert_type(unit, rb_cCXTranslationUnit);
    return RB_BOOL(clang_isFileMultipleIncludeGuarded(DATA_PTR(unit), DATA_PTR(self)));
}

static VALUE file_name(VALUE self)
{
    CXString name = clang_getFileName(DATA_PTR(self));
    return RUBYSTR(name);
}

static VALUE file_initialize(VALUE self, VALUE filename, VALUE translation_unit)
{
    rb_assert_type(translation_unit, rb_cCXTranslationUnit);

    const char *name = StringValueCStr(filename);
    CXFile file = clang_getFile(DATA_PTR(translation_unit), name);
    if (!file)
        rb_raise(rb_eLoadError, "failed to create File instance");

    RDATA(self)->data = file;
    return self;
}

static VALUE file_real_name(VALUE self)
{
    return RUBYSTR(clang_File_tryGetRealPathName(DATA_PTR(self)));
}

static VALUE file_contents(VALUE self, VALUE translation_unit)
{
    rb_assert_type(translation_unit, rb_cCXTranslationUnit);

    size_t size;
    const char *str = clang_getFileContents(DATA_PTR(translation_unit), DATA_PTR(self), &size);
    return rb_str_new(str, size);
}

static VALUE unsaved_initialize(VALUE self, VALUE filename, VALUE contents)
{
    struct CXUnsavedFile *file = DATA_PTR(self);

    if (NIL_P(filename))
        rb_raise(rb_eArgError, "filename cannot be nil");
    file->Filename = StringValuePtr(filename);

    ID id = rb_intern("@contents");

    if (RTEST(contents))
    {
        // Storing a copy ensures the string is not garbage collected and/or modified after initialization
        VALUE str = rb_str_dup(contents);
        rb_str_freeze(str);
        rb_ivar_set(self, id, str);
        file->Contents = StringValuePtr(str);
        file->Length = RSTRING_LEN(str);
    }
    else
    {
        rb_ivar_set(self, id, Qnil);
        file->Contents = NULL;
        file->Length = 0;
    }
    
    return self;
}

void Init_clang_file(void)
{
    rb_define_method2(rb_cCXUnsavedFile, "initialize", unsaved_initialize, 2);

    rb_define_method2(rb_cCXFile, "initialize", file_initialize, 2);
    rb_define_method1(rb_cCXFile, "guarded?", file_include_guarded, 1);
    rb_define_method0(rb_cCXFile, "name", file_name, 0);
    rb_define_method0(rb_cCXFile, "real_name", file_real_name, 0);
    rb_define_method1(rb_cCXFile, "contents", file_contents, 1);
    rb_define_method0(rb_cCXFile, "time", file_time, 0);
    rb_define_method1(rb_cCXFile, "==", file_equal, 1);
    rb_define_method1(rb_cCXFile, "eql?", file_equal, 1);
}