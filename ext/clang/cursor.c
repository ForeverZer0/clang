#include "clang.h"

#define CURSORKIND_GET_BOOL(name, func)                                                                                \
    static VALUE name(VALUE self)                                                                                      \
    {                                                                                                                  \
        CXCursor *c = DATA_PTR(self);                                                                                  \
        return RB_BOOL(func(c->kind));                                                                                 \
    }

CURSORKIND_GET_BOOL(cursor_is_reference, clang_isReference)
CURSORKIND_GET_BOOL(cursor_is_expression, clang_isExpression)
CURSORKIND_GET_BOOL(cursor_is_statement, clang_isStatement)
CURSORKIND_GET_BOOL(cursor_is_attribute, clang_isAttribute)
CURSORKIND_GET_BOOL(cursor_is_invalid, clang_isInvalid)
CURSORKIND_GET_BOOL(cursor_is_translation_unit, clang_isTranslationUnit)
CURSORKIND_GET_BOOL(cursor_is_preprocessing, clang_isPreprocessing)
CURSORKIND_GET_BOOL(cursor_is_unexposed, clang_isUnexposed)

static VALUE pa_platform(VALUE self)
{
    CXPlatformAvailability *pa = DATA_PTR(self);
    return rb_str_new_cstr(clang_getCString(pa->Platform));
}

static VALUE pa_introduced(VALUE self)
{
    CXVersion vs = ((CXPlatformAvailability*)DATA_PTR(self))->Introduced;
    return rb_sprintf("%d.%d.%d", vs.Major, vs.Minor, vs.Subminor);
}

static VALUE pa_deprecated(VALUE self)
{
    CXVersion vs = ((CXPlatformAvailability*)DATA_PTR(self))->Deprecated;
    return rb_sprintf("%d.%d.%d", vs.Major, vs.Minor, vs.Subminor);
}

static VALUE pa_obsoleted(VALUE self)
{
    CXVersion vs = ((CXPlatformAvailability*)DATA_PTR(self))->Obsoleted;
    return rb_sprintf("%d.%d.%d", vs.Major, vs.Minor, vs.Subminor);
}

static VALUE pa_is_unavailable(VALUE self)
{
    return RB_BOOL(((CXPlatformAvailability*)DATA_PTR(self))->Unavailable);
}

static VALUE pa_message(VALUE self)
{
    CXPlatformAvailability *pa = DATA_PTR(self);
    return rb_str_new_cstr(clang_getCString(pa->Message));
}

static VALUE cursor_display_name(VALUE self)
{
    return RUBYSTR(clang_getCursorDisplayName(*(CXCursor*)DATA_PTR(self)));
}

static VALUE cursor_pretty_printed(VALUE self, VALUE policy)
{
    CXPrintingPolicy p;
    if (RTEST(policy))
    {
        rb_assert_type(policy, rb_cCXPrintingPolicy);
        p = DATA_PTR(policy);
    }
    else
    {
        p = NULL;
    }

    return RUBYSTR(clang_getCursorPrettyPrinted(*(CXCursor*)DATA_PTR(self), p));
}

static VALUE cursor_null(VALUE klass)
{
    CXCursor *cursor = ALLOC(CXCursor);
    *cursor = clang_getNullCursor();
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, cursor);
}

static VALUE cursor_is_null(VALUE self)
{
    CXCursor *c = DATA_PTR(self);
    return RB_BOOL(clang_Cursor_isNull(*c));
}

static VALUE cursor_equal(VALUE self, VALUE other)
{
    if (CLASS_OF(self) != CLASS_OF(other))
        return Qfalse;

    CXCursor *c1 = DATA_PTR(self), *c2 = DATA_PTR(other);
    return RB_BOOL(clang_equalCursors(*c1, *c2));
}

static VALUE cursor_hash(VALUE self)
{
    CXCursor *c = DATA_PTR(self);
    return UINT2NUM(clang_hashCursor(*c));
}

static VALUE cursor_kind(VALUE self)
{
    CXCursor *c = DATA_PTR(self);
    enum CXCursorKind kind = clang_getCursorKind(*c);
    return rb_enum_symbol(rb_CursorKind, kind);
}

static VALUE cursor_has_attrs(VALUE self)
{
    CXCursor *c = DATA_PTR(self);
    return RB_BOOL(clang_Cursor_hasAttrs(*c));
}


static enum CXChildVisitResult cursor_visitor(CXCursor cursor, CXCursor parent, CXClientData data)
{
    VALUE proc = *(VALUE *)data;
    CXCursor *c = ALLOC(CXCursor), *p = ALLOC(CXCursor);
    *c = cursor;
    *p = parent;

