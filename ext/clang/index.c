#include "clang.h"

static void index_free(void *data)
{
    if (data)
        clang_disposeIndex(data);
}

static VALUE index_get_opts(VALUE self)
{
    unsigned int mask = clang_CXIndex_getGlobalOptions(DATA_PTR(self));
    return rb_enum_unmask(rb_GlobalOptFlags, mask);
}

static VALUE index_set_opts(VALUE self, VALUE opts)
{
    unsigned int mask;
    if (SYMBOL_P(opts))
    {
        mask = rb_enum_value(rb_GlobalOptFlags, opts);
    }
    else if (RB_TYPE_P(opts, T_ARRAY))
    {
        mask = rb_enum_mask(rb_GlobalOptFlags, opts);
    }
    else
    {
        rb_raise(rb_eTypeError, "%s is not a Symbol or Array", CLASS_NAME(opts));
    }

    clang_CXIndex_setGlobalOptions(DATA_PTR(self), mask);
    return opts;
}

static VALUE index_create(int argc, VALUE *argv, VALUE klass)
{
    VALUE exclude, display, options;
    rb_scan_args(argc, argv, "02*", &exclude, &display, &options);


    CXIndex idx = clang_createIndex(RTEST(exclude), RTEST(display));
    
    if (RTEST(options))
    {
        unsigned int opts = rb_enum_mask(rb_GlobalOptFlags, options);
        clang_CXIndex_setGlobalOptions(idx, opts);
    }

    if (rb_block_given_p())
    {
        VALUE index = Data_Wrap_Struct(klass, NULL, RUBY_NEVER_FREE, idx);
        rb_yield(index);
        clang_disposeIndex(idx);
        return Qnil;
    }

    return Data_Wrap_Struct(klass, NULL, index_free, idx);
}

static VALUE index_emission_path(VALUE self, VALUE path)
{
    const char *p = RTEST(path) ? StringValueCStr(path) : NULL;
    clang_CXIndex_setInvocationEmissionPathOption(DATA_PTR(self), p);
    return path;
}

void Init_clang_index(void)
{
    rb_define_singleton_methodm1(rb_cCXIndex, "create", index_create, -1);
    rb_define_method0(rb_cCXIndex, "global_opts", index_get_opts, 0);
    rb_define_method1(rb_cCXIndex, "global_opts=", index_set_opts, 1);
    rb_define_method1(rb_cCXIndex, "emission_path", index_emission_path, 1);
}