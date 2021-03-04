#include "clang.h"

ID id_unit;

static VALUE mod_initialize(VALUE self, VALUE translation_unit, VALUE file)
{
    rb_assert_type(translation_unit, rb_cCXTranslationUnit);
    rb_assert_type(file, rb_cCXFile);

    CXModule mod = clang_getModuleForFile(DATA_PTR(translation_unit), DATA_PTR(file));
    if (!mod)
        rb_raise(rb_eLoadError, "failed to create Module instance");

    RDATA(self)->data = mod;
    rb_ivar_set(self, id_unit, translation_unit);
    return self;
}

static VALUE mod_parent(VALUE self)
{
    CXModule mod = clang_Module_getParent(DATA_PTR(self));
    return mod ? Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_NEVER_FREE, mod) : Qnil;
}

static VALUE mod_name(VALUE self)
{
    return RUBYSTR(clang_Module_getName(DATA_PTR(self)));
}

static VALUE mod_full_name(VALUE self)
{
    return RUBYSTR(clang_Module_getFullName(DATA_PTR(self)));
}

static VALUE mod_is_system(VALUE self)
{
    return RB_BOOL(clang_Module_isSystem(DATA_PTR(self)));
}

static VALUE mod_ast_file(VALUE self)
{
    CXFile file = clang_Module_getASTFile(DATA_PTR(self));
    return file ? Data_Wrap_Struct(rb_cCXFile, NULL, RUBY_NEVER_FREE, file) : Qnil;
}

static VALUE mod_headers(VALUE self)
{
    VALUE unit = rb_ivar_get(self, id_unit);
    if (NIL_P(unit))
        rb_raise(rb_eRuntimeError, "undefined translation unit for Module");
    CXTranslationUnit tu = DATA_PTR(unit);
    CXModule mod = DATA_PTR(self);
    unsigned int n = clang_Module_getNumTopLevelHeaders(tu, mod);

    VALUE ary = rb_ary_new_capa(n);
    for (unsigned i = 0; i < n; i++)
    {
        CXFile file = clang_Module_getTopLevelHeader(tu, mod, i);
        rb_ary_store(ary, i, Data_Wrap_Struct(rb_cCXFile, NULL, RUBY_NEVER_FREE, file));
    }
    return ary;
}

static VALUE mod_each_header(VALUE self)
{
    RETURN_ENUMERATOR(self, 0, NULL);

    VALUE unit = rb_ivar_get(self, id_unit);
    if (NIL_P(unit))
        rb_raise(rb_eRuntimeError, "undefined translation unit for Module");
    CXTranslationUnit tu = DATA_PTR(unit);
    CXModule mod = DATA_PTR(self);
    unsigned int n = clang_Module_getNumTopLevelHeaders(tu, mod);

    for (unsigned i = 0; i < n; i++)
    {
        CXFile file = clang_Module_getTopLevelHeader(tu, mod, i);
        rb_yield(Data_Wrap_Struct(rb_cCXFile, NULL, RUBY_NEVER_FREE, file));
    }

    return self;
}

static VALUE mod_header_count(VALUE self)
{
    VALUE unit = rb_ivar_get(self, id_unit);
    if (NIL_P(unit))
        rb_raise(rb_eRuntimeError, "undefined translation unit for Module");
    return UINT2NUM(clang_Module_getNumTopLevelHeaders(DATA_PTR(unit), DATA_PTR(self)));
}

void Init_clang_module(void)
{
    rb_define_method2(rb_cCXModule, "initialize", mod_initialize, 2);
    rb_define_method0(rb_cCXModule, "parent", mod_parent, 0);
    rb_define_method0(rb_cCXModule, "name", mod_name, 0);
    rb_define_method0(rb_cCXModule, "full_name", mod_full_name, 0);
    rb_define_method0(rb_cCXModule, "system?", mod_is_system, 0);
    rb_define_method0(rb_cCXModule, "ast_file", mod_ast_file, 0);
    rb_define_method0(rb_cCXModule, "headers", mod_headers, 0);
    rb_define_method0(rb_cCXModule, "each_header", mod_each_header, 0);
    rb_define_method0(rb_cCXModule, "header_count", mod_header_count, 0);

    id_unit = rb_intern("@translation_unit");
}