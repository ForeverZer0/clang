#include "clang.h"

void clang_dset_free(void *data)
{
    if (data)
        clang_disposeDiagnosticSet(data);
}

void clang_diagnostic_free(void *data)
{
    if (data)
        clang_disposeDiagnostic(data);
}

static VALUE dset_each(VALUE self)
{
    RETURN_ENUMERATOR(self, 0, NULL);

    CXDiagnosticSet set = DATA_PTR(self);
    unsigned int n = clang_getNumDiagnosticsInSet(set);

    for (unsigned i = 0; i < n; i++)
    {
        CXDiagnostic d = clang_getDiagnosticInSet(set, i);
        rb_yield(Data_Wrap_Struct(rb_cCXDiagnostic, NULL, clang_diagnostic_free, d));
    }
    return self;
}

static VALUE dset_get(VALUE self, VALUE index)
{
    CXDiagnosticSet set = DATA_PTR(self);
    unsigned int n = clang_getNumDiagnosticsInSet(set), i = NUM2UINT(index);
    if (i >= n)
        return Qnil;
    
    CXDiagnostic d = clang_getDiagnosticInSet(set, i);
    return Data_Wrap_Struct(rb_cCXDiagnostic, NULL, clang_diagnostic_free, d);
}

static VALUE dset_size(VALUE self)
{
    CXDiagnosticSet set = DATA_PTR(self);
    unsigned int n = clang_getNumDiagnosticsInSet(set);
    return UINT2NUM(n);
}

static VALUE dset_load(VALUE klass, VALUE path)
{
    const char *file = StringValueCStr(path);

    enum CXLoadDiag_Error err;
    CXString err_message;

    CXDiagnosticSet set = clang_loadDiagnostics(file, &err, &err_message);
    if (err != CXLoadDiag_None)
    {
        const char *msg = clang_getCString(err_message);
        rb_raise(rb_eRuntimeError, "%s", err_message);
    }

    return Data_Wrap_Struct(klass, NULL, clang_dset_free, set);
}

static VALUE diagnostic_children(VALUE self)
{
    CXDiagnostic d = DATA_PTR(self);
    CXDiagnosticSet set = clang_getChildDiagnostics(d);
    return set ? Data_Wrap_Struct(rb_cCXDiagnosticSet, NULL, RUBY_NEVER_FREE, set) : Qnil;
}

static VALUE diagnostic_format(VALUE self, VALUE options)
{
    CXDiagnostic d = DATA_PTR(self);
    unsigned int mask = rb_enum_mask(rb_DiagnosticDisplayOptions, options);
    clang_formatDiagnostic(d, mask);
    return UINT2NUM(mask);
}

static VALUE diagnostic_severity(VALUE self)
{
    CXDiagnostic d = DATA_PTR(self);
    return rb_enum_symbol(rb_DiagnosticSeverity, clang_getDiagnosticSeverity(d));
}

static VALUE diagnostic_default_format(VALUE klass)
{
    int mask = clang_defaultDiagnosticDisplayOptions();
    return rb_enum_unmask(rb_DiagnosticDisplayOptions, mask);
}

static VALUE diagnostic_location(VALUE self)
{
    CXSourceLocation *loc = ALLOC(CXSourceLocation);
    *loc = clang_getDiagnosticLocation(DATA_PTR(self));
    return Data_Wrap_Struct(rb_cCXSourceLocation, NULL, RUBY_DEFAULT_FREE, loc);
}

static VALUE diagnostic_spelling(VALUE self)
{
    CXString str = clang_getDiagnosticSpelling(DATA_PTR(self));
    return RUBYSTR(str);
}

static VALUE diagnostic_option_str(int argc, VALUE *argv, VALUE self)
{
    VALUE disable;
    rb_scan_args(argc, argv, "01", &disable);

    CXString enable_str, disable_str;
    enable_str = clang_getDiagnosticOption(DATA_PTR(self), &disable_str);
    const char *str = clang_getCString(RTEST(disable) ? enable_str : disable_str);
    VALUE rb = rb_str_new_cstr(str);

    clang_disposeString(enable_str);
    clang_disposeString(disable_str);   
    return rb;
}

static VALUE diagnostic_category(VALUE self)
{
    unsigned int n = clang_getDiagnosticCategory(DATA_PTR(self));
    return UINT2NUM(n);
}

static VALUE diagnostic_category_name(VALUE self)
{
    CXString str = clang_getDiagnosticCategoryText(DATA_PTR(self));
    return RUBYSTR(str);
}

static VALUE diagnostic_range_count(VALUE self)
{
    return UINT2NUM(clang_getDiagnosticNumRanges(DATA_PTR(self)));
}

