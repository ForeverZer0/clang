#include "clang.h"

static VALUE result_kind(VALUE self)
{
    CXCompletionResult *result = DATA_PTR(self);
    return rb_enum_symbol(rb_CursorKind, result->CursorKind);
}

static VALUE result_completion(VALUE self)
{
    CXCompletionResult *result = DATA_PTR(self);
    return Data_Wrap_Struct(rb_cCXCompletionString, NULL, RUBY_NEVER_FREE, result);
}

static VALUE compstr_brief_comment(VALUE self)
{
    return RUBYSTR(clang_getCompletionBriefComment(DATA_PTR(self)));
}

static VALUE compstr_parent(VALUE self)
{
    return RUBYSTR(clang_getCompletionParent(DATA_PTR(self), NULL));
}

static VALUE compstr_availability(VALUE self)
{
    enum CXAvailabilityKind kind = clang_getCompletionAvailability(DATA_PTR(self));
    return rb_enum_symbol(rb_AvailabilityKind, kind);
}

static VALUE compstr_priority(VALUE self)
{
    return UINT2NUM(clang_getCompletionPriority(DATA_PTR(self)));
}

static VALUE compstr_annotation_count(VALUE self)
{
    return UINT2NUM(clang_getCompletionNumAnnotations(DATA_PTR(self)));
}

static VALUE compstr_annotations(VALUE self)
{
    CXCompletionString str = DATA_PTR(self);
    unsigned int n = clang_getCompletionNumAnnotations(str);
    VALUE ary = rb_ary_new_capa(n);

    for (unsigned i = 0; i < n; i++)
    {
        CXString s = clang_getCompletionAnnotation(str, i);
        rb_ary_store(ary, i, RUBYSTR(s));
    }
    return ary;
}

static VALUE compstr_each_annotation(VALUE self)
{
    RETURN_ENUMERATOR(self, 0, NULL);
    CXCompletionString str = DATA_PTR(self);
    unsigned int n = clang_getCompletionNumAnnotations(str);

    for (unsigned i = 0; i < n; i++)
    {
        CXString s = clang_getCompletionAnnotation(str, i);
        rb_yield(RUBYSTR(s));
    }

    return self;
}

static VALUE compstr_chunk_count(VALUE self)
{
    return UINT2NUM(clang_getNumCompletionChunks(DATA_PTR(self)));
}

static VALUE compstr_chunks(VALUE self)
{
    CXCompletionString str = DATA_PTR(self);
    unsigned int n = clang_getNumCompletionChunks(str);
    VALUE ary = rb_ary_new_capa(n);

    for (unsigned i = 0; i < n; i++)
    {
        enum CXCompletionChunkKind kind = clang_getCompletionChunkKind(str, i);
        CXString text = clang_getCompletionChunkText(str, i);
        CXCompletionString comp = clang_getCompletionChunkCompletionString(str, i);

        VALUE values = rb_ary_new_from_args(4,
            UINT2NUM(i),
            rb_enum_symbol(rb_CompletionChunkKind, kind),
            RUBYSTR(text),
            comp ? Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_NEVER_FREE, comp) : Qnil
        );

        rb_ary_store(ary, i, values);
    }

    return ary;
}

static VALUE compstr_each_chunk(VALUE self)
{
    RETURN_ENUMERATOR(self, 0, NULL);

    CXCompletionString str = DATA_PTR(self);
    unsigned int n = clang_getNumCompletionChunks(str);

    for (unsigned i = 0; i < n; i++)
    {
        enum CXCompletionChunkKind kind = clang_getCompletionChunkKind(str, i);
        CXString text = clang_getCompletionChunkText(str, i);
        CXCompletionString comp = clang_getCompletionChunkCompletionString(str, i);

        VALUE values = rb_ary_new_from_args(4,
            UINT2NUM(i),
            rb_enum_symbol(rb_CompletionChunkKind, kind),
            RUBYSTR(text),
            comp ? Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_NEVER_FREE, comp) : Qnil
        );

        rb_yield(values);
    }

    return self;
}

static VALUE results_alloc(VALUE klass)
{
    return Data_Wrap_Struct(klass, NULL, RUBY_NEVER_FREE, NULL);
}

static VALUE results_fixit_count(VALUE self, VALUE index)
{
    CXCodeCompleteResults *results = DATA_PTR(self);
    return UINT2NUM(clang_getCompletionNumFixIts(results, NUM2UINT(index)));
}

static VALUE results_each_fixit(VALUE self, VALUE index)
{
    RETURN_ENUMERATOR(self, 1, &index);

    CXCodeCompleteResults *results = DATA_PTR(self);
    unsigned int completion_index = NUM2UINT(index);
    unsigned int n = clang_getCompletionNumFixIts(results, completion_index);

    for (unsigned int i = 0; i < n; i++)
    {
        CXSourceRange range, *value = ALLOC(CXSourceRange);
        CXString str = clang_getCompletionFixIt(results, completion_index, i, &range);

        VALUE args = rb_ary_new_from_args(2, RUBYSTR(str), Data_Wrap_Struct(rb_cCXSourceRange, NULL, RUBY_DEFAULT_FREE, value));
        rb_yield(args);
    }

    return self;
}

static VALUE results_default_options(VALUE klass)
{
    return rb_enum_unmask(rb_CodeCompleteFlags, clang_defaultCodeCompleteOptions());
}

static VALUE results_size(VALUE self)
{
    return UINT2NUM(((CXCodeCompleteResults*)DATA_PTR(self))->NumResults);
}