    VALUE args = rb_ary_new_from_args(2, Data_Wrap_Struct(rb_cCXCursor, NULL, RUBY_DEFAULT_FREE, c),
                                      Data_Wrap_Struct(rb_cCXCursor, NULL, RUBY_DEFAULT_FREE, p));
    VALUE result = rb_proc_call(proc, args);
    return SYMBOL_P(result) ? rb_enum_value(rb_ChildVisitResult, result) : CXChildVisit_Break;
}

static VALUE cursor_is_declaration(int argc, VALUE *argv, VALUE self)
{
    VALUE valid;
    rb_scan_args(argc, argv, "01", &valid);

    CXCursor *c = DATA_PTR(self);
    unsigned result = clang_isDeclaration(c->kind);

    if (RTEST(valid))
        result = result && !clang_isInvalidDeclaration(*c);
    
    return RB_BOOL(result);
}

static VALUE cursor_visit_children(VALUE self)
{
    rb_need_block();
    VALUE proc = rb_block_proc();
    CXCursor *c = DATA_PTR(self);
    clang_visitChildren(*c, cursor_visitor, &proc);
    return self;
}

static VALUE cursor_spelling(VALUE self)
{
    CXCursor *cursor = DATA_PTR(self);
    CXString str = clang_getCursorSpelling(*cursor);
    return RUBYSTR(str);
}

static VALUE cursor_linkage(VALUE self)
{
    CXCursor *c = DATA_PTR(self);
    int value = clang_getCursorLinkage(*c);
    return rb_enum_symbol(rb_LinkageKind, value);
}

static VALUE cursor_visibility(VALUE self)
{
    CXCursor *c = DATA_PTR(self);
    int value = clang_getCursorVisibility(*c);
    return rb_enum_symbol(rb_VisibilityKind, value);
}

static VALUE cursor_availability(VALUE self)
{
    CXCursor *c = DATA_PTR(self);
    int value = clang_getCursorAvailability(*c);
    return rb_enum_symbol(rb_AvailabilityKind, value);
}

static VALUE cursor_language(VALUE self)
{
    CXCursor *c = DATA_PTR(self);
    int value = clang_getCursorLanguage(*c);
    return rb_enum_symbol(rb_LanguageKind, value);
}

static VALUE cursor_translation_unit(VALUE self)
{
    CXCursor *c = DATA_PTR(self);
    CXTranslationUnit unit = clang_Cursor_getTranslationUnit(*c);
    return unit ? Data_Wrap_Struct(rb_cCXTranslationUnit, NULL, RUBY_NEVER_FREE, unit) : Qnil;
}

static VALUE cursor_semantic_parent(VALUE self)
{
    CXCursor *c = DATA_PTR(self), *parent = ALLOC(CXCursor);
    *parent = clang_getCursorSemanticParent(*c);
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, parent);
}

static VALUE cursor_lexical_parent(VALUE self)
{
    CXCursor *c = DATA_PTR(self), *parent = ALLOC(CXCursor);
    *parent = clang_getCursorLexicalParent(*c);
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, parent);
}

static VALUE cursor_location(VALUE self)
{
    CXCursor *cursor = DATA_PTR(self);
    CXSourceLocation *loc = ALLOC(CXSourceLocation);
    *loc = clang_getCursorLocation(*cursor);
    return Data_Wrap_Struct(rb_cCXSourceLocation, NULL, RUBY_DEFAULT_FREE, loc);
}

static VALUE cursor_extent(VALUE self)
{
    CXCursor *cursor = DATA_PTR(self);
    CXSourceRange *range = ALLOC(CXSourceRange);
    *range = clang_getCursorExtent(*cursor);
    return Data_Wrap_Struct(rb_cCXSourceRange, NULL, RUBY_DEFAULT_FREE, range);
}

static VALUE cursor_tls_kind(VALUE self)
{
    CXCursor *cursor = DATA_PTR(self);
    int kind = clang_getCursorTLSKind(*cursor);
    return rb_enum_symbol(rb_TLSKind, kind);
}

static VALUE cursor_initialize(VALUE self, VALUE translation_unit, VALUE location)
{
    rb_assert_type(translation_unit, rb_cCXTranslationUnit);
    rb_assert_type(location, rb_cCXSourceLocation);

    CXTranslationUnit unit = DATA_PTR(translation_unit);
    CXSourceLocation *loc = DATA_PTR(location);

    CXCursor cursor = clang_getCursor(unit, *loc);
    memcpy(DATA_PTR(self), &cursor, sizeof(CXCursor));
    return self;
}

static VALUE cursor_included_file(VALUE self)
{
    CXCursor *c = DATA_PTR(self);
    CXFile file = clang_getIncludedFile(*c);
    return file ? Data_Wrap_Struct(rb_cCXFile, NULL, RUBY_NEVER_FREE, file) : Qnil;
}