static VALUE diagnostic_ranges(VALUE self)
{
    CXDiagnostic d = DATA_PTR(self);
    unsigned int n = clang_getDiagnosticNumRanges(d);

    VALUE ary = rb_ary_new_capa(n);
    for (unsigned i = 0; i < n; i++)
    {
        CXSourceRange *range = ALLOC(CXSourceRange);
        *range = clang_getDiagnosticRange(d, i);
        rb_ary_store(ary, i, Data_Wrap_Struct(rb_cCXSourceRange, NULL, RUBY_DEFAULT_FREE, range));
    }
    return ary;
}

static VALUE diagnostic_each_range(VALUE self)
{
    RETURN_ENUMERATOR(self, 0, NULL);

    CXDiagnostic d = DATA_PTR(self);
    unsigned int n = clang_getDiagnosticNumRanges(d);

    for (unsigned i = 0; i < n; i++)
    {
        CXSourceRange *range = ALLOC(CXSourceRange);
        *range = clang_getDiagnosticRange(d, i);
        rb_yield(Data_Wrap_Struct(rb_cCXSourceRange, NULL, RUBY_DEFAULT_FREE, range));
    }
    return self; 
}

static VALUE diagnostic_fixit_count(VALUE self)
{
    return UINT2NUM(clang_getDiagnosticNumFixIts(DATA_PTR(self)));
}

static VALUE diagnostic_fixits(VALUE self)
{
    CXDiagnostic d = DATA_PTR(self);
    unsigned int n = clang_getDiagnosticNumFixIts(d);
    VALUE ary = rb_ary_new_capa(n);

    for (unsigned i = 0; i < n; i++)
    {
        CXSourceRange *range = ALLOC(CXSourceRange);
        CXString s = clang_getDiagnosticFixIt(d, i, range);
        VALUE r = Data_Wrap_Struct(rb_cCXSourceRange, NULL, RUBY_DEFAULT_FREE, range);
        rb_ary_store(ary, i, rb_ary_new_from_args(2, RUBYSTR(s), r));
    }

    return ary;
}

static VALUE diagnostic_each_fixit(VALUE self)
{
    RETURN_ENUMERATOR(self, 0, NULL);
    CXDiagnostic d = DATA_PTR(self);
    unsigned int n = clang_getDiagnosticNumFixIts(d);

    for (unsigned i = 0; i < n; i++)
    {
        CXSourceRange *range = ALLOC(CXSourceRange);
        CXString s = clang_getDiagnosticFixIt(d, i, range);
        VALUE r = Data_Wrap_Struct(rb_cCXSourceRange, NULL, RUBY_DEFAULT_FREE, range);
        rb_yield(rb_ary_new_from_args(2, RUBYSTR(s), r));
    }

    return self;
}

void Init_clang_diagnostic(void)
{
    rb_include_module(rb_cCXDiagnosticSet, rb_mEnumerable);
    rb_define_singleton_method1(rb_cCXDiagnosticSet, "load", dset_load, 1);
    rb_define_method1(rb_cCXDiagnosticSet, "[]", dset_get, 1);
    rb_define_method0(rb_cCXDiagnosticSet, "each", dset_each, 0);
    rb_define_method0(rb_cCXDiagnosticSet, "size", dset_size, 0);
    rb_define_alias(rb_cCXDiagnosticSet, "length", "size");

    rb_define_singleton_method0(rb_cCXDiagnostic, "default_format", diagnostic_default_format, 0);
    rb_define_method0(rb_cCXDiagnostic, "children", diagnostic_children, 0);
    rb_define_methodm2(rb_cCXDiagnostic, "format", diagnostic_format, -2);
    rb_define_method0(rb_cCXDiagnostic, "severity", diagnostic_severity, 0);
    rb_define_method0(rb_cCXDiagnostic, "location", diagnostic_location, 0);
    rb_define_method0(rb_cCXDiagnostic, "spelling", diagnostic_spelling, 0);
    rb_define_methodm1(rb_cCXDiagnostic, "option_str", diagnostic_option_str, -1);
    rb_define_method0(rb_cCXDiagnostic, "category", diagnostic_category, 0);
    rb_define_method0(rb_cCXDiagnostic, "category_name", diagnostic_category_name, 0);
    rb_define_method0(rb_cCXDiagnostic, "range_count", diagnostic_range_count, 0);
    rb_define_method0(rb_cCXDiagnostic, "ranges", diagnostic_ranges, 0);
    rb_define_method0(rb_cCXDiagnostic, "each_range", diagnostic_each_range, 0);
    rb_define_method0(rb_cCXDiagnostic, "fixit_count", diagnostic_fixit_count, 0);
    rb_define_method0(rb_cCXDiagnostic, "fixits", diagnostic_fixits, 0);
    rb_define_method0(rb_cCXDiagnostic, "each_fixit", diagnostic_each_fixit, 0);

    rb_define_method0(rb_cCXDiagnostic, "to_s", diagnostic_spelling, 0);
    rb_define_alias(rb_cCXDiagnostic, "to_str", "to_s");
}