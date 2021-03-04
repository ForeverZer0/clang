#include "clang.h"

#define ALLOC_RECORD(name, type)                                                                                       \
    static VALUE alloc_##name(VALUE klass)                                                                             \
    {                                                                                                                  \
        type *obj = ALLOC(type);                                                                                       \
        memset(obj, 0, sizeof(type));                                                                                  \
        return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, obj);                                                  \
    }

static VALUE alloc_null(VALUE klass)
{
    return Data_Wrap_Struct(klass, NULL, RUBY_NEVER_FREE, NULL);
}

ALLOC_RECORD(unsaved_file, struct CXUnsavedFile);
ALLOC_RECORD(source_location, CXSourceLocation);
ALLOC_RECORD(source_range, CXSourceRange);
ALLOC_RECORD(cursor, CXCursor);
ALLOC_RECORD(platform_availability, CXPlatformAvailability);
ALLOC_RECORD(type, CXType);
ALLOC_RECORD(token, CXToken);
ALLOC_RECORD(completion_result, CXCompletionResult);

VALUE rb_mClang;
VALUE rb_cCXUnsavedFile;
VALUE rb_cCXVirtualFileOverlay;
VALUE rb_cCXModuleMapDescriptor;
VALUE rb_cCXIndex;
VALUE rb_cCXTargetInfo;
VALUE rb_cCXTranslationUnit;
VALUE rb_cCXFile;
VALUE rb_cCXSourceLocation;
VALUE rb_cCXSourceRange;
VALUE rb_cCXDiagnostic;
VALUE rb_cCXDiagnosticSet;
VALUE rb_cCXCursor;
VALUE rb_cCXPlatformAvailability;
VALUE rb_cCXType;
VALUE rb_cCXPrintingPolicy;
VALUE rb_cCXModule;
VALUE rb_cCXToken;
VALUE rb_cCXTokenSet;
VALUE rb_cCXCompletionString;
VALUE rb_cCXCompletionResult;
VALUE rb_cCXCodeCompleteResults;
VALUE rb_cCXRemapping;

void Init_clang_enums(void);
void Init_clang_source_location(void);
void Init_clang_source_range(void);
void Init_clang_translation_unit(void);
void Init_clang_diagnostic(void);
void Init_clang_cursor(void);
void Init_clang_index(void);
void Init_clang_type(void);
void Init_clang_comment(void);
void Init_clang_printing_policy(void);
void Init_clang_token(void);
void Init_clang_remapping(void);
void Init_clang_file(void);
void Init_clang_module(void);
void Init_clang_completion(void);

static VALUE clang_version(VALUE clang)
{
    return RUBYSTR(clang_getClangVersion());
}

static VALUE clang_crash_recovery(VALUE clang, VALUE enabled)
{
    clang_toggleCrashRecovery(RTEST(enabled));
    return enabled;
}

static VALUE virtual_file_initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE case_sensitive;
    rb_scan_args(argc, argv, "01", &case_sensitive);

    CXVirtualFileOverlay overlay = clang_VirtualFileOverlay_create(0);
    clang_VirtualFileOverlay_setCaseSensitivity(overlay, argc == 0 || RTEST(case_sensitive));

    struct RData *f = RDATA(self);
    f->data = overlay;
    f->dfree = (RUBY_DATA_FUNC) clang_VirtualFileOverlay_dispose;
    return self;
}

static VALUE virtual_file_map(VALUE self, VALUE virtual_path, VALUE real_path)
{
    CXVirtualFileOverlay overlay = DATA_PTR(self);
    const char *vp = StringValueCStr(virtual_path);
    const char *rp = StringValueCStr(real_path);

    enum CXErrorCode result = clang_VirtualFileOverlay_addFileMapping(overlay, vp, rp);
    return RB_BOOL(result == CXError_Success);
}

static VALUE virtual_file_read(VALUE self)
{
    CXVirtualFileOverlay overlay = DATA_PTR(self);

    char *str;
    unsigned int size;
    enum CXErrorCode error = clang_VirtualFileOverlay_writeToBuffer(overlay, 0, &str, &size);
    if (error != CXError_Success)
        rb_raise(rb_eRuntimeError, "failed to read virtual file");

    VALUE value = rb_str_new(str, size);
    clang_free(str);
    return value;
}

static VALUE modmap_initialize(VALUE self, VALUE mod_name, VALUE header)
{
    const char *name = StringValueCStr(mod_name);
    const char *umbrella = StringValueCStr(header);

    enum CXErrorCode error[2];
    CXModuleMapDescriptor mod = clang_ModuleMapDescriptor_create(0);

    error[0] = clang_ModuleMapDescriptor_setFrameworkModuleName(mod, name);
    error[1] = clang_ModuleMapDescriptor_setUmbrellaHeader(mod, umbrella);
    if (error[0] != CXError_Success || error[1] != CXError_Success)
        rb_raise(rb_eRuntimeError, "failed to configure descriptor");

    struct RData *f = RDATA(self);
    f->data = mod;
    f->dfree = (RUBY_DATA_FUNC) clang_ModuleMapDescriptor_dispose;
    return self;
}

static VALUE modmap_read(VALUE self)
{
    CXModuleMapDescriptor mod = DATA_PTR(self);

    char *str;
    unsigned int size;
    enum CXErrorCode error = clang_ModuleMapDescriptor_writeToBuffer(mod, 0, &str, &size);
    if (error != CXError_Success)
        rb_raise(rb_eRuntimeError, "failed to read virtual file");

    VALUE value = rb_str_new(str, size);
    clang_free(str);
    return value;
}