static VALUE cursor_overrides(VALUE self)
{
    CXCursor *c = DATA_PTR(self);
    CXCursor *cursors;
    unsigned int count;

    clang_getOverriddenCursors(*c, &cursors, &count);
    VALUE ary = rb_ary_new_capa(count);
    if (count && cursors)
    {
        for (unsigned i = 0; i < count; i++)
        {
            CXCursor *cursor = ALLOC(CXCursor);
            *cursor = cursors[i];
            rb_ary_store(ary, i, Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, cursor));
        }
        clang_disposeOverriddenCursors(cursors);
    }
    return ary;
}

static VALUE cursor_type(VALUE self)
{
    CXCursor *cursor = DATA_PTR(self);
    CXType *type = ALLOC(CXType);

    *type = clang_getCursorType(*cursor);
    if (type->kind == CXType_Invalid)
    {
        xfree(type);
        return Qnil;
    }
    return Data_Wrap_Struct(rb_cCXType, NULL, RUBY_DEFAULT_FREE, type);
}

static VALUE cursor_underlying_type(VALUE self)
{
    CXCursor *cursor = DATA_PTR(self); 
    CXType *type = ALLOC(CXType);
    *type = clang_getTypedefDeclUnderlyingType(*cursor);
    if (type->kind == CXType_Invalid)
    {
        xfree(type);
        return Qnil;
    }
    return Data_Wrap_Struct(rb_cCXType, NULL, RUBY_DEFAULT_FREE, type);
}

static VALUE cursor_enum_int_type(VALUE self)
{
    CXCursor *cursor = DATA_PTR(self); 
    CXType *type = ALLOC(CXType);
    *type = clang_getEnumDeclIntegerType(*cursor);
    if (type->kind == CXType_Invalid)
    {
        xfree(type);
        return Qnil;
    }
    return Data_Wrap_Struct(rb_cCXType, NULL, RUBY_DEFAULT_FREE, type);
}

static VALUE cursor_result_type(VALUE self)
{
    CXType *type = ALLOC(CXType);
    CXCursor *c = DATA_PTR(self);
    *type = clang_getCursorResultType(*c);
    if (type->kind == CXType_Invalid)
    {
        xfree(type);
        return Qnil;
    }
    return Data_Wrap_Struct(rb_cCXType, NULL, RUBY_DEFAULT_FREE, type);
}

static VALUE cursor_receiver_type(VALUE self)
{
    CXType *type = ALLOC(CXType);
    *type = clang_Cursor_getReceiverType(*(CXCursor*)DATA_PTR(self));
    if (type->kind == CXType_Invalid)
    {
        xfree(type);
        return Qnil;
    }
    return Data_Wrap_Struct(rb_cCXType, NULL, RUBY_DEFAULT_FREE, type);
}

static VALUE cursor_ib_outlet_type(VALUE self)
{
    CXType *type = ALLOC(CXType);
    *type = clang_getIBOutletCollectionType(*(CXCursor*)DATA_PTR(self));
    if (type->kind == CXType_Invalid)
    {
        xfree(type);
        return Qnil;
    }
    return Data_Wrap_Struct(rb_cCXType, NULL, RUBY_DEFAULT_FREE, type);
}

static VALUE cursor_enum_value(int argc, VALUE *argv, VALUE self)
{
    VALUE unsigned_value;
    rb_scan_args(argc, argv, "01", &unsigned_value);
    CXCursor *cursor = DATA_PTR(self);

    if (RTEST(unsigned_value))
        return ULL2NUM(clang_getEnumConstantDeclUnsignedValue(*cursor));

    return LL2NUM(clang_getEnumConstantDeclValue(*cursor));
}

static VALUE cursor_bit_width(VALUE self)
{
    CXCursor *c = DATA_PTR(self);
    int w = clang_getFieldDeclBitWidth(*c);
    return INT2NUM(w);
}

static VALUE cursor_arg_count(VALUE self)
{
    CXCursor *c = DATA_PTR(self);
    return INT2NUM(clang_Cursor_getNumArguments(*c));
}

static VALUE cursor_args(VALUE self)
{
    CXCursor *c = DATA_PTR(self);
    int n = clang_Cursor_getNumArguments(*c);
    if (n <= 0)
        return rb_ary_new_capa(0);

    VALUE ary = rb_ary_new_capa(n);
    for (int i = 0; i < n; i++)
    {
        CXCursor *arg = ALLOC(CXCursor);
        *arg = clang_Cursor_getArgument(*c, i);
        rb_ary_store(ary, i, Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, arg));
    }

    return ary;
}

