#include "clang.h"

typedef struct {
    unsigned int count;
    CXToken *tokens;
    VALUE unit;
} rb_tokenset;

typedef struct {
    CXToken token;
    VALUE unit;
} rb_token;

static void tokenset_mark(void *data)
{
    rb_tokenset *set = data;
    rb_gc_mark(set->unit);
}

static void token_mark(void *data)
{
    rb_token *set = data;
    rb_gc_mark(set->unit);
}

static void token_free(void *data)
{
    rb_token *t = data;
    if (!NIL_P(t->unit))
        clang_disposeTokens(DATA_PTR(t->unit), &t->token, 1);
    xfree(data);
}

static void tokenset_free(void *data)
{
    if (!data)
        return;

    rb_tokenset *set = data;
    if (!NIL_P(set->unit))
        clang_disposeTokens(DATA_PTR(set->unit), set->tokens, set->count);
    xfree(set);
}

static VALUE token_alloc(VALUE klass)
{
    rb_token *t = ALLOC(rb_token);
    memset(t, 0, sizeof(rb_token));
    t->unit = Qnil;
    return Data_Wrap_Struct(klass, token_mark, RUBY_DEFAULT_FREE, t);
}

static VALUE tokenset_alloc(VALUE klass)
{
    rb_tokenset *set = ALLOC(rb_tokenset);
    memset(set, 0, sizeof(rb_tokenset));
    set->unit = Qnil;
    return Data_Wrap_Struct(rb_cCXTokenSet, tokenset_mark, tokenset_free, set);
}

static VALUE tokenset_initialize(VALUE self, VALUE unit, VALUE range)
{
    rb_assert_type(unit, rb_cCXTranslationUnit);
    rb_assert_type(range, rb_cCXSourceRange);

    rb_tokenset *set = DATA_PTR(self);
    set->unit = unit;

    CXSourceRange *r = DATA_PTR(range);
    clang_tokenize(DATA_PTR(unit), *r, &set->tokens, &set->count);
    return self;
}

static VALUE tokenset_get(VALUE self, VALUE index)
{
    rb_tokenset *set = DATA_PTR(self);
    unsigned int i = NUM2UINT(index);
    if (i >= set->count)
        return Qnil;

    rb_token *t = ALLOC(rb_token);
    t->unit = set->unit;
    t->token = set->tokens[i];

    return Data_Wrap_Struct(rb_cCXToken, token_mark, RUBY_DEFAULT_FREE, t);
}

static VALUE tokenset_each(VALUE self)
{
    RETURN_ENUMERATOR(self, 0, NULL);
    rb_tokenset *set = DATA_PTR(self);

    for (unsigned i = 0; i < set->count; i++)
    {
        rb_token *t = ALLOC(rb_token);
        t->unit = set->unit;
        t->token = set->tokens[i]; 
        rb_yield(Data_Wrap_Struct(rb_cCXToken, token_mark, RUBY_DEFAULT_FREE, t));
    }
    return self;
}

static VALUE tokenset_size(VALUE self)
{
    return UINT2NUM(((rb_tokenset*)DATA_PTR(self))->count);
}

static VALUE tokenset_unit(VALUE self)
{
    return ((rb_tokenset*)DATA_PTR(self))->unit;
}

static VALUE token_unit(VALUE self)
{
    return ((rb_token*)DATA_PTR(self))->unit;
}

static VALUE token_initialize(VALUE self, VALUE unit, VALUE location)
{
    rb_assert_type(unit, rb_cCXTranslationUnit);
    rb_assert_type(location, rb_cCXSourceLocation);

    CXToken *token = clang_getToken(DATA_PTR(unit), *(CXSourceLocation*) DATA_PTR(location));
    if (!token)
        rb_raise(rb_eRuntimeError, "failed to create Token from specified location");

    rb_token *t = DATA_PTR(self);
    t->token = *token;
    t->unit = unit;
    RDATA(self)->dfree = token_free;

    return self;
}

static VALUE token_kind(VALUE self)
{
    rb_token *t = DATA_PTR(self);
    enum CXTokenKind kind = clang_getTokenKind(t->token);
    return rb_enum_symbol(rb_TokenKind, kind);
}

static VALUE token_location(VALUE self)
{
    CXSourceLocation *location = ALLOC(CXSourceLocation);
    rb_token *t = DATA_PTR(self);
    *location = clang_getTokenLocation(DATA_PTR(t->unit), t->token);
    return Data_Wrap_Struct(rb_cCXSourceLocation, NULL, RUBY_DEFAULT_FREE, location);
}

