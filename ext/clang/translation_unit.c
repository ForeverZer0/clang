#include "clang.h"

void clang_dset_free(void *data);
void clang_diagnostic_free(void *data);


static inline void tu_check_error(enum CXErrorCode code)
{
    switch (code)
    {
        case CXError_Success: return;
        case CXError_Crashed: rb_raise(rb_eFatal, "native library crashed");
        case CXError_InvalidArguments: rb_raise(rb_eArgError, "invalid Clang arguments specified");
        case CXError_ASTReadError: rb_raise(rb_eLoadError, "an AST deserialization error has occurred");
        default: rb_raise(rb_eRuntimeError, "failed to create translation unit");
    }
}

static void tu_free(void *data)
{
    if (data)
        clang_disposeTranslationUnit(data);
}

static VALUE tu_skipped_ranges(int argc, VALUE *argv, VALUE self)
{
    VALUE file;
    rb_scan_args(argc, argv, "01", &file);

    CXTranslationUnit unit = DATA_PTR(self);
    CXSourceRangeList *list;

    if (RTEST(file))
    {
        rb_assert_type(file, rb_cCXFile);
        list = clang_getSkippedRanges(unit, DATA_PTR(file));
    }
    else
    {
        list = clang_getAllSkippedRanges(unit);
    }

    if (!list)
        return rb_ary_new_capa(0);
    VALUE ary = rb_ary_new_capa(list->count);

    for (unsigned i = 0; i < list->count; i++)
    {
        CXSourceRange *range = ALLOC(CXSourceRange);
        *range = list->ranges[i];
        rb_ary_store(ary, i, Data_Wrap_Struct(rb_cCXSourceRange, NULL, RUBY_DEFAULT_FREE, range));
    }

    clang_disposeSourceRangeList(list);
    return ary;
}

static VALUE tu_each_diagnostic(VALUE self)
{
    RETURN_ENUMERATOR(self, 0, NULL);

    CXTranslationUnit unit = DATA_PTR(self);
    unsigned int n = clang_getNumDiagnostics(unit);
    for (unsigned i = 0; i < n; i++)
    {
        CXDiagnostic d = clang_getDiagnostic(unit, i);
        rb_yield(Data_Wrap_Struct(rb_cCXDiagnostic, NULL, clang_diagnostic_free, d));
    }
    return self;
}

static VALUE tu_diagnostics(VALUE self)
{
    CXTranslationUnit unit = DATA_PTR(self);
    CXDiagnosticSet set = clang_getDiagnosticSetFromTU(unit);
    return set ? Data_Wrap_Struct(rb_cCXDiagnosticSet, NULL, clang_dset_free, set) : Qnil;
}

static VALUE tu_diagnostic_count(VALUE self)
{
    CXTranslationUnit unit = DATA_PTR(self);
    return UINT2NUM(clang_getNumDiagnostics(unit));
}

static VALUE tu_spelling(VALUE self)
{
    CXString str = clang_getTranslationUnitSpelling(DATA_PTR(self));
    return RUBYSTR(str);
}

static VALUE tu_parse(int argc, VALUE *argv, VALUE klass)
{
    VALUE index, source, args, unsaved, opts;
    rb_scan_args(argc, argv, "13*", &index, &source, &args, &unsaved, &opts);

    rb_assert_type(index, rb_cCXIndex);
    CXIndex idx = DATA_PTR(index);

    const char *src = RTEST(source) ? StringValueCStr(source) : NULL;
    int num_cmds = RTEST(args) ? rb_array_len(args) : 0;
    int num_file = RTEST(unsaved) ? rb_array_len(unsaved) : 0;

    const char *cmds[num_cmds];
    for (int i = 0; i < num_cmds; i++)
    {
        VALUE s = rb_ary_entry(args, i);
        cmds[i] = StringValueCStr(s);
    }

    struct CXUnsavedFile files[num_file];
    for (int i = 0; i < num_file; i++)
    {
        VALUE s = rb_ary_entry(unsaved, i);
        rb_assert_type(s, rb_cCXUnsavedFile);
        files[i] = *(struct CXUnsavedFile*) DATA_PTR(s);
    }

    unsigned int mask = rb_enum_mask(rb_TranslationUnitFlags, opts);
    CXTranslationUnit unit;
    enum CXErrorCode err = clang_parseTranslationUnit2(idx, src, cmds, num_cmds, files, num_file, mask, &unit);
    tu_check_error(err);

    return Data_Wrap_Struct(klass, NULL, tu_free, unit);
}

