#ifndef RB_CLANG_H
#define RB_CLANG_H 1

#include <ruby.h>
#include <clang-c/Index.h>

#define NUM2FLT(v) ((float) NUM2DBL(v))
#define RB_BOOL(expr) ((expr) ? Qtrue : Qfalse)
#define RUBYSTR CXString2Ruby
#define RUBYSTRSET CXStringSet2Ruby
#define CLASS_NAME(obj) rb_class2name(CLASS_OF(obj))
#define STR2SYM(str) ID2SYM(rb_intern(str))

extern VALUE rb_mClang;
extern VALUE rb_cEnum;

extern VALUE rb_AvailabilityKind;
extern VALUE rb_CursorExceptionSpecificationKind;
extern VALUE rb_GlobalOptFlags;
extern VALUE rb_DiagnosticSeverity;
extern VALUE rb_DiagnosticDisplayOptions;
extern VALUE rb_TranslationUnitFlags;
extern VALUE rb_SaveTranslationUnitFlags;
extern VALUE rb_ReparseFlags;
extern VALUE rb_TUResourceUsageKind;
extern VALUE rb_CursorKind;
extern VALUE rb_LinkageKind;
extern VALUE rb_VisibilityKind;
extern VALUE rb_LanguageKind;
extern VALUE rb_TLSKind;
extern VALUE rb_TypeKind;
extern VALUE rb_CallingConv;
extern VALUE rb_TemplateArgumentKind;
extern VALUE rb_TypeNullabilityKind;
extern VALUE rb_TypeLayoutError;
extern VALUE rb_RefQualifierKind;
extern VALUE rb_CXXAccessSpecifier;
extern VALUE rb_StorageClass;
extern VALUE rb_ChildVisitResult;
extern VALUE rb_PrintingPolicyProperty;
extern VALUE rb_ObjCPropertyAttrKind;
extern VALUE rb_ObjCDeclQualifierKind;
extern VALUE rb_NameRefFlags;
extern VALUE rb_TokenKind;
extern VALUE rb_CompletionChunkKind;
extern VALUE rb_CodeCompleteFlags;
extern VALUE rb_CompletionContext;
extern VALUE rb_VisitorResult;
extern VALUE rb_Result;

extern VALUE rb_cCXComment;
extern VALUE rb_cCXUnsavedFile;
extern VALUE rb_cCXVirtualFileOverlay;
extern VALUE rb_cCXModuleMapDescriptor;
extern VALUE rb_cCXIndex;
extern VALUE rb_cCXTargetInfo;
extern VALUE rb_cCXTranslationUnit;
extern VALUE rb_cCXFile;
extern VALUE rb_cCXSourceLocation;
extern VALUE rb_cCXSourceRange;
extern VALUE rb_cCXDiagnostic;
extern VALUE rb_cCXDiagnosticSet;
extern VALUE rb_cCXCursor;
extern VALUE rb_cCXXInfo;
extern VALUE rb_cCXPlatformAvailability;
extern VALUE rb_cCXType;
extern VALUE rb_cCXPrintingPolicy;
extern VALUE rb_cCXModule;
extern VALUE rb_cCXToken;
extern VALUE rb_cCXTokenSet;
extern VALUE rb_cCXCompletionString;
extern VALUE rb_cCXCompletionResult;
extern VALUE rb_cCXCodeCompleteResults;
extern VALUE rb_cCXRemapping;

unsigned int rb_enum_mask(VALUE enumeration, VALUE symbol_array);
unsigned int rb_enum_value(VALUE enumeration, VALUE symbol);
VALUE rb_enum_unmask(VALUE enumeration, unsigned int mask);
VALUE rb_enum_symbol(VALUE enumeration, unsigned int value);

static inline VALUE CXString2Ruby(CXString str)
{
    const char *cstr = clang_getCString(str);
    VALUE rb = rb_str_new_cstr(cstr);
    clang_disposeString(str);
    return rb;
}

static inline VALUE CXStringSet2Ruby(CXStringSet *set)
{
    if (!set)
        return rb_ary_new_capa(0);

    VALUE ary = rb_ary_new_capa(set->Count);
    for (unsigned i = 0; i < set->Count; i++)
    {
        const char *cstr = clang_getCString(set->Strings[i]);
        rb_ary_store(ary, i, rb_str_new_cstr(cstr));
    }
    clang_disposeStringSet(set);
    return ary;
}

static inline void rb_assert_type(VALUE obj, VALUE klass)
{
    if (rb_obj_is_kind_of(obj, klass) == Qtrue)
        return;
    rb_raise(rb_eTypeError, "%s is not a %s",  CLASS_NAME(obj), rb_class2name(klass));
}

#endif /* RB_CLANG_H */