static VALUE token_spelling(VALUE self)
{
    rb_token *t = DATA_PTR(self);
    return RUBYSTR(clang_getTokenSpelling(DATA_PTR(t->unit), t->token));
}

static VALUE token_extent(VALUE self)
{
    CXSourceRange *range = ALLOC(CXSourceRange);
    rb_token *t = DATA_PTR(self);
    *range = clang_getTokenExtent(DATA_PTR(t->unit), t->token);
    return Data_Wrap_Struct(rb_cCXSourceRange, NULL, RUBY_DEFAULT_FREE, range);
}

static VALUE tu_tokenize(VALUE self, VALUE range)
{
    rb_assert_type(range, rb_cCXSourceRange);
    rb_tokenset *set = ALLOC(rb_tokenset);
    set->unit = self;
    clang_tokenize(DATA_PTR(self), *(CXSourceRange*) DATA_PTR(range), &set->tokens, &set->count);
    return Data_Wrap_Struct(rb_cCXTokenSet, tokenset_mark, tokenset_free, set);
}

static VALUE tokenset_cursors(VALUE self)
{
    rb_tokenset *set = DATA_PTR(self);
    CXCursor cursors[set->count];
    clang_annotateTokens(DATA_PTR(set->unit), set->tokens, set->count, cursors);

    VALUE ary = rb_ary_new_capa(set->count);
    for (unsigned i = 0; i < set->count; i++)
    {
        CXCursor *cursor = ALLOC(CXCursor);
        *cursor = cursors[i];
        rb_ary_store(ary, i, Data_Wrap_Struct(rb_cCXCursor, NULL, RUBY_DEFAULT_FREE, cursor));
    }
    return ary;
}

static VALUE tokenset_annotate(VALUE self)
{
    rb_need_block();
    rb_tokenset *set = DATA_PTR(self);
    CXCursor cursors[set->count];
    clang_annotateTokens(DATA_PTR(set->unit), set->tokens, set->count, cursors);

    rb_token t;
    t.unit = set->unit;
    VALUE args = rb_ary_new_capa(2);
    for (unsigned i = 0; i < set->count; i++)
    {
        t.token = set->tokens[i];
        rb_ary_store(args, 0, Data_Wrap_Struct(rb_cCXToken, NULL, RUBY_NEVER_FREE, &t));
        rb_ary_store(args, 1, Data_Wrap_Struct(rb_cCXCursor, NULL, RUBY_NEVER_FREE, &cursors[i]));
        rb_yield(args);
    }

    return Qnil;
}

void Init_clang_token(void)
{
    rb_define_method1(rb_cCXTranslationUnit, "tokenize", tu_tokenize, 1);

    rb_cCXTokenSet = rb_define_class_under(rb_mClang, "TokenSet", rb_cObject);
    rb_define_alloc_func(rb_cCXTokenSet, tokenset_alloc);
    rb_include_module(rb_cCXTokenSet, rb_mEnumerable);
    rb_define_method2(rb_cCXTokenSet, "initialize", tokenset_initialize, 2);
    rb_define_method0(rb_cCXTokenSet, "translation_unit", tokenset_unit, 0);
    rb_define_method0(rb_cCXTokenSet, "size", tokenset_size, 0);
    rb_define_method0(rb_cCXTokenSet, "each", tokenset_each, 0);
    rb_define_method0(rb_cCXTokenSet, "cursors", tokenset_cursors, 0);
    rb_define_method0(rb_cCXTokenSet, "annotate", tokenset_annotate, 0);
    rb_define_method1(rb_cCXTokenSet, "[]", tokenset_get, 1);
    rb_define_alias(rb_cCXTokenSet, "length", "size");

    rb_define_method2(rb_cCXToken, "initialize", token_initialize, 2);
    rb_define_method0(rb_cCXToken, "translation_unit", token_unit, 0);
    rb_define_method0(rb_cCXToken, "kind", token_kind, 0);
    rb_define_method0(rb_cCXToken, "location", token_location, 0);
    rb_define_method0(rb_cCXToken, "extent", token_extent, 0);
    rb_define_method0(rb_cCXToken, "spelling", token_spelling, 0);
    rb_define_method0(rb_cCXToken, "to_s", token_spelling, 0);
    rb_define_method0(rb_cCXToken, "to_str", token_spelling, 0);

    rb_define_alias(rb_cCXToken, "range", "extent");
}