void Init_clang(void)
{
    rb_mClang = rb_define_module("Clang");
    rb_define_singleton_method0(rb_mClang, "version", clang_version, 0);
    rb_define_singleton_method1(rb_mClang, "crash_recovery", clang_crash_recovery, 1);

    rb_cCXUnsavedFile = rb_define_class_under(rb_mClang, "UnsavedFile", rb_cObject);
    rb_cCXVirtualFileOverlay = rb_define_class_under(rb_mClang, "VirtualFileOverlay", rb_cObject);
    rb_cCXModuleMapDescriptor = rb_define_class_under(rb_mClang, "ModuleMapDescriptor", rb_cObject);
    rb_cCXIndex = rb_define_class_under(rb_mClang, "Index", rb_cObject);
    rb_cCXTargetInfo = rb_define_class_under(rb_mClang, "TargetInfo", rb_cObject);
    rb_cCXTranslationUnit = rb_define_class_under(rb_mClang, "TranslationUnit", rb_cObject);
    rb_cCXFile = rb_define_class_under(rb_mClang, "File", rb_cObject);
    rb_cCXSourceLocation = rb_define_class_under(rb_mClang, "SourceLocation", rb_cObject);
    rb_cCXSourceRange = rb_define_class_under(rb_mClang, "SourceRange", rb_cObject);
    rb_cCXDiagnostic = rb_define_class_under(rb_mClang, "Diagnostic", rb_cObject);
    rb_cCXDiagnosticSet = rb_define_class_under(rb_mClang, "DiagnosticSet", rb_cObject);
    rb_cCXCursor = rb_define_class_under(rb_mClang, "Cursor", rb_cObject);
    rb_cCXPlatformAvailability = rb_define_class_under(rb_mClang, "PlatformAvailability", rb_cObject);
    rb_cCXType = rb_define_class_under(rb_mClang, "Type", rb_cObject);
    rb_cCXPrintingPolicy = rb_define_class_under(rb_mClang, "PrintingPolicy", rb_cObject);
    rb_cCXModule = rb_define_class_under(rb_mClang, "Module", rb_cObject);
    rb_cCXToken = rb_define_class_under(rb_mClang, "Token", rb_cObject);
    rb_cCXTokenSet = rb_define_class_under(rb_mClang, "TokenSet", rb_cObject);
    rb_cCXCompletionString = rb_define_class_under(rb_mClang, "CompletionString", rb_cObject);
    rb_cCXCompletionResult = rb_define_class_under(rb_mClang, "CompletionResult", rb_cObject);
    rb_cCXCodeCompleteResults = rb_define_class_under(rb_mClang, "CodeCompleteResults", rb_cObject);
    rb_cCXRemapping = rb_define_class_under(rb_mClang, "Remapping", rb_cObject);

    rb_define_alloc_func(rb_cCXVirtualFileOverlay, alloc_null);
    rb_define_alloc_func(rb_cCXModuleMapDescriptor, alloc_null);
    rb_define_alloc_func(rb_cCXIndex, alloc_null);
    rb_define_alloc_func(rb_cCXTargetInfo, alloc_null);
    rb_define_alloc_func(rb_cCXTranslationUnit, alloc_null);
    rb_define_alloc_func(rb_cCXDiagnostic, alloc_null);
    rb_define_alloc_func(rb_cCXDiagnosticSet, alloc_null);
    rb_define_alloc_func(rb_cCXPrintingPolicy, alloc_null);
    rb_define_alloc_func(rb_cCXModule, alloc_null);
    rb_define_alloc_func(rb_cCXCompletionString, alloc_null);
    rb_define_alloc_func(rb_cCXRemapping, alloc_null);
    rb_define_alloc_func(rb_cCXUnsavedFile, alloc_unsaved_file);
    rb_define_alloc_func(rb_cCXSourceLocation, alloc_source_location);
    rb_define_alloc_func(rb_cCXSourceRange, alloc_source_range);
    rb_define_alloc_func(rb_cCXCursor, alloc_cursor);
    rb_define_alloc_func(rb_cCXPlatformAvailability, alloc_platform_availability);
    rb_define_alloc_func(rb_cCXType, alloc_type);
    rb_define_alloc_func(rb_cCXToken, alloc_token);
    rb_define_alloc_func(rb_cCXCompletionResult, alloc_completion_result);

    rb_define_methodm1(rb_cCXVirtualFileOverlay, "initialize", virtual_file_initialize, -1);
    rb_define_method0(rb_cCXVirtualFileOverlay, "read", virtual_file_read, 0);
    rb_define_method2(rb_cCXVirtualFileOverlay, "map", virtual_file_map, 2);
    rb_define_method0(rb_cCXVirtualFileOverlay, "to_s", virtual_file_read, 0);
    rb_define_method2(rb_cCXModuleMapDescriptor, "initialize", modmap_initialize, 2);
    rb_define_method0(rb_cCXModuleMapDescriptor, "read", modmap_read, 0);
    rb_define_method0(rb_cCXModuleMapDescriptor, "to_s", modmap_read, 0);

    Init_clang_enums();
    Init_clang_source_location();
    Init_clang_source_range();
    Init_clang_translation_unit();
    Init_clang_diagnostic();
    Init_clang_cursor();
    Init_clang_index();
    Init_clang_type();
    Init_clang_comment();
    Init_clang_printing_policy();
    Init_clang_token();
    Init_clang_remapping();
    Init_clang_file();
    Init_clang_module();
    Init_clang_completion();
}