static VALUE cursor_each_arg(VALUE self)
{
    RETURN_ENUMERATOR(self, 0, NULL);
    CXCursor *c = DATA_PTR(self);
    int n = clang_Cursor_getNumArguments(*c);

    for (int i = 0; i < n; i++)
    {
        CXCursor *arg = ALLOC(CXCursor);
        *arg = clang_Cursor_getArgument(*c, i);
        rb_yield(Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, arg));
    }

    return self;
}

static VALUE cursor_each_overload(VALUE self)
{
    RETURN_ENUMERATOR(self, 0, NULL);
    CXCursor c = *(CXCursor*) DATA_PTR(self);

    unsigned int n = clang_getNumOverloadedDecls(c);
    for (unsigned i = 0; i < n; i++)
    {
        CXCursor *cursor = ALLOC(CXCursor);
        *cursor = clang_getOverloadedDecl(c, i);
        rb_yield(Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, cursor));
    }

    return self;
}

static VALUE cursor_is_macro_func(VALUE self)
{
    CXCursor *c = DATA_PTR(self);
    return RB_BOOL(clang_Cursor_isMacroFunctionLike(*c));
}

static VALUE cursor_is_macro_builtin(VALUE self)
{
    CXCursor *c = DATA_PTR(self);
    return RB_BOOL(clang_Cursor_isMacroBuiltin(*c));
}

static VALUE cursor_field_offset(VALUE self)
{
    CXCursor *c = DATA_PTR(self);
    return LL2NUM(clang_Cursor_getOffsetOfField(*c));
}

static VALUE cursor_is_anonymous(VALUE self)
{
    CXCursor c = *(CXCursor*) DATA_PTR(self);
    return RB_BOOL(clang_Cursor_isAnonymous(c) || clang_Cursor_isAnonymousRecordDecl(c));
}

static VALUE cursor_is_inline(VALUE self)
{
    CXCursor c = *(CXCursor*) DATA_PTR(self);
    return RB_BOOL(clang_Cursor_isFunctionInlined(c) || clang_Cursor_isInlineNamespace(c));
}

static VALUE cursor_definition(VALUE self)
{
    CXCursor *c = DATA_PTR(self), *def = ALLOC(CXCursor);
    *def = clang_getCursorDefinition(*c);
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, def);
}

static VALUE cursor_is_definition(VALUE self)
{
    CXCursor *c = DATA_PTR(self);
    return RB_BOOL(clang_isCursorDefinition(*c));
}

static VALUE cursor_raw_comment(VALUE self)
{
    CXString str = clang_Cursor_getRawCommentText(*(CXCursor*)DATA_PTR(self));
    return RUBYSTR(str);
}

static VALUE cursor_brief_comment(VALUE self)
{
    CXString str = clang_Cursor_getBriefCommentText(*(CXCursor*)DATA_PTR(self));
    return RUBYSTR(str);
}

static VALUE cursor_comment_range(VALUE self)
{
    CXSourceRange *range = ALLOC(CXSourceRange);
    *range = clang_Cursor_getCommentRange(*(CXCursor*)DATA_PTR(self));
    return Data_Wrap_Struct(rb_cCXSourceRange, NULL, RUBY_DEFAULT_FREE, range);
}

static VALUE cursor_is_bitfield(VALUE self)
{
    return RB_BOOL(clang_Cursor_isBitField(*(CXCursor*)DATA_PTR(self)));
}

static VALUE cursor_is_virtual_base(VALUE self)
{
    return RB_BOOL(clang_isVirtualBase(*(CXCursor*)DATA_PTR(self)));
}

static VALUE cursor_canonical(VALUE self)
{
    CXCursor *cursor = DATA_PTR(self), *canonical = ALLOC(CXCursor);
    *canonical = clang_getCanonicalCursor(*cursor);
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, canonical);
}

static VALUE cursor_is_dynamic_call(VALUE self)
{
    return RB_BOOL(clang_Cursor_isDynamicCall(*(CXCursor*)DATA_PTR(self)));
}

static VALUE cursor_is_variadic(VALUE self)
{
    return RB_BOOL(clang_Cursor_isVariadic(*(CXCursor*)DATA_PTR(self)));
}

static VALUE cursor_mangling(VALUE self)
{
    return RUBYSTR(clang_Cursor_getMangling(*(CXCursor*)DATA_PTR(self)));
}

static VALUE cursor_cxx_manglings(VALUE self)
{
    return RUBYSTRSET(clang_Cursor_getCXXManglings(*(CXCursor*)DATA_PTR(self)));
}

static VALUE cursor_obj_c_manglings(VALUE self)
{
    return RUBYSTRSET(clang_Cursor_getObjCManglings(*(CXCursor*)DATA_PTR(self)));
}