static VALUE results_each(VALUE self)
{
    RETURN_ENUMERATOR(self, 0, NULL);

    CXCodeCompleteResults *results = DATA_PTR(self);
    for (unsigned i = 0; i < results->NumResults; i++)
    {
        CXCompletionResult *result = ALLOC(CXCompletionResult);
        *result = results->Results[i];
        rb_yield(Data_Wrap_Struct(rb_cCXCompletionResult, NULL, RUBY_DEFAULT_FREE, result));
    }
    return self;
}

static VALUE results_get(VALUE self, VALUE index)
{
    CXCodeCompleteResults *results = DATA_PTR(self);
    unsigned int i = NUM2UINT(index);

    if (i >= results->NumResults)
        return Qnil;

    CXCompletionResult *result = ALLOC(CXCompletionResult);
    *result = results->Results[i];
    return Data_Wrap_Struct(rb_cCXCompletionResult, NULL, RUBY_DEFAULT_FREE, result);
}

static VALUE results_sort_bang(VALUE self)
{
    CXCodeCompleteResults *results = DATA_PTR(self);
    clang_sortCodeCompletionResults(results->Results, results->NumResults);
    return self;
}

static VALUE results_diagnostic_count(VALUE self)
{
    CXCodeCompleteResults *results = DATA_PTR(self);
    return UINT2NUM(clang_codeCompleteGetNumDiagnostics(results));
}

static VALUE results_each_diagnostic(VALUE self)
{
    RETURN_ENUMERATOR(self, 0, NULL);

    CXCodeCompleteResults *results = DATA_PTR(self);
    unsigned int n = clang_codeCompleteGetNumDiagnostics(results);

    for (unsigned i = 0; i < n; i++)
    {
        CXDiagnostic diagnostic = clang_codeCompleteGetDiagnostic(results, i);
        if (!diagnostic)
            continue;
        rb_yield(Data_Wrap_Struct(rb_cCXDiagnostic, NULL, clang_disposeDiagnostic, diagnostic));
    }

    return self;
}

static VALUE results_contexts(VALUE self)
{
    CXCodeCompleteResults *results = DATA_PTR(self);
    rb_enum_unmask(rb_CompletionContext, clang_codeCompleteGetContexts(results));
}

static VALUE results_container(VALUE self)
{
    CXCodeCompleteResults *results = DATA_PTR(self);

    unsigned complete;
    enum CXCursorKind kind = clang_codeCompleteGetContainerKind(results, &complete);

    VALUE hash = rb_hash_new();
    rb_hash_aset(hash, STR2SYM("kind"), rb_enum_symbol(rb_CursorKind, kind));
    rb_hash_aset(hash, STR2SYM("complete"), RB_BOOL(complete));
    rb_hash_aset(hash, STR2SYM("usr"), RUBYSTR(clang_codeCompleteGetContainerUSR(results)));
    rb_hash_aset(hash, STR2SYM("selector"), RUBYSTR(clang_codeCompleteGetObjCSelector(results)));
    return hash;
}

void Init_clang_completion(void)
{
    // CXCompletionResult
    rb_define_method0(rb_cCXCompletionResult, "kind", result_kind, 0);
    rb_define_method0(rb_cCXCompletionResult, "completion", result_completion, 0);
    
    // CXCompletionString
    rb_define_method0(rb_cCXCompletionString, "brief_comment", compstr_brief_comment, 0);
    rb_define_method0(rb_cCXCompletionString, "parent", compstr_parent, 0);
    rb_define_method0(rb_cCXCompletionString, "availability", compstr_availability, 0);
    rb_define_method0(rb_cCXCompletionString, "priority", compstr_priority, 0);
    rb_define_method0(rb_cCXCompletionString, "annotation_count", compstr_annotation_count, 0);
    rb_define_method0(rb_cCXCompletionString, "annotations", compstr_annotations, 0);
    rb_define_method0(rb_cCXCompletionString, "each_annotation", compstr_each_annotation, 0);
    rb_define_method0(rb_cCXCompletionString, "chunk_count", compstr_chunk_count, 0);
    rb_define_method0(rb_cCXCompletionString, "chunks", compstr_chunks, 0);
    rb_define_method0(rb_cCXCompletionString, "each_chunk", compstr_each_chunk, 0);

    // CXCodeCompleteResults
    rb_define_singleton_method0(rb_cCXCodeCompleteResults, "default_options", results_default_options, 0);
    rb_include_module(rb_cCXCodeCompleteResults, rb_mEnumerable);
    rb_define_method1(rb_cCXCodeCompleteResults, "fixit_count", results_fixit_count, 1);
    rb_define_method1(rb_cCXCodeCompleteResults, "each_fixit", results_each_fixit, 1);
    rb_define_method0(rb_cCXCodeCompleteResults, "size", results_size, 0);
    rb_define_method0(rb_cCXCodeCompleteResults, "each", results_each, 0);
    rb_define_method1(rb_cCXCodeCompleteResults, "[]", results_get, 1);
    rb_define_method0(rb_cCXCodeCompleteResults, "sort!", results_sort_bang, 0);
    rb_define_method0(rb_cCXCodeCompleteResults, "contexts", results_contexts, 0);
    rb_define_method0(rb_cCXCodeCompleteResults, "diagnostic_count", results_diagnostic_count, 0);
    rb_define_method0(rb_cCXCodeCompleteResults, "each_diagnostic", results_each_diagnostic, 0);
    rb_define_method0(rb_cCXCodeCompleteResults, "container", results_container, 0);
    rb_define_alias(rb_cCXCodeCompleteResults, "length", "size");
}