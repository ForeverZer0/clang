#include "clang.h"

static void remap_free(void *data)
{
    if (data)
        clang_remap_dispose(data);
}

static VALUE remap_initialize(VALUE self, VALUE paths)
{
    unsigned int n = (unsigned int) rb_array_len(paths);
    if (n < 1)
        rb_raise(rb_eArgError, "wrong number of arguments (given 0, expected 1 or more)");

    const char *filenames[n];
    for (unsigned i = 0; i < n; i++)
    {
        VALUE path = rb_ary_entry(paths, i);
        filenames[i] = StringValueCStr(path);
    }

    CXRemapping remap = clang_getRemappingsFromFileList(filenames, n);
    if (!remap)
        rb_raise(rb_eLoadError, "failed to load remapping metadata");

    RDATA(self)->data = remap;
    RDATA(self)->dfree = remap_free;
    return self;
}

static VALUE remap_size(VALUE self)
{
    return UINT2NUM(clang_remap_getNumFiles(DATA_PTR(self)));
}

static VALUE remap_each(VALUE self)
{
    RETURN_ENUMERATOR(self, 0, NULL);

    CXRemapping remap = DATA_PTR(self);
    unsigned int n = clang_remap_getNumFiles(remap);
    CXString original, transform;
    for (unsigned int i = 0; i < n; i++)
    {
        clang_remap_getFilenames(remap, i, &original, &transform);
        rb_yield(rb_ary_new_from_args(2, RUBYSTR(original), RUBYSTR(transform)));
    }

    return self;
}

static VALUE remap_originals(VALUE self)
{
    CXRemapping remap = DATA_PTR(self);
    unsigned int n = clang_remap_getNumFiles(remap);
    VALUE ary = rb_ary_new_capa(n);

    CXString str;
    for (unsigned int i = 0; i < n; i++)
    {
        clang_remap_getFilenames(remap, i, &str, NULL);
        rb_ary_store(ary, i, RUBYSTR(str));
    }

    return ary;
}

static VALUE remap_transformations(VALUE self)
{
    CXRemapping remap = DATA_PTR(self);
    unsigned int n = clang_remap_getNumFiles(remap);
    VALUE ary = rb_ary_new_capa(n);

    CXString str;
    for (unsigned int i = 0; i < n; i++)
    {
        clang_remap_getFilenames(remap, i, NULL, &str);
        rb_ary_store(ary, i, RUBYSTR(str));
    }

    return ary;
}

static VALUE remap_to_h(VALUE self)
{
    CXRemapping remap = DATA_PTR(self);
    unsigned int n = clang_remap_getNumFiles(remap);
    VALUE hash = rb_hash_new();

    CXString key, value;
    for (unsigned int i = 0; i < n; i++)
    {
        clang_remap_getFilenames(remap, i, &key, &value);
        rb_hash_aset(hash, RUBYSTR(key), RUBYSTR(value));
    }

    return hash;
}

void Init_clang_remapping(void)
{
    rb_define_methodm2(rb_cCXRemapping, "initialize", remap_initialize, -2);
    rb_include_module(rb_cCXRemapping, rb_mEnumerable);
    rb_define_method0(rb_cCXRemapping, "size", remap_size, 0);
    rb_define_method0(rb_cCXRemapping, "each", remap_each, 0);
    rb_define_method0(rb_cCXRemapping, "inputs", remap_originals, 0);
    rb_define_method0(rb_cCXRemapping, "outputs", remap_transformations, 0);
    rb_define_method0(rb_cCXRemapping, "to_h", remap_to_h, 0);

    rb_define_alias(rb_cCXRemapping, "length", "size");
    rb_define_alias(rb_cCXRemapping, "to_hash", "to_h");
}