static VALUE cursor_module(VALUE self)
{
    CXModule mod = clang_Cursor_getModule(*(CXCursor*)DATA_PTR(self));

    if (!mod)
        return Qnil;

    VALUE value = Data_Wrap_Struct(rb_cCXModule, NULL, RUBY_NEVER_FREE, mod);
    rb_iv_set(value, "@translation_unit", cursor_translation_unit(self));
    return value;
}

static VALUE cursor_platform_availability(VALUE self)
{
    CXCursor cursor = *(CXCursor*)DATA_PTR(self);

    int always_deprecated, always_unavailable;
    CXString deprecated_message, unavailable_message;
    int count = clang_getCursorPlatformAvailability(cursor, &always_deprecated, &deprecated_message, &always_unavailable, &unavailable_message, NULL, 0);

    VALUE ary = rb_ary_new_capa(count);
    CXPlatformAvailability availabilty[count];
    clang_getCursorPlatformAvailability(cursor, NULL, NULL, NULL, NULL, availabilty, count);

    VALUE hash = rb_hash_new();
    rb_hash_aset(hash, STR2SYM("deprecated"), RB_BOOL(always_deprecated));
    rb_hash_aset(hash, STR2SYM("deprecated_msg"), RUBYSTR(deprecated_message));
    rb_hash_aset(hash, STR2SYM("unavailable"), RB_BOOL(always_unavailable));
    rb_hash_aset(hash, STR2SYM("unavailable_msg"), RUBYSTR(unavailable_message));

    for (int i = 0; i < count; i++)
    {
        CXPlatformAvailability *avail = ALLOC(CXPlatformAvailability);
        *avail = availabilty[i];
        VALUE a = Data_Wrap_Struct(rb_cCXPlatformAvailability, NULL, (RUBY_DATA_FUNC) clang_disposeCXPlatformAvailability, avail);
        rb_ary_store(ary, i, a);
    }

    rb_hash_aset(hash, STR2SYM("availability"), ary);
    return hash;
}

static VALUE cursor_each_template_arg(VALUE self)
{
    RETURN_ENUMERATOR(self, 0, NULL);

    CXCursor cursor = *(CXCursor*)DATA_PTR(self);
    int count = clang_Cursor_getNumTemplateArguments(cursor);

    if (count < 1)
        return self;

    for (unsigned i = 0; i < count; i++)
    {
        CXType *type = ALLOC(CXType);
        *type = clang_Cursor_getTemplateArgumentType(cursor, i);
        enum CXTemplateArgumentKind kind = clang_Cursor_getTemplateArgumentKind(cursor, i);
        VALUE value = Qnil;

        if (kind == CXTemplateArgumentKind_Integral)
        {
            switch (type->kind)
            {
                CXType_UInt:
                CXType_ULong:
                CXType_ULongLong:
                CXType_Char_U:
                CXType_UChar:
                CXType_UShort:
                {
                    value = ULL2NUM(clang_Cursor_getTemplateArgumentUnsignedValue(cursor, i));
                    break;  
                }
                default:
                {
                    value = LL2NUM(clang_Cursor_getTemplateArgumentValue(cursor, i));
                    break;
                }
            }
        }

        VALUE args = rb_ary_new_from_args(4,
            INT2NUM(i),
            Data_Wrap_Struct(rb_cCXType, NULL, RUBY_DEFAULT_FREE, type),
            rb_enum_symbol(rb_TemplateArgumentKind, kind),
            value
        );

        rb_yield(args);
    }

    return self;
}

static VALUE cursor_exception_type(VALUE self)
{
    CXCursor *cursor = DATA_PTR(self);
    enum CXCursor_ExceptionSpecificationKind kind = clang_getCursorExceptionSpecificationType(*cursor);
    if (kind < 0)
        return STR2SYM("invalid");
    return rb_enum_symbol(rb_CursorExceptionSpecificationKind, kind);
}

static VALUE cursor_referenced(VALUE self)
{
    CXCursor *cursor = DATA_PTR(self), *referenced = ALLOC(CXCursor);
    *referenced = clang_getCursorReferenced(*cursor);
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, referenced);
}

static VALUE cursor_completion_string(VALUE self)
{
    CXCompletionString str = clang_getCursorCompletionString(*(CXCursor*)DATA_PTR(self));
    return str ? Data_Wrap_Struct(rb_cCXCompletionString, NULL, RUBY_NEVER_FREE, str) : Qnil;
}