static VALUE tu_from_source(int argc, VALUE *argv, VALUE klass)
{
    VALUE index, source, args, unsaved;
    rb_scan_args(argc, argv, "13", &index, &source, &args, &unsaved);

    rb_assert_type(index, rb_cCXIndex);
    CXIndex idx = DATA_PTR(index);

    const char *src = RTEST(source) ? StringValueCStr(source) : NULL;
    int num_cmds = RTEST(args) ? rb_array_len(args) : 0;
    int num_file = RTEST(unsaved) ? rb_array_len(unsaved) : 0;

    const char *cmds[num_cmds];
    for (int i = 0; i < num_cmds; i++)
    {
        VALUE s = rb_ary_entry(args, i);
        cmds[i] = StringValueCStr(s);
    }

    struct CXUnsavedFile files[num_file];
    for (int i = 0; i < num_file; i++)
    {
        VALUE s = rb_ary_entry(unsaved, i);
        rb_assert_type(s, rb_cCXUnsavedFile);
        files[i] = *(struct CXUnsavedFile*) DATA_PTR(s);
    }

    CXTranslationUnit unit = clang_createTranslationUnitFromSourceFile(idx, src, num_cmds, cmds, num_file, files);
    if (!unit)
        rb_raise(rb_eRuntimeError, "failed to create translation unit");

    return Data_Wrap_Struct(klass, NULL, tu_free, unit);
}

static VALUE tu_default_options(VALUE klass)
{
    int mask = clang_defaultEditingTranslationUnitOptions();
    return rb_enum_unmask(rb_TranslationUnitFlags, mask);
}

static VALUE tu_default_reparse_options(VALUE self)
{
    int mask = clang_defaultReparseOptions(DATA_PTR(self));
    return rb_enum_unmask(rb_ReparseFlags, mask);
}

static VALUE tu_default_save_options(VALUE self)
{
    int mask = clang_defaultSaveOptions(DATA_PTR(self));
    return rb_enum_unmask(rb_SaveTranslationUnitFlags, mask);
}

static VALUE tu_initialize(VALUE self, VALUE index, VALUE ast_path)
{
    rb_assert_type(index, rb_cCXIndex);
    CXIndex idx = DATA_PTR(index);
    const char *ast = StringValueCStr(ast_path);

    enum CXErrorCode err = clang_createTranslationUnit2(idx, ast, DATA_PTR(self));
    tu_check_error(err);

    RDATA(self)->dfree = tu_free;
    return self;
}

static VALUE tu_save(int argc, VALUE *argv, VALUE self)
{
    VALUE filename, options;
    rb_scan_args(argc, argv, "1*", &filename, &options);

    unsigned int mask = RTEST(options) ? rb_enum_mask(rb_SaveTranslationUnitFlags, options) : CXSaveTranslationUnit_None;
    enum CXSaveError err = clang_saveTranslationUnit(DATA_PTR(self), StringValueCStr(filename), mask);

    switch (err)
    {
        case CXSaveError_None: return self;
        case CXSaveError_Unknown: rb_raise(rb_eRuntimeError, "failed to save translation unit");
        case CXSaveError_TranslationErrors: rb_raise(rb_eRuntimeError, "the translation unit contains error(s), cannot save");
        case CXSaveError_InvalidTU: rb_raise(rb_eArgError, "invalid translation unit");
        default: rb_raise(rb_eRuntimeError, "failed to save translation unit");
    }
}

static VALUE tu_suspend(VALUE self)
{
    return RB_BOOL(clang_suspendTranslationUnit(DATA_PTR(self)));
}

static VALUE tu_reparse(int argc, VALUE *argv, VALUE self)
{
    VALUE unsaved, options;
    rb_scan_args(argc, argv, "01*", &unsaved, &options);
    unsigned int mask = rb_enum_mask(rb_ReparseFlags, options);
    
    int num_file = RTEST(unsaved) ? rb_array_len(unsaved) : 0;
    struct CXUnsavedFile files[num_file];
    for (int i = 0; i < num_file; i++)
    {
        VALUE s = rb_ary_entry(unsaved, i);
        rb_assert_type(s, rb_cCXUnsavedFile);
        files[i] = *(struct CXUnsavedFile*) DATA_PTR(s);
    }

    enum CXErrorCode err = clang_reparseTranslationUnit(DATA_PTR(self), num_file, files, mask);
    tu_check_error(err);
    return self;
}

static VALUE tu_cursor(VALUE self)
{
    CXCursor *cursor = ALLOC(CXCursor);
    *cursor = clang_getTranslationUnitCursor(DATA_PTR(self));
    return Data_Wrap_Struct(rb_cCXCursor, NULL, RUBY_DEFAULT_FREE, cursor);
}

static VALUE tu_include_guarded(VALUE self, VALUE file)
{
    rb_assert_type(file, rb_cCXFile);
    return RB_BOOL(clang_isFileMultipleIncludeGuarded(DATA_PTR(self), DATA_PTR(file)));
}

static VALUE tu_resource_usage(VALUE self)
{
    CXTUResourceUsage usage = clang_getCXTUResourceUsage(DATA_PTR(self));
    VALUE key, value, hash = rb_hash_new();
    for (unsigned i = 0; i < usage.numEntries; i++)
    {
        key = rb_str_new_cstr(clang_getTUResourceUsageName(usage.entries[i].kind));
        value = ULL2NUM(usage.entries[i].amount);
        rb_hash_aset(hash, key, value);
    }
    clang_disposeCXTUResourceUsage(usage);
    return hash;
}