static VALUE cursor_eval(VALUE self)
{
    CXEvalResult result = clang_Cursor_Evaluate(*(CXCursor*)DATA_PTR(self));
    CXEvalResultKind kind = clang_EvalResult_getKind(result);
    
    VALUE value;

    switch (kind)
    {
        case CXEval_Int:
        {
            if (clang_EvalResult_isUnsignedInt(result))
                value = ULL2NUM(clang_EvalResult_getAsUnsigned(result));
            else
                value = LL2NUM(clang_EvalResult_getAsLongLong(result));
            break;
        }
        case CXEval_Float: 
        {
            value = DBL2NUM(clang_EvalResult_getAsDouble(result));
            break;
        }
        case CXEval_ObjCStrLiteral:
        case CXEval_StrLiteral:
        case CXEval_CFStr:
        {
            value = rb_str_new_cstr(clang_EvalResult_getAsStr(result));
            break;
        }
        case CXEval_Other:
        case CXEval_UnExposed:
        default:
        {
            value = Qnil;
            break;
        }
    }

    clang_EvalResult_dispose(result);
    return value;
}

static enum CXVisitorResult cursor_reference_visitor(void *context, CXCursor cursor, CXSourceRange range)
{
    VALUE proc = *(VALUE*)context;
    CXCursor *c = ALLOC(CXCursor);
    CXSourceRange *r = ALLOC(CXSourceRange);

    *c = cursor;
    *r = range;
    VALUE args = rb_ary_new_from_args(2, 
        Data_Wrap_Struct(rb_cCXCursor, NULL, RUBY_DEFAULT_FREE, c),
        Data_Wrap_Struct(rb_cCXSourceRange, NULL, RUBY_DEFAULT_FREE, r)
    );

    VALUE result = rb_proc_call(proc, args);
    return (result == STR2SYM("continue")) ? CXVisit_Continue : CXVisit_Break;
}

static VALUE cursor_find_references(int argc, VALUE *argv, VALUE self)
{
    rb_need_block();
    VALUE source_file;
    rb_scan_args(argc, argv, "01", &source_file);

    CXCursor cursor = *(CXCursor*)DATA_PTR(self);
    CXFile file;
    if (NIL_P(source_file))
    {
        CXSourceLocation location = clang_getCursorLocation(cursor);
        clang_getFileLocation(location, &file, NULL, NULL, NULL);
    }
    else
    {
        rb_assert_type(source_file, rb_cCXFile);
        file = DATA_PTR(file);
    }

    VALUE proc = rb_block_proc();
    CXCursorAndRangeVisitor visitor = { &proc, cursor_reference_visitor };

    clang_findReferencesInFile(cursor, file, visitor);
    return Qnil;
}

static VALUE cursor_storage_class(VALUE self)
{
    enum CX_StorageClass sc = clang_Cursor_getStorageClass(*(CXCursor*)DATA_PTR(self));
    return rb_enum_symbol(rb_StorageClass, sc);
}

static VALUE cursor_is_external(VALUE self)
{
    return RB_BOOL(clang_Cursor_isExternalSymbol(*(CXCursor*)DATA_PTR(self), NULL, NULL, NULL));
}

static VALUE cursor_external_info(VALUE self)
{
    CXString language, defined_in;
    unsigned int generated;

    unsigned external = clang_Cursor_isExternalSymbol(*(CXCursor*)DATA_PTR(self), &language, &defined_in, &generated);
    if (!external)
        return Qnil;
    
    VALUE hash = rb_hash_new();
    rb_hash_aset(hash, STR2SYM("language"), RUBYSTR(language));
    rb_hash_aset(hash, STR2SYM("defined"), RUBYSTR(defined_in));
    rb_hash_aset(hash, STR2SYM("generated"), RB_BOOL(generated));
    return hash;
}


static VALUE cursor_obj_c_getter(VALUE self)
{
    return RUBYSTR(clang_Cursor_getObjCPropertyGetterName(*(CXCursor*)DATA_PTR(self)));
}

static VALUE cursor_obj_c_setter(VALUE self)
{
    return RUBYSTR(clang_Cursor_getObjCPropertySetterName(*(CXCursor*)DATA_PTR(self)));
}

static VALUE cursor_obj_c_qualifiers(VALUE self)
{
    CXObjCDeclQualifierKind kind = clang_Cursor_getObjCDeclQualifiers(*(CXCursor*)DATA_PTR(self));
    return rb_enum_unmask(rb_ObjCDeclQualifierKind, kind);
}

static VALUE cursor_obj_c_selector_index(VALUE self)
{
    return INT2NUM(clang_Cursor_getObjCSelectorIndex(*(CXCursor*)DATA_PTR(self)));
}

static VALUE cursor_obj_c_attributes(VALUE self)
{
    CXObjCPropertyAttrKind kind = clang_Cursor_getObjCPropertyAttributes(*(CXCursor*)DATA_PTR(self), 0);
    return rb_enum_unmask(rb_ObjCPropertyAttrKind, kind);
}

static VALUE cursor_obj_c_is_optional(VALUE self)
{
    return RB_BOOL(clang_Cursor_isObjCOptional(*(CXCursor*)DATA_PTR(self)));
}

static VALUE cursor_default_policy(VALUE klass)
{
    CXCursor null = clang_getNullCursor();
    CXPrintingPolicy policy = clang_getCursorPrintingPolicy(null);
    if (!policy)
    {
        printf("POLICY NULL\n");
        return Qnil;
    }

    // TODO: Remove policy, user sets via kwargs, get from cursor, assert non-null, update policy, apply.
    // TODO: setup getting and returning as hash, remove class completely

    // TODO
    return Data_Wrap_Struct(rb_cCXPrintingPolicy, NULL, clang_PrintingPolicy_dispose, policy);
}

void Init_clang_cursor(void)
{
    rb_define_singleton_method0(rb_cCXCursor, "default_policy", cursor_default_policy, 0); // TODO


    rb_define_singleton_method0(rb_cCXCursor, "null", cursor_null, 0);
    rb_define_method2(rb_cCXCursor, "initialize", cursor_initialize, 2);
    rb_define_method0(rb_cCXCursor, "null?", cursor_is_null, 0);
    rb_define_method0(rb_cCXCursor, "module", cursor_module, 0);
    rb_define_method0(rb_cCXCursor, "ib_outlet_type", cursor_ib_outlet_type, 0);
    rb_define_method0(rb_cCXCursor, "storage_class", cursor_storage_class, 0);
    rb_define_method0(rb_cCXCursor, "each_overload", cursor_each_overload, 0);
    rb_define_methodm1(rb_cCXCursor, "find_references", cursor_find_references, -1);
    rb_define_method0(rb_cCXCursor, "raw_comment", cursor_raw_comment, 0);
    rb_define_method0(rb_cCXCursor, "brief_comment", cursor_brief_comment, 0);
    rb_define_method0(rb_cCXCursor, "comment_range", cursor_comment_range, 0);
    rb_define_method0(rb_cCXCursor, "canonical", cursor_canonical, 0);
    rb_define_method0(rb_cCXCursor, "receiver_type", cursor_receiver_type, 0);
    rb_define_method0(rb_cCXCursor, "mangling", cursor_mangling, 0);
    rb_define_method0(rb_cCXCursor, "cxx_manglings", cursor_cxx_manglings, 0);
    rb_define_method1(rb_cCXCursor, "pretty_printed", cursor_pretty_printed, 1);
    rb_define_method0(rb_cCXCursor, "translation_unit", cursor_translation_unit, 0);
    rb_define_method0(rb_cCXCursor, "semantic_parent", cursor_semantic_parent, 0);
    rb_define_method0(rb_cCXCursor, "lexical_parent", cursor_lexical_parent, 0);
    rb_define_method0(rb_cCXCursor, "location", cursor_location, 0);
    rb_define_method0(rb_cCXCursor, "extent", cursor_extent, 0);
    rb_define_method0(rb_cCXCursor, "kind", cursor_kind, 0);
    rb_define_method0(rb_cCXCursor, "visit_children", cursor_visit_children, 0);
    rb_define_method0(rb_cCXCursor, "spelling", cursor_spelling, 0);
    rb_define_methodm1(rb_cCXCursor, "declaration?", cursor_is_declaration, -1);
    rb_define_method0(rb_cCXCursor, "reference?", cursor_is_reference, 0);
    rb_define_method0(rb_cCXCursor, "dynamic_call?", cursor_is_dynamic_call, 0);
    rb_define_method0(rb_cCXCursor, "expression?", cursor_is_expression, 0);
    rb_define_method0(rb_cCXCursor, "statement?", cursor_is_statement, 0);
    rb_define_method0(rb_cCXCursor, "attribute?", cursor_is_attribute, 0);
    rb_define_method0(rb_cCXCursor, "invalid?", cursor_is_invalid, 0);
    rb_define_method0(rb_cCXCursor, "external?", cursor_is_external, 0);
    rb_define_method0(rb_cCXCursor, "external_info", cursor_external_info, 0);
    rb_define_method0(rb_cCXCursor, "variadic?", cursor_is_variadic, 0);
    rb_define_method0(rb_cCXCursor, "anonymous?", cursor_is_anonymous, 0);
    rb_define_method0(rb_cCXCursor, "inline?", cursor_is_inline, 0);
    rb_define_method0(rb_cCXCursor, "bitfield?", cursor_is_bitfield, 0);
    rb_define_method0(rb_cCXCursor, "virtual_base?", cursor_is_virtual_base, 0);
    rb_define_method0(rb_cCXCursor, "translation_unit?", cursor_is_translation_unit, 0);
    rb_define_method0(rb_cCXCursor, "preprocessing?", cursor_is_preprocessing, 0);
    rb_define_method0(rb_cCXCursor, "unexposed?", cursor_is_unexposed, 0);
    rb_define_method0(rb_cCXCursor, "has_attrs?", cursor_has_attrs, 0);
    rb_define_method0(rb_cCXCursor, "macro_func?", cursor_is_macro_func, 0);
    rb_define_method0(rb_cCXCursor, "macro_builtin?", cursor_is_macro_builtin, 0);
    rb_define_method0(rb_cCXCursor, "linkage", cursor_linkage, 0);
    rb_define_method0(rb_cCXCursor, "visibility", cursor_visibility, 0);
    rb_define_method0(rb_cCXCursor, "availability", cursor_availability, 0);
    rb_define_method0(rb_cCXCursor, "language", cursor_language, 0);
    rb_define_method0(rb_cCXCursor, "tls_kind", cursor_tls_kind, 0);
    rb_define_method0(rb_cCXCursor, "included_file", cursor_included_file, 0);
    rb_define_method0(rb_cCXCursor, "overrides", cursor_overrides, 0);
    rb_define_method0(rb_cCXCursor, "type", cursor_type, 0);
    rb_define_method0(rb_cCXCursor, "result_type", cursor_result_type, 0);
    rb_define_method0(rb_cCXCursor, "underlying_type", cursor_underlying_type, 0);
    rb_define_method0(rb_cCXCursor, "enum_int_type", cursor_enum_int_type, 0);
    rb_define_methodm1(rb_cCXCursor, "enum_value", cursor_enum_value, -1);
    rb_define_method0(rb_cCXCursor, "bit_width", cursor_bit_width, 0);
    rb_define_method0(rb_cCXCursor, "field_offset", cursor_field_offset, 0);
    rb_define_method0(rb_cCXCursor, "definition", cursor_definition, 0);
    rb_define_method0(rb_cCXCursor, "definition?", cursor_is_definition, 0);
    rb_define_method0(rb_cCXCursor, "arg_count", cursor_arg_count, 0);
    rb_define_method0(rb_cCXCursor, "args", cursor_args, 0);
    rb_define_method0(rb_cCXCursor, "each_arg", cursor_each_arg, 0);
    rb_define_method0(rb_cCXCursor, "each_template_arg", cursor_each_template_arg, 0);
    rb_define_method0(rb_cCXCursor, "hash", cursor_hash, 0);
    rb_define_method1(rb_cCXCursor, "==", cursor_equal, 1);
    rb_define_method1(rb_cCXCursor, "eql?", cursor_equal, 1);
    rb_define_method0(rb_cCXCursor, "to_s", cursor_display_name, 0);
    rb_define_method0(rb_cCXCursor, "to_str", cursor_display_name, 0);
    rb_define_method0(rb_cCXCursor, "platform_availability", cursor_platform_availability, 0);
    rb_define_method0(rb_cCXCursor, "exception_type", cursor_exception_type, 0);
    rb_define_method0(rb_cCXCursor, "referenced", cursor_referenced, 0);
    rb_define_method0(rb_cCXCursor, "completion_string", cursor_completion_string, 0);
    rb_define_method0(rb_cCXCursor, "eval", cursor_eval, 0);

    rb_define_method0(rb_cCXCursor, "obj_c_manglings", cursor_obj_c_manglings, 0);
    rb_define_method0(rb_cCXCursor, "obj_c_getter", cursor_obj_c_getter, 0);
    rb_define_method0(rb_cCXCursor, "obj_c_setter", cursor_obj_c_setter, 0);
    rb_define_method0(rb_cCXCursor, "obj_c_qualifiers", cursor_obj_c_qualifiers, 0);
    rb_define_method0(rb_cCXCursor, "obj_c_selector_index", cursor_obj_c_selector_index, 0);
    rb_define_method0(rb_cCXCursor, "obj_c_attributes", cursor_obj_c_attributes, 0);
    rb_define_method0(rb_cCXCursor, "obj_c_optional?", cursor_obj_c_is_optional, 0);

    rb_define_method0(rb_cCXPlatformAvailability, "platform", pa_platform, 0);
    rb_define_method0(rb_cCXPlatformAvailability, "introduced", pa_introduced, 0);
    rb_define_method0(rb_cCXPlatformAvailability, "deprecated", pa_deprecated, 0);
    rb_define_method0(rb_cCXPlatformAvailability, "obsoleted", pa_obsoleted, 0);
    rb_define_method0(rb_cCXPlatformAvailability, "unavailable?", pa_is_unavailable, 0);
    rb_define_method0(rb_cCXPlatformAvailability, "message", pa_message, 0);
}