static VALUE tu_target_info(VALUE self)
{
    CXTargetInfo info = clang_getTranslationUnitTargetInfo(DATA_PTR(self));
    VALUE hash = rb_hash_new();

    rb_hash_aset(hash, STR2SYM("platform"), RUBYSTR(clang_TargetInfo_getTriple(info)));
    rb_hash_aset(hash, STR2SYM("pointer_width"), INT2NUM(clang_TargetInfo_getPointerWidth(info)));

    clang_TargetInfo_dispose(info);
    return hash;
}

static VALUE tu_code_complete(int argc, VALUE *argv, VALUE self)
{
    VALUE filename, line, column, unsaved, options;
    rb_scan_args(argc, argv, "4*", &filename, &line, &column, &unsaved, &options);

    const char *path = StringValueCStr(filename);
    unsigned int l = NUM2UINT(line), c = UINT2NUM(column); 
    unsigned int num_unsaved = RTEST(unsaved) ? rb_array_len(unsaved) : 0;
    struct CXUnsavedFile files[num_unsaved];
    for (unsigned i = 0; i < num_unsaved; i++)
    {
        VALUE f = rb_ary_entry(unsaved, i);
        rb_assert_type(f, rb_cCXUnsavedFile);
        files[i] = *(struct CXUnsavedFile*)DATA_PTR(f);
    }

    unsigned int opts;
    if (NIL_P(options) || rb_array_len(options) == 0)
        opts = clang_defaultCodeCompleteOptions();
    else
        opts = rb_enum_mask(rb_CodeCompleteFlags, options);

    CXCodeCompleteResults *results = clang_codeCompleteAt(DATA_PTR(self), path, l, c, files, num_unsaved, opts);
    return results ? Data_Wrap_Struct(rb_cCXCodeCompleteResults, NULL, (RUBY_DATA_FUNC) clang_disposeCodeCompleteResults, results) : Qnil;
}

static void tu_inclusion_visitor(CXFile included_file, CXSourceLocation *inclusion_stack, unsigned include_len, CXClientData client_data)
{
    VALUE proc = *(VALUE*)client_data;

    VALUE stack = rb_ary_new_capa(include_len);
    for (unsigned i = 0; i < include_len; i++)
    {
        CXSourceLocation *location = ALLOC(CXSourceLocation);
        *location = inclusion_stack[i];
        rb_ary_store(stack, i, Data_Wrap_Struct(rb_cCXSourceLocation, NULL, RUBY_DEFAULT_FREE, location));
    }

    VALUE file = Data_Wrap_Struct(rb_cCXFile, NULL, RUBY_NEVER_FREE, included_file);
    rb_proc_call(proc, rb_ary_new_from_args(2, file, stack));
}

static VALUE tu_inclusions(VALUE self)
{
    rb_need_block();
    VALUE proc = rb_block_proc();
    clang_getInclusions(DATA_PTR(self), tu_inclusion_visitor, &proc);
    return Qnil;
}


void Init_clang_translation_unit(void)
{
    rb_define_singleton_methodm1(rb_cCXTranslationUnit, "parse", tu_parse, -1);
    rb_define_singleton_methodm1(rb_cCXTranslationUnit, "from_source", tu_from_source, -1);
    rb_define_singleton_method0(rb_cCXTranslationUnit, "default_options", tu_default_options, 0);

    rb_define_method2(rb_cCXTranslationUnit, "initialize", tu_initialize, 2);
    rb_define_method0(rb_cCXTranslationUnit, "default_save_options", tu_default_save_options, 0);
    rb_define_method0(rb_cCXTranslationUnit, "default_reparse_options", tu_default_reparse_options, 0);
    rb_define_methodm1(rb_cCXTranslationUnit, "save", tu_save, -1);
    rb_define_method0(rb_cCXTranslationUnit, "resource_usage", tu_resource_usage, 0);
    rb_define_methodm1(rb_cCXTranslationUnit, "skipped_ranges", tu_skipped_ranges, -1);
    rb_define_method0(rb_cCXTranslationUnit, "spelling", tu_spelling, 0);
    rb_define_method0(rb_cCXTranslationUnit, "suspend", tu_suspend, 0);
    rb_define_method0(rb_cCXTranslationUnit, "inclusions", tu_inclusions, 0);
    rb_define_methodm1(rb_cCXTranslationUnit, "reparse", tu_reparse, -1);
    rb_define_method0(rb_cCXTranslationUnit, "cursor", tu_cursor, 0);
    rb_define_method1(rb_cCXTranslationUnit, "include_guarded?", tu_include_guarded, 1);
    rb_define_method0(rb_cCXTranslationUnit, "diagnostic_count", tu_diagnostic_count, 0);
    rb_define_method0(rb_cCXTranslationUnit, "each_diagnostic", tu_each_diagnostic, 0);
    rb_define_method0(rb_cCXTranslationUnit, "diagnostics", tu_diagnostics, 0);
    rb_define_method0(rb_cCXTranslationUnit, "target_info", tu_target_info, 0);
    rb_define_methodm1(rb_cCXTranslationUnit, "code_complete", tu_code_complete, -1);
}

