#include "clang.h"
#include "uthash.h"

VALUE rb_cEnum;
VALUE rb_AvailabilityKind;
VALUE rb_CursorExceptionSpecificationKind;
VALUE rb_GlobalOptFlags;
VALUE rb_DiagnosticSeverity;
VALUE rb_DiagnosticDisplayOptions;
VALUE rb_TranslationUnitFlags;
VALUE rb_SaveTranslationUnitFlags;
VALUE rb_ReparseFlags;
VALUE rb_TUResourceUsageKind;
VALUE rb_CursorKind;
VALUE rb_LinkageKind;
VALUE rb_VisibilityKind;
VALUE rb_LanguageKind;
VALUE rb_TLSKind;
VALUE rb_TypeKind;
VALUE rb_CallingConv;
VALUE rb_TemplateArgumentKind;
VALUE rb_TypeNullabilityKind;
VALUE rb_TypeLayoutError;
VALUE rb_RefQualifierKind;
VALUE rb_CXXAccessSpecifier;
VALUE rb_StorageClass;
VALUE rb_ChildVisitResult;
VALUE rb_PrintingPolicyProperty;
VALUE rb_ObjCPropertyAttrKind;
VALUE rb_ObjCDeclQualifierKind;
VALUE rb_NameRefFlags;
VALUE rb_TokenKind;
VALUE rb_CompletionChunkKind;
VALUE rb_CodeCompleteFlags;
VALUE rb_CompletionContext;
VALUE rb_VisitorResult;
VALUE rb_Result;

typedef struct 
{
    VALUE sym;
    unsigned int value;
    UT_hash_handle hh;
} rb_enum;

static void enum_free(void *data)
{
    rb_enum *e, *temp, *head = data;
    HASH_ITER(hh, head, e, temp)
    {
        HASH_DEL(head, e);
        xfree(e);
    }
}

VALUE enum_allocate(VALUE klass)
{
    
    return Data_Wrap_Struct(klass, NULL, enum_free, NULL);
}

static VALUE enum_create(VALUE *value, const char *name)
{
    *value = enum_allocate(rb_cEnum);
    rb_define_const(rb_mClang, name, *value);
}

void enum_field(VALUE enumeration, const char *name, unsigned int value)
{
    rb_enum *field = ALLOC(rb_enum);
    field->sym = STR2SYM(name);
    field->value = value;

    rb_enum **e = (rb_enum**) &RDATA(enumeration)->data;
    HASH_ADD(hh, *e, sym, sizeof(VALUE), field);
}

static VALUE enum_each(VALUE self)
{
    RETURN_ENUMERATOR(self, 0, NULL);

    rb_enum *e, *temp, *head = DATA_PTR(self);
    HASH_ITER(hh, head, e, temp)
    {
        rb_yield(rb_ary_new_from_args(2, e->sym, INT2NUM(e->value)));
    }

    return self;
}

static VALUE enum_count(VALUE self)
{
    rb_enum *head = DATA_PTR(self);
    return UINT2NUM(HASH_COUNT(head));
}

static VALUE enum_names(VALUE self)
{
    rb_enum *e, *temp, *head = DATA_PTR(self);
    unsigned int count = HASH_COUNT(head);
    VALUE ary = rb_ary_new_capa(count);

    HASH_ITER(hh, head, e, temp)
    {
        rb_ary_push(ary, e->sym);
    }

    return ary;
}

static VALUE enum_fields(VALUE self)
{
    rb_enum *e, *temp, *head = DATA_PTR(self);
    unsigned int count = HASH_COUNT(head);
    VALUE ary = rb_ary_new_capa(count);

    HASH_ITER(hh, head, e, temp)
    {
        rb_ary_push(ary, INT2NUM(e->value));
    }

    return ary;
}

static VALUE enum_to_hash(VALUE self)
{
    VALUE hash = rb_hash_new();
    rb_enum *e, *temp, *head = DATA_PTR(self);
    
    HASH_ITER(hh, head, e, temp)
    {
        rb_hash_aset(hash, e->sym, UINT2NUM(e->value));
    }

    return hash;
}

static VALUE enum_to_s(VALUE self)
{
    VALUE hash = enum_to_hash(self);
    return rb_funcall(hash, rb_intern("to_s"), 0);
}

static VALUE enum_inspect(VALUE self)
{
    VALUE hash = enum_to_hash(self);
    return rb_inspect(hash);
}

unsigned int rb_enum_mask(VALUE enumeration, VALUE symbols)
{
    long len = rb_array_len(symbols);
    unsigned int mask = 0;
    rb_enum *e, *head = DATA_PTR(enumeration);
    VALUE sym;

    for (long i = 0; i < len; i++)
    {
        sym = rb_ary_entry(symbols, i);
        if (!SYMBOL_P(sym))
            continue;

        HASH_FIND(hh, head, &sym, sizeof(VALUE), e);
        if (e)
            mask |= e->value;
    }

    return mask;
}

VALUE rb_enum_symbol(VALUE enumeration, unsigned int value)
{
    rb_enum *e, *temp, *head = DATA_PTR(enumeration);
    HASH_ITER(hh, head, e, temp)
    {
        if (e->value == value)
            return e->sym;
    }
    return UINT2NUM(value);
}

unsigned int rb_enum_value(VALUE enumeration, VALUE symbol)
{
    if (!SYMBOL_P(symbol))
        return 0;
    rb_enum *e, *head = DATA_PTR(enumeration);
    HASH_FIND(hh, head, &symbol, sizeof(VALUE), e);
    return e ? e->value : 0;
}

VALUE rb_enum_unmask(VALUE enumeration, unsigned int mask)
{
    VALUE ary = rb_ary_new();
    rb_enum *e, *temp, *head = DATA_PTR(enumeration);
    HASH_ITER(hh, head, e, temp)
    {
        if ((e->value & mask) != 0)
            rb_ary_push(ary, e->sym);
    }

    return ary;
}

static VALUE enum_mask(VALUE self, VALUE symbols)
{
    unsigned int mask = rb_enum_mask(self, symbols);
    return UINT2NUM(mask);
}

static VALUE enum_unmask(VALUE self, VALUE value)
{
    if (NIL_P(value))
        return rb_ary_new_capa(0);

    unsigned int mask = NUM2UINT(value);
    return rb_enum_mask(self, mask);
}

static VALUE enum_symbol(VALUE self, VALUE value)
{
    return rb_enum_symbol(self, NUM2UINT(value));
}

static VALUE enum_value(VALUE self, VALUE symbol)
{
    return UINT2NUM(rb_enum_value(self, symbol));
}

static VALUE enum_method_missing(int argc, VALUE *argv, VALUE self)
{
    if (argc == 1 && SYMBOL_P(argv[0]))
    {
        rb_enum *result, *head = DATA_PTR(self);    
        HASH_FIND(hh, head, &argv[0], sizeof(VALUE), result);
        if (result)
            return UINT2NUM(result->value);
    }
    return rb_call_super(argc, argv);
}

void Init_clang_enums(void)
{
    rb_cEnum = rb_define_class_under(rb_mClang, "Enum", rb_cBasicObject);
    rb_include_module(rb_cEnum, rb_mEnumerable);
    rb_define_alloc_func(rb_cEnum, enum_allocate);
    rb_define_method0(rb_cEnum, "each", enum_each, 0);
    rb_define_method0(rb_cEnum, "size", enum_count, 0);
    rb_define_method0(rb_cEnum, "names", enum_names, 0);
    rb_define_method0(rb_cEnum, "fields", enum_fields, 0);
    rb_define_method0(rb_cEnum, "to_h", enum_to_hash, 0);
    rb_define_method0(rb_cEnum, "to_s", enum_to_s, 0);
    rb_define_method0(rb_cEnum, "inspect", enum_inspect, 0);
    rb_define_methodm2(rb_cEnum, "mask", enum_mask, -2);
    rb_define_method1(rb_cEnum, "unmask", enum_unmask, 1);
    rb_define_method1(rb_cEnum, "symbol", enum_symbol, 1);
    rb_define_method1(rb_cEnum, "value", enum_value, 1);
    rb_define_method1(rb_cEnum, "[]", enum_symbol, 1);
    rb_define_methodm1(rb_cEnum, "method_missing", enum_method_missing, -1);
    rb_define_alias(rb_cEnum, "length", "size");
    rb_define_alias(rb_cEnum, "to_hash", "to_h");
    rb_define_alias(rb_cEnum, "to_str", "to_s");

    // CXAvailabilityKind
    enum_create(&rb_AvailabilityKind, "AvailabilityKind");
    enum_field(rb_AvailabilityKind, "available", CXAvailability_Available);
    enum_field(rb_AvailabilityKind, "deprecated", CXAvailability_Deprecated);
    enum_field(rb_AvailabilityKind, "not_available", CXAvailability_NotAvailable);
    enum_field(rb_AvailabilityKind, "not_accessible", CXAvailability_NotAccessible);

    // CXCursor_ExceptionSpecificationKind
    enum_create(&rb_CursorExceptionSpecificationKind, "CursorExceptionSpecificationKind");
    enum_field(rb_CursorExceptionSpecificationKind, "none", CXCursor_ExceptionSpecificationKind_None);
    enum_field(rb_CursorExceptionSpecificationKind, "dynamic_none", CXCursor_ExceptionSpecificationKind_DynamicNone);
    enum_field(rb_CursorExceptionSpecificationKind, "dynamic", CXCursor_ExceptionSpecificationKind_Dynamic);
    enum_field(rb_CursorExceptionSpecificationKind, "ms_any", CXCursor_ExceptionSpecificationKind_MSAny);
    enum_field(rb_CursorExceptionSpecificationKind, "basic_noexcept", CXCursor_ExceptionSpecificationKind_BasicNoexcept);
    enum_field(rb_CursorExceptionSpecificationKind, "computed_noexcept", CXCursor_ExceptionSpecificationKind_ComputedNoexcept);
    enum_field(rb_CursorExceptionSpecificationKind, "unevaluated", CXCursor_ExceptionSpecificationKind_Unevaluated);
    enum_field(rb_CursorExceptionSpecificationKind, "uninstantiated", CXCursor_ExceptionSpecificationKind_Uninstantiated);
    enum_field(rb_CursorExceptionSpecificationKind, "unparsed", CXCursor_ExceptionSpecificationKind_Unparsed);
    enum_field(rb_CursorExceptionSpecificationKind, "no_throw", CXCursor_ExceptionSpecificationKind_NoThrow);

    // CXGlobalOptFlags
    enum_create(&rb_GlobalOptFlags, "GlobalOptFlags");
    enum_field(rb_GlobalOptFlags, "none", CXGlobalOpt_None);
    enum_field(rb_GlobalOptFlags, "thread_background_priority_for_indexing", CXGlobalOpt_ThreadBackgroundPriorityForIndexing);
    enum_field(rb_GlobalOptFlags, "thread_background_priority_for_editing", CXGlobalOpt_ThreadBackgroundPriorityForEditing);
    enum_field(rb_GlobalOptFlags, "thread_background_priority_for_all", CXGlobalOpt_ThreadBackgroundPriorityForAll);

    // CXDiagnosticSeverity
    enum_create(&rb_DiagnosticSeverity, "DiagnosticSeverity");
    enum_field(rb_DiagnosticSeverity, "ignored", CXDiagnostic_Ignored);
    enum_field(rb_DiagnosticSeverity, "note", CXDiagnostic_Note);
    enum_field(rb_DiagnosticSeverity, "warning", CXDiagnostic_Warning);
    enum_field(rb_DiagnosticSeverity, "error", CXDiagnostic_Error);
    enum_field(rb_DiagnosticSeverity, "fatal", CXDiagnostic_Fatal);

    // CXDiagnosticDisplayOptions
    enum_create(&rb_DiagnosticDisplayOptions, "DiagnosticDisplayOptions");
    enum_field(rb_DiagnosticDisplayOptions, "display_source_location", CXDiagnostic_DisplaySourceLocation);
    enum_field(rb_DiagnosticDisplayOptions, "display_column", CXDiagnostic_DisplayColumn);
    enum_field(rb_DiagnosticDisplayOptions, "display_source_ranges", CXDiagnostic_DisplaySourceRanges);
    enum_field(rb_DiagnosticDisplayOptions, "display_option", CXDiagnostic_DisplayOption);
    enum_field(rb_DiagnosticDisplayOptions, "display_category_id", CXDiagnostic_DisplayCategoryId);
    enum_field(rb_DiagnosticDisplayOptions, "display_category_name", CXDiagnostic_DisplayCategoryName);

    // CXTranslationUnit_Flags
    enum_create(&rb_TranslationUnitFlags, "TranslationUnitFlags");
    enum_field(rb_TranslationUnitFlags, "none", CXTranslationUnit_None);
    enum_field(rb_TranslationUnitFlags, "detailed_preprocessing_record", CXTranslationUnit_DetailedPreprocessingRecord);
    enum_field(rb_TranslationUnitFlags, "incomplete", CXTranslationUnit_Incomplete);
    enum_field(rb_TranslationUnitFlags, "precompiled_preamble", CXTranslationUnit_PrecompiledPreamble);
    enum_field(rb_TranslationUnitFlags, "cache_completion_results", CXTranslationUnit_CacheCompletionResults);
    enum_field(rb_TranslationUnitFlags, "for_serialization", CXTranslationUnit_ForSerialization);
    enum_field(rb_TranslationUnitFlags, "cxx_chained_pch", CXTranslationUnit_CXXChainedPCH);
    enum_field(rb_TranslationUnitFlags, "skip_function_bodies", CXTranslationUnit_SkipFunctionBodies);
    enum_field(rb_TranslationUnitFlags, "include_brief_comments_in_code_completion", CXTranslationUnit_IncludeBriefCommentsInCodeCompletion);
    enum_field(rb_TranslationUnitFlags, "create_preamble_on_first_parse", CXTranslationUnit_CreatePreambleOnFirstParse);
    enum_field(rb_TranslationUnitFlags, "keep_going", CXTranslationUnit_KeepGoing);
    enum_field(rb_TranslationUnitFlags, "single_file_parse", CXTranslationUnit_SingleFileParse);
    enum_field(rb_TranslationUnitFlags, "limit_skip_function_bodies_to_preamble", CXTranslationUnit_LimitSkipFunctionBodiesToPreamble);
    enum_field(rb_TranslationUnitFlags, "include_attributed_types", CXTranslationUnit_IncludeAttributedTypes);
    enum_field(rb_TranslationUnitFlags, "visit_implicit_attributes", CXTranslationUnit_VisitImplicitAttributes);
    enum_field(rb_TranslationUnitFlags, "ignore_non_errors_from_included_files", CXTranslationUnit_IgnoreNonErrorsFromIncludedFiles);
    enum_field(rb_TranslationUnitFlags, "retain_excluded_conditional_blocks", CXTranslationUnit_RetainExcludedConditionalBlocks);

    // CXSaveTranslationUnit_Flags
    enum_create(&rb_SaveTranslationUnitFlags, "SaveTranslationUnitFlags");
    enum_field(rb_SaveTranslationUnitFlags, "none", CXSaveTranslationUnit_None);

    // CXReparse_Flags
    enum_create(&rb_ReparseFlags, "ReparseFlags");
    enum_field(rb_ReparseFlags, "none", CXReparse_None);

    // CXTUResourceUsageKind
    enum_create(&rb_TUResourceUsageKind, "TUResourceUsageKind");
    enum_field(rb_TUResourceUsageKind, "ast", CXTUResourceUsage_AST);
    enum_field(rb_TUResourceUsageKind, "identifiers", CXTUResourceUsage_Identifiers);
    enum_field(rb_TUResourceUsageKind, "selectors", CXTUResourceUsage_Selectors);
    enum_field(rb_TUResourceUsageKind, "global_completion_results", CXTUResourceUsage_GlobalCompletionResults);
    enum_field(rb_TUResourceUsageKind, "source_manager_content_cache", CXTUResourceUsage_SourceManagerContentCache);
    enum_field(rb_TUResourceUsageKind, "ast_side_tables", CXTUResourceUsage_AST_SideTables);
    enum_field(rb_TUResourceUsageKind, "source_manager_membuffer_malloc", CXTUResourceUsage_SourceManager_Membuffer_Malloc);
    enum_field(rb_TUResourceUsageKind, "source_manager_membuffer_m_map", CXTUResourceUsage_SourceManager_Membuffer_MMap);
    enum_field(rb_TUResourceUsageKind, "external_ast_source_membuffer_malloc", CXTUResourceUsage_ExternalASTSource_Membuffer_Malloc);
    enum_field(rb_TUResourceUsageKind, "external_ast_source_membuffer_m_map", CXTUResourceUsage_ExternalASTSource_Membuffer_MMap);
    enum_field(rb_TUResourceUsageKind, "preprocessor", CXTUResourceUsage_Preprocessor);
    enum_field(rb_TUResourceUsageKind, "preprocessing_record", CXTUResourceUsage_PreprocessingRecord);
    enum_field(rb_TUResourceUsageKind, "source_manager_data_structures", CXTUResourceUsage_SourceManager_DataStructures);
    enum_field(rb_TUResourceUsageKind, "preprocessor_header_search", CXTUResourceUsage_Preprocessor_HeaderSearch);
    enum_field(rb_TUResourceUsageKind, "memory_in_bytes_begin", CXTUResourceUsage_MEMORY_IN_BYTES_BEGIN);
    enum_field(rb_TUResourceUsageKind, "memory_in_bytes_end", CXTUResourceUsage_MEMORY_IN_BYTES_END);
    enum_field(rb_TUResourceUsageKind, "first", CXTUResourceUsage_First);
    enum_field(rb_TUResourceUsageKind, "last", CXTUResourceUsage_Last);

    // CXCursorKind
    enum_create(&rb_CursorKind, "CursorKind");
    enum_field(rb_CursorKind, "unexposed_decl", CXCursor_UnexposedDecl);
    enum_field(rb_CursorKind, "struct_decl", CXCursor_StructDecl);
    enum_field(rb_CursorKind, "union_decl", CXCursor_UnionDecl);
    enum_field(rb_CursorKind, "class_decl", CXCursor_ClassDecl);
    enum_field(rb_CursorKind, "enum_decl", CXCursor_EnumDecl);
    enum_field(rb_CursorKind, "field_decl", CXCursor_FieldDecl);
    enum_field(rb_CursorKind, "enum_constant_decl", CXCursor_EnumConstantDecl);
    enum_field(rb_CursorKind, "function_decl", CXCursor_FunctionDecl);
    enum_field(rb_CursorKind, "var_decl", CXCursor_VarDecl);
    enum_field(rb_CursorKind, "parm_decl", CXCursor_ParmDecl);
    enum_field(rb_CursorKind, "obj_c_interface_decl", CXCursor_ObjCInterfaceDecl);
    enum_field(rb_CursorKind, "obj_c_category_decl", CXCursor_ObjCCategoryDecl);
    enum_field(rb_CursorKind, "obj_c_protocol_decl", CXCursor_ObjCProtocolDecl);
    enum_field(rb_CursorKind, "obj_c_property_decl", CXCursor_ObjCPropertyDecl);
    enum_field(rb_CursorKind, "obj_c_ivar_decl", CXCursor_ObjCIvarDecl);
    enum_field(rb_CursorKind, "obj_c_instance_method_decl", CXCursor_ObjCInstanceMethodDecl);
    enum_field(rb_CursorKind, "obj_c_class_method_decl", CXCursor_ObjCClassMethodDecl);
    enum_field(rb_CursorKind, "obj_c_implementation_decl", CXCursor_ObjCImplementationDecl);
    enum_field(rb_CursorKind, "obj_c_category_impl_decl", CXCursor_ObjCCategoryImplDecl);
    enum_field(rb_CursorKind, "typedef_decl", CXCursor_TypedefDecl);
    enum_field(rb_CursorKind, "cxx_method", CXCursor_CXXMethod);
    enum_field(rb_CursorKind, "namespace", CXCursor_Namespace);
    enum_field(rb_CursorKind, "linkage_spec", CXCursor_LinkageSpec);
    enum_field(rb_CursorKind, "constructor", CXCursor_Constructor);
    enum_field(rb_CursorKind, "destructor", CXCursor_Destructor);
    enum_field(rb_CursorKind, "conversion_function", CXCursor_ConversionFunction);
    enum_field(rb_CursorKind, "template_type_parameter", CXCursor_TemplateTypeParameter);
    enum_field(rb_CursorKind, "non_type_template_parameter", CXCursor_NonTypeTemplateParameter);
    enum_field(rb_CursorKind, "template_template_parameter", CXCursor_TemplateTemplateParameter);
    enum_field(rb_CursorKind, "function_template", CXCursor_FunctionTemplate);
    enum_field(rb_CursorKind, "class_template", CXCursor_ClassTemplate);
    enum_field(rb_CursorKind, "class_template_partial_specialization", CXCursor_ClassTemplatePartialSpecialization);
    enum_field(rb_CursorKind, "namespace_alias", CXCursor_NamespaceAlias);
    enum_field(rb_CursorKind, "using_directive", CXCursor_UsingDirective);
    enum_field(rb_CursorKind, "using_declaration", CXCursor_UsingDeclaration);
    enum_field(rb_CursorKind, "type_alias_decl", CXCursor_TypeAliasDecl);
    enum_field(rb_CursorKind, "obj_c_synthesize_decl", CXCursor_ObjCSynthesizeDecl);
    enum_field(rb_CursorKind, "obj_c_dynamic_decl", CXCursor_ObjCDynamicDecl);
    enum_field(rb_CursorKind, "cxx_access_specifier", CXCursor_CXXAccessSpecifier);
    enum_field(rb_CursorKind, "first_decl", CXCursor_FirstDecl);
    enum_field(rb_CursorKind, "last_decl", CXCursor_LastDecl);
    enum_field(rb_CursorKind, "first_ref", CXCursor_FirstRef);
    enum_field(rb_CursorKind, "obj_c_super_class_ref", CXCursor_ObjCSuperClassRef);
    enum_field(rb_CursorKind, "obj_c_protocol_ref", CXCursor_ObjCProtocolRef);
    enum_field(rb_CursorKind, "obj_c_class_ref", CXCursor_ObjCClassRef);
    enum_field(rb_CursorKind, "type_ref", CXCursor_TypeRef);
    enum_field(rb_CursorKind, "cxx_base_specifier", CXCursor_CXXBaseSpecifier);
    enum_field(rb_CursorKind, "template_ref", CXCursor_TemplateRef);
    enum_field(rb_CursorKind, "namespace_ref", CXCursor_NamespaceRef);
    enum_field(rb_CursorKind, "member_ref", CXCursor_MemberRef);
    enum_field(rb_CursorKind, "label_ref", CXCursor_LabelRef);
    enum_field(rb_CursorKind, "overloaded_decl_ref", CXCursor_OverloadedDeclRef);
    enum_field(rb_CursorKind, "variable_ref", CXCursor_VariableRef);
    enum_field(rb_CursorKind, "last_ref", CXCursor_LastRef);
    enum_field(rb_CursorKind, "first_invalid", CXCursor_FirstInvalid);
    enum_field(rb_CursorKind, "invalid_file", CXCursor_InvalidFile);
    enum_field(rb_CursorKind, "no_decl_found", CXCursor_NoDeclFound);
    enum_field(rb_CursorKind, "not_implemented", CXCursor_NotImplemented);
    enum_field(rb_CursorKind, "invalid_code", CXCursor_InvalidCode);
    enum_field(rb_CursorKind, "last_invalid", CXCursor_LastInvalid);
    enum_field(rb_CursorKind, "first_expr", CXCursor_FirstExpr);
    enum_field(rb_CursorKind, "unexposed_expr", CXCursor_UnexposedExpr);
    enum_field(rb_CursorKind, "decl_ref_expr", CXCursor_DeclRefExpr);
    enum_field(rb_CursorKind, "member_ref_expr", CXCursor_MemberRefExpr);
    enum_field(rb_CursorKind, "call_expr", CXCursor_CallExpr);
    enum_field(rb_CursorKind, "obj_c_message_expr", CXCursor_ObjCMessageExpr);
    enum_field(rb_CursorKind, "block_expr", CXCursor_BlockExpr);
    enum_field(rb_CursorKind, "integer_literal", CXCursor_IntegerLiteral);
    enum_field(rb_CursorKind, "floating_literal", CXCursor_FloatingLiteral);
    enum_field(rb_CursorKind, "imaginary_literal", CXCursor_ImaginaryLiteral);
    enum_field(rb_CursorKind, "string_literal", CXCursor_StringLiteral);
    enum_field(rb_CursorKind, "character_literal", CXCursor_CharacterLiteral);
    enum_field(rb_CursorKind, "paren_expr", CXCursor_ParenExpr);
    enum_field(rb_CursorKind, "unary_operator", CXCursor_UnaryOperator);
    enum_field(rb_CursorKind, "array_subscript_expr", CXCursor_ArraySubscriptExpr);
    enum_field(rb_CursorKind, "binary_operator", CXCursor_BinaryOperator);
    enum_field(rb_CursorKind, "compound_assign_operator", CXCursor_CompoundAssignOperator);
    enum_field(rb_CursorKind, "conditional_operator", CXCursor_ConditionalOperator);
    enum_field(rb_CursorKind, "c_style_cast_expr", CXCursor_CStyleCastExpr);
    enum_field(rb_CursorKind, "compound_literal_expr", CXCursor_CompoundLiteralExpr);
    enum_field(rb_CursorKind, "init_list_expr", CXCursor_InitListExpr);
    enum_field(rb_CursorKind, "addr_label_expr", CXCursor_AddrLabelExpr);
    enum_field(rb_CursorKind, "stmt_expr", CXCursor_StmtExpr);
    enum_field(rb_CursorKind, "generic_selection_expr", CXCursor_GenericSelectionExpr);
    enum_field(rb_CursorKind, "gnu_null_expr", CXCursor_GNUNullExpr);
    enum_field(rb_CursorKind, "cxx_static_cast_expr", CXCursor_CXXStaticCastExpr);
    enum_field(rb_CursorKind, "cxx_dynamic_cast_expr", CXCursor_CXXDynamicCastExpr);
    enum_field(rb_CursorKind, "cxx_reinterpret_cast_expr", CXCursor_CXXReinterpretCastExpr);
    enum_field(rb_CursorKind, "cxx_const_cast_expr", CXCursor_CXXConstCastExpr);
    enum_field(rb_CursorKind, "cxx_functional_cast_expr", CXCursor_CXXFunctionalCastExpr);
    enum_field(rb_CursorKind, "cxx_typeid_expr", CXCursor_CXXTypeidExpr);
    enum_field(rb_CursorKind, "cxx_bool_literal_expr", CXCursor_CXXBoolLiteralExpr);
    enum_field(rb_CursorKind, "cxx_null_ptr_literal_expr", CXCursor_CXXNullPtrLiteralExpr);
    enum_field(rb_CursorKind, "cxx_this_expr", CXCursor_CXXThisExpr);
    enum_field(rb_CursorKind, "cxx_throw_expr", CXCursor_CXXThrowExpr);
    enum_field(rb_CursorKind, "cxx_new_expr", CXCursor_CXXNewExpr);
    enum_field(rb_CursorKind, "cxx_delete_expr", CXCursor_CXXDeleteExpr);
    enum_field(rb_CursorKind, "unary_expr", CXCursor_UnaryExpr);
    enum_field(rb_CursorKind, "obj_c_string_literal", CXCursor_ObjCStringLiteral);
    enum_field(rb_CursorKind, "obj_c_encode_expr", CXCursor_ObjCEncodeExpr);
    enum_field(rb_CursorKind, "obj_c_selector_expr", CXCursor_ObjCSelectorExpr);
    enum_field(rb_CursorKind, "obj_c_protocol_expr", CXCursor_ObjCProtocolExpr);
    enum_field(rb_CursorKind, "obj_c_bridged_cast_expr", CXCursor_ObjCBridgedCastExpr);
    enum_field(rb_CursorKind, "pack_expansion_expr", CXCursor_PackExpansionExpr);
    enum_field(rb_CursorKind, "size_of_pack_expr", CXCursor_SizeOfPackExpr);
    enum_field(rb_CursorKind, "lambda_expr", CXCursor_LambdaExpr);
    enum_field(rb_CursorKind, "obj_c_bool_literal_expr", CXCursor_ObjCBoolLiteralExpr);
    enum_field(rb_CursorKind, "obj_c_self_expr", CXCursor_ObjCSelfExpr);
    enum_field(rb_CursorKind, "omp_array_section_expr", CXCursor_OMPArraySectionExpr);
    enum_field(rb_CursorKind, "obj_c_availability_check_expr", CXCursor_ObjCAvailabilityCheckExpr);
    enum_field(rb_CursorKind, "fixed_point_literal", CXCursor_FixedPointLiteral);
    enum_field(rb_CursorKind, "omp_array_shaping_expr", CXCursor_OMPArrayShapingExpr);
    enum_field(rb_CursorKind, "omp_iterator_expr", CXCursor_OMPIteratorExpr);
    enum_field(rb_CursorKind, "cxx_addrspace_cast_expr", CXCursor_CXXAddrspaceCastExpr);
    enum_field(rb_CursorKind, "last_expr", CXCursor_LastExpr);
    enum_field(rb_CursorKind, "first_stmt", CXCursor_FirstStmt);
    enum_field(rb_CursorKind, "unexposed_stmt", CXCursor_UnexposedStmt);
    enum_field(rb_CursorKind, "label_stmt", CXCursor_LabelStmt);
    enum_field(rb_CursorKind, "compound_stmt", CXCursor_CompoundStmt);
    enum_field(rb_CursorKind, "case_stmt", CXCursor_CaseStmt);
    enum_field(rb_CursorKind, "default_stmt", CXCursor_DefaultStmt);
    enum_field(rb_CursorKind, "if_stmt", CXCursor_IfStmt);
    enum_field(rb_CursorKind, "switch_stmt", CXCursor_SwitchStmt);
    enum_field(rb_CursorKind, "while_stmt", CXCursor_WhileStmt);
    enum_field(rb_CursorKind, "do_stmt", CXCursor_DoStmt);
    enum_field(rb_CursorKind, "for_stmt", CXCursor_ForStmt);
    enum_field(rb_CursorKind, "goto_stmt", CXCursor_GotoStmt);
    enum_field(rb_CursorKind, "indirect_goto_stmt", CXCursor_IndirectGotoStmt);
    enum_field(rb_CursorKind, "continue_stmt", CXCursor_ContinueStmt);
    enum_field(rb_CursorKind, "break_stmt", CXCursor_BreakStmt);
    enum_field(rb_CursorKind, "return_stmt", CXCursor_ReturnStmt);
    enum_field(rb_CursorKind, "gcc_asm_stmt", CXCursor_GCCAsmStmt);
    enum_field(rb_CursorKind, "asm_stmt", CXCursor_AsmStmt);
    enum_field(rb_CursorKind, "obj_c_at_try_stmt", CXCursor_ObjCAtTryStmt);
    enum_field(rb_CursorKind, "obj_c_at_catch_stmt", CXCursor_ObjCAtCatchStmt);
    enum_field(rb_CursorKind, "obj_c_at_finally_stmt", CXCursor_ObjCAtFinallyStmt);
    enum_field(rb_CursorKind, "obj_c_at_throw_stmt", CXCursor_ObjCAtThrowStmt);
    enum_field(rb_CursorKind, "obj_c_at_synchronized_stmt", CXCursor_ObjCAtSynchronizedStmt);
    enum_field(rb_CursorKind, "obj_c_autorelease_pool_stmt", CXCursor_ObjCAutoreleasePoolStmt);
    enum_field(rb_CursorKind, "obj_c_for_collection_stmt", CXCursor_ObjCForCollectionStmt);
    enum_field(rb_CursorKind, "cxx_catch_stmt", CXCursor_CXXCatchStmt);
    enum_field(rb_CursorKind, "cxx_try_stmt", CXCursor_CXXTryStmt);
    enum_field(rb_CursorKind, "cxx_for_range_stmt", CXCursor_CXXForRangeStmt);
    enum_field(rb_CursorKind, "seh_try_stmt", CXCursor_SEHTryStmt);
    enum_field(rb_CursorKind, "seh_except_stmt", CXCursor_SEHExceptStmt);
    enum_field(rb_CursorKind, "seh_finally_stmt", CXCursor_SEHFinallyStmt);
    enum_field(rb_CursorKind, "ms_asm_stmt", CXCursor_MSAsmStmt);
    enum_field(rb_CursorKind, "null_stmt", CXCursor_NullStmt);
    enum_field(rb_CursorKind, "decl_stmt", CXCursor_DeclStmt);
    enum_field(rb_CursorKind, "omp_parallel_directive", CXCursor_OMPParallelDirective);
    enum_field(rb_CursorKind, "omp_simd_directive", CXCursor_OMPSimdDirective);
    enum_field(rb_CursorKind, "omp_for_directive", CXCursor_OMPForDirective);
    enum_field(rb_CursorKind, "omp_sections_directive", CXCursor_OMPSectionsDirective);
    enum_field(rb_CursorKind, "omp_section_directive", CXCursor_OMPSectionDirective);
    enum_field(rb_CursorKind, "omp_single_directive", CXCursor_OMPSingleDirective);
    enum_field(rb_CursorKind, "omp_parallel_for_directive", CXCursor_OMPParallelForDirective);
    enum_field(rb_CursorKind, "omp_parallel_sections_directive", CXCursor_OMPParallelSectionsDirective);
    enum_field(rb_CursorKind, "omp_task_directive", CXCursor_OMPTaskDirective);
    enum_field(rb_CursorKind, "omp_master_directive", CXCursor_OMPMasterDirective);
    enum_field(rb_CursorKind, "omp_critical_directive", CXCursor_OMPCriticalDirective);
    enum_field(rb_CursorKind, "omp_taskyield_directive", CXCursor_OMPTaskyieldDirective);
    enum_field(rb_CursorKind, "omp_barrier_directive", CXCursor_OMPBarrierDirective);
    enum_field(rb_CursorKind, "omp_taskwait_directive", CXCursor_OMPTaskwaitDirective);
    enum_field(rb_CursorKind, "omp_flush_directive", CXCursor_OMPFlushDirective);
    enum_field(rb_CursorKind, "seh_leave_stmt", CXCursor_SEHLeaveStmt);
    enum_field(rb_CursorKind, "omp_ordered_directive", CXCursor_OMPOrderedDirective);
    enum_field(rb_CursorKind, "omp_atomic_directive", CXCursor_OMPAtomicDirective);
    enum_field(rb_CursorKind, "omp_for_simd_directive", CXCursor_OMPForSimdDirective);
    enum_field(rb_CursorKind, "omp_parallel_for_simd_directive", CXCursor_OMPParallelForSimdDirective);
    enum_field(rb_CursorKind, "omp_target_directive", CXCursor_OMPTargetDirective);
    enum_field(rb_CursorKind, "omp_teams_directive", CXCursor_OMPTeamsDirective);
    enum_field(rb_CursorKind, "omp_taskgroup_directive", CXCursor_OMPTaskgroupDirective);
    enum_field(rb_CursorKind, "omp_cancellation_point_directive", CXCursor_OMPCancellationPointDirective);
    enum_field(rb_CursorKind, "omp_cancel_directive", CXCursor_OMPCancelDirective);
    enum_field(rb_CursorKind, "omp_target_data_directive", CXCursor_OMPTargetDataDirective);
    enum_field(rb_CursorKind, "omp_task_loop_directive", CXCursor_OMPTaskLoopDirective);
    enum_field(rb_CursorKind, "omp_task_loop_simd_directive", CXCursor_OMPTaskLoopSimdDirective);
    enum_field(rb_CursorKind, "omp_distribute_directive", CXCursor_OMPDistributeDirective);
    enum_field(rb_CursorKind, "omp_target_enter_data_directive", CXCursor_OMPTargetEnterDataDirective);
    enum_field(rb_CursorKind, "omp_target_exit_data_directive", CXCursor_OMPTargetExitDataDirective);
    enum_field(rb_CursorKind, "omp_target_parallel_directive", CXCursor_OMPTargetParallelDirective);
    enum_field(rb_CursorKind, "omp_target_parallel_for_directive", CXCursor_OMPTargetParallelForDirective);
    enum_field(rb_CursorKind, "omp_target_update_directive", CXCursor_OMPTargetUpdateDirective);
    enum_field(rb_CursorKind, "omp_distribute_parallel_for_directive", CXCursor_OMPDistributeParallelForDirective);
    enum_field(rb_CursorKind, "omp_distribute_parallel_for_simd_directive", CXCursor_OMPDistributeParallelForSimdDirective);
    enum_field(rb_CursorKind, "omp_distribute_simd_directive", CXCursor_OMPDistributeSimdDirective);
    enum_field(rb_CursorKind, "omp_target_parallel_for_simd_directive", CXCursor_OMPTargetParallelForSimdDirective);
    enum_field(rb_CursorKind, "omp_target_simd_directive", CXCursor_OMPTargetSimdDirective);
    enum_field(rb_CursorKind, "omp_teams_distribute_directive", CXCursor_OMPTeamsDistributeDirective);
    enum_field(rb_CursorKind, "omp_teams_distribute_simd_directive", CXCursor_OMPTeamsDistributeSimdDirective);
    enum_field(rb_CursorKind, "omp_teams_distribute_parallel_for_simd_directive", CXCursor_OMPTeamsDistributeParallelForSimdDirective);
    enum_field(rb_CursorKind, "omp_teams_distribute_parallel_for_directive", CXCursor_OMPTeamsDistributeParallelForDirective);
    enum_field(rb_CursorKind, "omp_target_teams_directive", CXCursor_OMPTargetTeamsDirective);
    enum_field(rb_CursorKind, "omp_target_teams_distribute_directive", CXCursor_OMPTargetTeamsDistributeDirective);
    enum_field(rb_CursorKind, "omp_target_teams_distribute_parallel_for_directive", CXCursor_OMPTargetTeamsDistributeParallelForDirective);
    enum_field(rb_CursorKind, "omp_target_teams_distribute_parallel_for_simd_directive", CXCursor_OMPTargetTeamsDistributeParallelForSimdDirective);
    enum_field(rb_CursorKind, "omp_target_teams_distribute_simd_directive", CXCursor_OMPTargetTeamsDistributeSimdDirective);
    enum_field(rb_CursorKind, "builtin_bit_cast_expr", CXCursor_BuiltinBitCastExpr);
    enum_field(rb_CursorKind, "omp_master_task_loop_directive", CXCursor_OMPMasterTaskLoopDirective);
    enum_field(rb_CursorKind, "omp_parallel_master_task_loop_directive", CXCursor_OMPParallelMasterTaskLoopDirective);
    enum_field(rb_CursorKind, "omp_master_task_loop_simd_directive", CXCursor_OMPMasterTaskLoopSimdDirective);
    enum_field(rb_CursorKind, "omp_parallel_master_task_loop_simd_directive", CXCursor_OMPParallelMasterTaskLoopSimdDirective);
    enum_field(rb_CursorKind, "omp_parallel_master_directive", CXCursor_OMPParallelMasterDirective);
    enum_field(rb_CursorKind, "omp_depobj_directive", CXCursor_OMPDepobjDirective);
    enum_field(rb_CursorKind, "omp_scan_directive", CXCursor_OMPScanDirective);
    enum_field(rb_CursorKind, "last_stmt", CXCursor_LastStmt);
    enum_field(rb_CursorKind, "translation_unit", CXCursor_TranslationUnit);
    enum_field(rb_CursorKind, "first_attr", CXCursor_FirstAttr);
    enum_field(rb_CursorKind, "unexposed_attr", CXCursor_UnexposedAttr);
    enum_field(rb_CursorKind, "ib_action_attr", CXCursor_IBActionAttr);
    enum_field(rb_CursorKind, "ib_outlet_attr", CXCursor_IBOutletAttr);
    enum_field(rb_CursorKind, "ib_outlet_collection_attr", CXCursor_IBOutletCollectionAttr);
    enum_field(rb_CursorKind, "cxx_final_attr", CXCursor_CXXFinalAttr);
    enum_field(rb_CursorKind, "cxx_override_attr", CXCursor_CXXOverrideAttr);
    enum_field(rb_CursorKind, "annotate_attr", CXCursor_AnnotateAttr);
    enum_field(rb_CursorKind, "asm_label_attr", CXCursor_AsmLabelAttr);
    enum_field(rb_CursorKind, "packed_attr", CXCursor_PackedAttr);
    enum_field(rb_CursorKind, "pure_attr", CXCursor_PureAttr);
    enum_field(rb_CursorKind, "const_attr", CXCursor_ConstAttr);
    enum_field(rb_CursorKind, "no_duplicate_attr", CXCursor_NoDuplicateAttr);
    enum_field(rb_CursorKind, "cuda_constant_attr", CXCursor_CUDAConstantAttr);
    enum_field(rb_CursorKind, "cuda_device_attr", CXCursor_CUDADeviceAttr);
    enum_field(rb_CursorKind, "cuda_global_attr", CXCursor_CUDAGlobalAttr);
    enum_field(rb_CursorKind, "cuda_host_attr", CXCursor_CUDAHostAttr);
    enum_field(rb_CursorKind, "cuda_shared_attr", CXCursor_CUDASharedAttr);
    enum_field(rb_CursorKind, "visibility_attr", CXCursor_VisibilityAttr);
    enum_field(rb_CursorKind, "dll_export", CXCursor_DLLExport);
    enum_field(rb_CursorKind, "dll_import", CXCursor_DLLImport);
    enum_field(rb_CursorKind, "ns_returns_retained", CXCursor_NSReturnsRetained);
    enum_field(rb_CursorKind, "ns_returns_not_retained", CXCursor_NSReturnsNotRetained);
    enum_field(rb_CursorKind, "ns_returns_autoreleased", CXCursor_NSReturnsAutoreleased);
    enum_field(rb_CursorKind, "ns_consumes_self", CXCursor_NSConsumesSelf);
    enum_field(rb_CursorKind, "ns_consumed", CXCursor_NSConsumed);
    enum_field(rb_CursorKind, "obj_c_exception", CXCursor_ObjCException);
    enum_field(rb_CursorKind, "obj_cns_object", CXCursor_ObjCNSObject);
    enum_field(rb_CursorKind, "obj_c_independent_class", CXCursor_ObjCIndependentClass);
    enum_field(rb_CursorKind, "obj_c_precise_lifetime", CXCursor_ObjCPreciseLifetime);
    enum_field(rb_CursorKind, "obj_c_returns_inner_pointer", CXCursor_ObjCReturnsInnerPointer);
    enum_field(rb_CursorKind, "obj_c_requires_super", CXCursor_ObjCRequiresSuper);
    enum_field(rb_CursorKind, "obj_c_root_class", CXCursor_ObjCRootClass);
    enum_field(rb_CursorKind, "obj_c_subclassing_restricted", CXCursor_ObjCSubclassingRestricted);
    enum_field(rb_CursorKind, "obj_c_explicit_protocol_impl", CXCursor_ObjCExplicitProtocolImpl);
    enum_field(rb_CursorKind, "obj_c_designated_initializer", CXCursor_ObjCDesignatedInitializer);
    enum_field(rb_CursorKind, "obj_c_runtime_visible", CXCursor_ObjCRuntimeVisible);
    enum_field(rb_CursorKind, "obj_c_boxable", CXCursor_ObjCBoxable);
    enum_field(rb_CursorKind, "flag_enum", CXCursor_FlagEnum);
    enum_field(rb_CursorKind, "convergent_attr", CXCursor_ConvergentAttr);
    enum_field(rb_CursorKind, "warn_unused_attr", CXCursor_WarnUnusedAttr);
    enum_field(rb_CursorKind, "warn_unused_result_attr", CXCursor_WarnUnusedResultAttr);
    enum_field(rb_CursorKind, "aligned_attr", CXCursor_AlignedAttr);
    enum_field(rb_CursorKind, "last_attr", CXCursor_LastAttr);
    enum_field(rb_CursorKind, "preprocessing_directive", CXCursor_PreprocessingDirective);
    enum_field(rb_CursorKind, "macro_definition", CXCursor_MacroDefinition);
    enum_field(rb_CursorKind, "macro_expansion", CXCursor_MacroExpansion);
    enum_field(rb_CursorKind, "macro_instantiation", CXCursor_MacroInstantiation);
    enum_field(rb_CursorKind, "inclusion_directive", CXCursor_InclusionDirective);
    enum_field(rb_CursorKind, "first_preprocessing", CXCursor_FirstPreprocessing);
    enum_field(rb_CursorKind, "last_preprocessing", CXCursor_LastPreprocessing);
    enum_field(rb_CursorKind, "module_import_decl", CXCursor_ModuleImportDecl);
    enum_field(rb_CursorKind, "type_alias_template_decl", CXCursor_TypeAliasTemplateDecl);
    enum_field(rb_CursorKind, "static_assert", CXCursor_StaticAssert);
    enum_field(rb_CursorKind, "friend_decl", CXCursor_FriendDecl);
    enum_field(rb_CursorKind, "first_extra_decl", CXCursor_FirstExtraDecl);
    enum_field(rb_CursorKind, "last_extra_decl", CXCursor_LastExtraDecl);
    enum_field(rb_CursorKind, "overload_candidate", CXCursor_OverloadCandidate);

    // CXLinkageKind
    enum_create(&rb_LinkageKind, "LinkageKind");
    enum_field(rb_LinkageKind, "invalid", CXLinkage_Invalid);
    enum_field(rb_LinkageKind, "no_linkage", CXLinkage_NoLinkage);
    enum_field(rb_LinkageKind, "internal", CXLinkage_Internal);
    enum_field(rb_LinkageKind, "unique_external", CXLinkage_UniqueExternal);
    enum_field(rb_LinkageKind, "external", CXLinkage_External);

    // CXVisibilityKind
    enum_create(&rb_VisibilityKind, "VisibilityKind");
    enum_field(rb_VisibilityKind, "invalid", CXVisibility_Invalid);
    enum_field(rb_VisibilityKind, "hidden", CXVisibility_Hidden);
    enum_field(rb_VisibilityKind, "protected", CXVisibility_Protected);
    enum_field(rb_VisibilityKind, "default", CXVisibility_Default);

    // CXLanguageKind
    enum_create(&rb_LanguageKind, "LanguageKind");
    enum_field(rb_LanguageKind, "invalid", CXLanguage_Invalid);
    enum_field(rb_LanguageKind, "c", CXLanguage_C);
    enum_field(rb_LanguageKind, "obj_c", CXLanguage_ObjC);
    enum_field(rb_LanguageKind, "c_plus_plus", CXLanguage_CPlusPlus);

    // CXTLSKind
    enum_create(&rb_TLSKind, "TLSKind");
    enum_field(rb_TLSKind, "none", CXTLS_None);
    enum_field(rb_TLSKind, "dynamic", CXTLS_Dynamic);
    enum_field(rb_TLSKind, "static", CXTLS_Static);

    // CXTypeKind
    enum_create(&rb_TypeKind, "TypeKind");
    enum_field(rb_TypeKind, "invalid", CXType_Invalid);
    enum_field(rb_TypeKind, "unexposed", CXType_Unexposed);
    enum_field(rb_TypeKind, "void", CXType_Void);
    enum_field(rb_TypeKind, "bool", CXType_Bool);
    enum_field(rb_TypeKind, "char_u", CXType_Char_U);
    enum_field(rb_TypeKind, "u_char", CXType_UChar);
    enum_field(rb_TypeKind, "char16", CXType_Char16);
    enum_field(rb_TypeKind, "char32", CXType_Char32);
    enum_field(rb_TypeKind, "u_short", CXType_UShort);
    enum_field(rb_TypeKind, "u_int", CXType_UInt);
    enum_field(rb_TypeKind, "u_long", CXType_ULong);
    enum_field(rb_TypeKind, "u_long_long", CXType_ULongLong);
    enum_field(rb_TypeKind, "u_int128", CXType_UInt128);
    enum_field(rb_TypeKind, "char_s", CXType_Char_S);
    enum_field(rb_TypeKind, "s_char", CXType_SChar);
    enum_field(rb_TypeKind, "w_char", CXType_WChar);
    enum_field(rb_TypeKind, "short", CXType_Short);
    enum_field(rb_TypeKind, "int", CXType_Int);
    enum_field(rb_TypeKind, "long", CXType_Long);
    enum_field(rb_TypeKind, "long_long", CXType_LongLong);
    enum_field(rb_TypeKind, "int128", CXType_Int128);
    enum_field(rb_TypeKind, "float", CXType_Float);
    enum_field(rb_TypeKind, "double", CXType_Double);
    enum_field(rb_TypeKind, "long_double", CXType_LongDouble);
    enum_field(rb_TypeKind, "null_ptr", CXType_NullPtr);
    enum_field(rb_TypeKind, "overload", CXType_Overload);
    enum_field(rb_TypeKind, "dependent", CXType_Dependent);
    enum_field(rb_TypeKind, "obj_c_id", CXType_ObjCId);
    enum_field(rb_TypeKind, "obj_c_class", CXType_ObjCClass);
    enum_field(rb_TypeKind, "obj_c_sel", CXType_ObjCSel);
    enum_field(rb_TypeKind, "float128", CXType_Float128);
    enum_field(rb_TypeKind, "half", CXType_Half);
    enum_field(rb_TypeKind, "float16", CXType_Float16);
    enum_field(rb_TypeKind, "short_accum", CXType_ShortAccum);
    enum_field(rb_TypeKind, "accum", CXType_Accum);
    enum_field(rb_TypeKind, "long_accum", CXType_LongAccum);
    enum_field(rb_TypeKind, "u_short_accum", CXType_UShortAccum);
    enum_field(rb_TypeKind, "u_accum", CXType_UAccum);
    enum_field(rb_TypeKind, "u_long_accum", CXType_ULongAccum);
    enum_field(rb_TypeKind, "b_float16", CXType_BFloat16);
    enum_field(rb_TypeKind, "first_builtin", CXType_FirstBuiltin);
    enum_field(rb_TypeKind, "last_builtin", CXType_LastBuiltin);
    enum_field(rb_TypeKind, "complex", CXType_Complex);
    enum_field(rb_TypeKind, "pointer", CXType_Pointer);
    enum_field(rb_TypeKind, "block_pointer", CXType_BlockPointer);
    enum_field(rb_TypeKind, "l_value_reference", CXType_LValueReference);
    enum_field(rb_TypeKind, "r_value_reference", CXType_RValueReference);
    enum_field(rb_TypeKind, "record", CXType_Record);
    enum_field(rb_TypeKind, "enum", CXType_Enum);
    enum_field(rb_TypeKind, "typedef", CXType_Typedef);
    enum_field(rb_TypeKind, "obj_c_interface", CXType_ObjCInterface);
    enum_field(rb_TypeKind, "obj_c_object_pointer", CXType_ObjCObjectPointer);
    enum_field(rb_TypeKind, "function_no_proto", CXType_FunctionNoProto);
    enum_field(rb_TypeKind, "function_proto", CXType_FunctionProto);
    enum_field(rb_TypeKind, "constant_array", CXType_ConstantArray);
    enum_field(rb_TypeKind, "vector", CXType_Vector);
    enum_field(rb_TypeKind, "incomplete_array", CXType_IncompleteArray);
    enum_field(rb_TypeKind, "variable_array", CXType_VariableArray);
    enum_field(rb_TypeKind, "dependent_sized_array", CXType_DependentSizedArray);
    enum_field(rb_TypeKind, "member_pointer", CXType_MemberPointer);
    enum_field(rb_TypeKind, "auto", CXType_Auto);
    enum_field(rb_TypeKind, "elaborated", CXType_Elaborated);
    enum_field(rb_TypeKind, "pipe", CXType_Pipe);
    enum_field(rb_TypeKind, "ocl_image1d_ro", CXType_OCLImage1dRO);
    enum_field(rb_TypeKind, "ocl_image1d_array_ro", CXType_OCLImage1dArrayRO);
    enum_field(rb_TypeKind, "ocl_image1d_buffer_ro", CXType_OCLImage1dBufferRO);
    enum_field(rb_TypeKind, "ocl_image2d_ro", CXType_OCLImage2dRO);
    enum_field(rb_TypeKind, "ocl_image2d_array_ro", CXType_OCLImage2dArrayRO);
    enum_field(rb_TypeKind, "ocl_image2d_depth_ro", CXType_OCLImage2dDepthRO);
    enum_field(rb_TypeKind, "ocl_image2d_array_depth_ro", CXType_OCLImage2dArrayDepthRO);
    enum_field(rb_TypeKind, "ocl_image2d_msaaro", CXType_OCLImage2dMSAARO);
    enum_field(rb_TypeKind, "ocl_image2d_array_msaaro", CXType_OCLImage2dArrayMSAARO);
    enum_field(rb_TypeKind, "ocl_image2d_msaa_depth_ro", CXType_OCLImage2dMSAADepthRO);
    enum_field(rb_TypeKind, "ocl_image2d_array_msaa_depth_ro", CXType_OCLImage2dArrayMSAADepthRO);
    enum_field(rb_TypeKind, "ocl_image3d_ro", CXType_OCLImage3dRO);
    enum_field(rb_TypeKind, "ocl_image1d_wo", CXType_OCLImage1dWO);
    enum_field(rb_TypeKind, "ocl_image1d_array_wo", CXType_OCLImage1dArrayWO);
    enum_field(rb_TypeKind, "ocl_image1d_buffer_wo", CXType_OCLImage1dBufferWO);
    enum_field(rb_TypeKind, "ocl_image2d_wo", CXType_OCLImage2dWO);
    enum_field(rb_TypeKind, "ocl_image2d_array_wo", CXType_OCLImage2dArrayWO);
    enum_field(rb_TypeKind, "ocl_image2d_depth_wo", CXType_OCLImage2dDepthWO);
    enum_field(rb_TypeKind, "ocl_image2d_array_depth_wo", CXType_OCLImage2dArrayDepthWO);
    enum_field(rb_TypeKind, "ocl_image2d_msaawo", CXType_OCLImage2dMSAAWO);
    enum_field(rb_TypeKind, "ocl_image2d_array_msaawo", CXType_OCLImage2dArrayMSAAWO);
    enum_field(rb_TypeKind, "ocl_image2d_msaa_depth_wo", CXType_OCLImage2dMSAADepthWO);
    enum_field(rb_TypeKind, "ocl_image2d_array_msaa_depth_wo", CXType_OCLImage2dArrayMSAADepthWO);
    enum_field(rb_TypeKind, "ocl_image3d_wo", CXType_OCLImage3dWO);
    enum_field(rb_TypeKind, "ocl_image1d_rw", CXType_OCLImage1dRW);
    enum_field(rb_TypeKind, "ocl_image1d_array_rw", CXType_OCLImage1dArrayRW);
    enum_field(rb_TypeKind, "ocl_image1d_buffer_rw", CXType_OCLImage1dBufferRW);
    enum_field(rb_TypeKind, "ocl_image2d_rw", CXType_OCLImage2dRW);
    enum_field(rb_TypeKind, "ocl_image2d_array_rw", CXType_OCLImage2dArrayRW);
    enum_field(rb_TypeKind, "ocl_image2d_depth_rw", CXType_OCLImage2dDepthRW);
    enum_field(rb_TypeKind, "ocl_image2d_array_depth_rw", CXType_OCLImage2dArrayDepthRW);
    enum_field(rb_TypeKind, "ocl_image2d_msaarw", CXType_OCLImage2dMSAARW);
    enum_field(rb_TypeKind, "ocl_image2d_array_msaarw", CXType_OCLImage2dArrayMSAARW);
    enum_field(rb_TypeKind, "ocl_image2d_msaa_depth_rw", CXType_OCLImage2dMSAADepthRW);
    enum_field(rb_TypeKind, "ocl_image2d_array_msaa_depth_rw", CXType_OCLImage2dArrayMSAADepthRW);
    enum_field(rb_TypeKind, "ocl_image3d_rw", CXType_OCLImage3dRW);
    enum_field(rb_TypeKind, "ocl_sampler", CXType_OCLSampler);
    enum_field(rb_TypeKind, "ocl_event", CXType_OCLEvent);
    enum_field(rb_TypeKind, "ocl_queue", CXType_OCLQueue);
    enum_field(rb_TypeKind, "ocl_reserve_id", CXType_OCLReserveID);
    enum_field(rb_TypeKind, "obj_c_object", CXType_ObjCObject);
    enum_field(rb_TypeKind, "obj_c_type_param", CXType_ObjCTypeParam);
    enum_field(rb_TypeKind, "attributed", CXType_Attributed);
    enum_field(rb_TypeKind, "ocl_intel_subgroup_avc_mce_payload", CXType_OCLIntelSubgroupAVCMcePayload);
    enum_field(rb_TypeKind, "ocl_intel_subgroup_avc_ime_payload", CXType_OCLIntelSubgroupAVCImePayload);
    enum_field(rb_TypeKind, "ocl_intel_subgroup_avc_ref_payload", CXType_OCLIntelSubgroupAVCRefPayload);
    enum_field(rb_TypeKind, "ocl_intel_subgroup_avc_sic_payload", CXType_OCLIntelSubgroupAVCSicPayload);
    enum_field(rb_TypeKind, "ocl_intel_subgroup_avc_mce_result", CXType_OCLIntelSubgroupAVCMceResult);
    enum_field(rb_TypeKind, "ocl_intel_subgroup_avc_ime_result", CXType_OCLIntelSubgroupAVCImeResult);
    enum_field(rb_TypeKind, "ocl_intel_subgroup_avc_ref_result", CXType_OCLIntelSubgroupAVCRefResult);
    enum_field(rb_TypeKind, "ocl_intel_subgroup_avc_sic_result", CXType_OCLIntelSubgroupAVCSicResult);
    enum_field(rb_TypeKind, "ocl_intel_subgroup_avc_ime_result_single_ref_streamout", CXType_OCLIntelSubgroupAVCImeResultSingleRefStreamout);
    enum_field(rb_TypeKind, "ocl_intel_subgroup_avc_ime_result_dual_ref_streamout", CXType_OCLIntelSubgroupAVCImeResultDualRefStreamout);
    enum_field(rb_TypeKind, "ocl_intel_subgroup_avc_ime_single_ref_streamin", CXType_OCLIntelSubgroupAVCImeSingleRefStreamin);
    enum_field(rb_TypeKind, "ocl_intel_subgroup_avc_ime_dual_ref_streamin", CXType_OCLIntelSubgroupAVCImeDualRefStreamin);
    enum_field(rb_TypeKind, "ext_vector", CXType_ExtVector);
    enum_field(rb_TypeKind, "atomic", CXType_Atomic);

    // CXCallingConv
    enum_create(&rb_CallingConv, "CallingConv");
    enum_field(rb_CallingConv, "default", CXCallingConv_Default);
    enum_field(rb_CallingConv, "c", CXCallingConv_C);
    enum_field(rb_CallingConv, "x86_std_call", CXCallingConv_X86StdCall);
    enum_field(rb_CallingConv, "x86_fast_call", CXCallingConv_X86FastCall);
    enum_field(rb_CallingConv, "x86_this_call", CXCallingConv_X86ThisCall);
    enum_field(rb_CallingConv, "x86_pascal", CXCallingConv_X86Pascal);
    enum_field(rb_CallingConv, "aapcs", CXCallingConv_AAPCS);
    enum_field(rb_CallingConv, "aapcs_vfp", CXCallingConv_AAPCS_VFP);
    enum_field(rb_CallingConv, "x86_reg_call", CXCallingConv_X86RegCall);
    enum_field(rb_CallingConv, "intel_ocl_bicc", CXCallingConv_IntelOclBicc);
    enum_field(rb_CallingConv, "win64", CXCallingConv_Win64);
    enum_field(rb_CallingConv, "x86_64_win64", CXCallingConv_X86_64Win64);
    enum_field(rb_CallingConv, "x86_64_sys_v", CXCallingConv_X86_64SysV);
    enum_field(rb_CallingConv, "x86_vector_call", CXCallingConv_X86VectorCall);
    enum_field(rb_CallingConv, "swift", CXCallingConv_Swift);
    enum_field(rb_CallingConv, "preserve_most", CXCallingConv_PreserveMost);
    enum_field(rb_CallingConv, "preserve_all", CXCallingConv_PreserveAll);
    enum_field(rb_CallingConv, "a_arch64_vector_call", CXCallingConv_AArch64VectorCall);
    enum_field(rb_CallingConv, "invalid", CXCallingConv_Invalid);
    enum_field(rb_CallingConv, "unexposed", CXCallingConv_Unexposed);

    // CXTemplateArgumentKind
    enum_create(&rb_TemplateArgumentKind, "TemplateArgumentKind");
    enum_field(rb_TemplateArgumentKind, "null", CXTemplateArgumentKind_Null);
    enum_field(rb_TemplateArgumentKind, "type", CXTemplateArgumentKind_Type);
    enum_field(rb_TemplateArgumentKind, "declaration", CXTemplateArgumentKind_Declaration);
    enum_field(rb_TemplateArgumentKind, "null_ptr", CXTemplateArgumentKind_NullPtr);
    enum_field(rb_TemplateArgumentKind, "integral", CXTemplateArgumentKind_Integral);
    enum_field(rb_TemplateArgumentKind, "template", CXTemplateArgumentKind_Template);
    enum_field(rb_TemplateArgumentKind, "template_expansion", CXTemplateArgumentKind_TemplateExpansion);
    enum_field(rb_TemplateArgumentKind, "expression", CXTemplateArgumentKind_Expression);
    enum_field(rb_TemplateArgumentKind, "pack", CXTemplateArgumentKind_Pack);
    enum_field(rb_TemplateArgumentKind, "invalid", CXTemplateArgumentKind_Invalid);

    // CXTypeNullabilityKind
    enum_create(&rb_TypeNullabilityKind, "TypeNullabilityKind");
    enum_field(rb_TypeNullabilityKind, "non_null", CXTypeNullability_NonNull);
    enum_field(rb_TypeNullabilityKind, "nullable", CXTypeNullability_Nullable);
    enum_field(rb_TypeNullabilityKind, "unspecified", CXTypeNullability_Unspecified);
    enum_field(rb_TypeNullabilityKind, "invalid", CXTypeNullability_Invalid);

    // CXRefQualifierKind
    enum_create(&rb_RefQualifierKind, "RefQualifierKind");
    enum_field(rb_RefQualifierKind, "none", CXRefQualifier_None);
    enum_field(rb_RefQualifierKind, "l_value", CXRefQualifier_LValue);
    enum_field(rb_RefQualifierKind, "r_value", CXRefQualifier_RValue);

    // CX_CXXAccessSpecifier
    enum_create(&rb_CXXAccessSpecifier, "CXXAccessSpecifier");
    enum_field(rb_CXXAccessSpecifier, "cxx_invalid_access_specifier", CX_CXXInvalidAccessSpecifier);
    enum_field(rb_CXXAccessSpecifier, "cxx_public", CX_CXXPublic);
    enum_field(rb_CXXAccessSpecifier, "cxx_protected", CX_CXXProtected);
    enum_field(rb_CXXAccessSpecifier, "cxx_private", CX_CXXPrivate);

    // CX_StorageClass
    enum_create(&rb_StorageClass, "StorageClass");
    enum_field(rb_StorageClass, "invalid", CX_SC_Invalid);
    enum_field(rb_StorageClass, "none", CX_SC_None);
    enum_field(rb_StorageClass, "extern", CX_SC_Extern);
    enum_field(rb_StorageClass, "static", CX_SC_Static);
    enum_field(rb_StorageClass, "private_extern", CX_SC_PrivateExtern);
    enum_field(rb_StorageClass, "open_cl_work_group_local", CX_SC_OpenCLWorkGroupLocal);
    enum_field(rb_StorageClass, "auto", CX_SC_Auto);
    enum_field(rb_StorageClass, "register", CX_SC_Register);

    // CXChildVisitResult
    enum_create(&rb_ChildVisitResult, "ChildVisitResult");
    enum_field(rb_ChildVisitResult, "break", CXChildVisit_Break);
    enum_field(rb_ChildVisitResult, "continue", CXChildVisit_Continue);
    enum_field(rb_ChildVisitResult, "recurse", CXChildVisit_Recurse);

    // CXPrintingPolicyProperty
    enum_create(&rb_PrintingPolicyProperty, "PrintingPolicyProperty");
    enum_field(rb_PrintingPolicyProperty, "indentation", CXPrintingPolicy_Indentation);
    enum_field(rb_PrintingPolicyProperty, "suppress_specifiers", CXPrintingPolicy_SuppressSpecifiers);
    enum_field(rb_PrintingPolicyProperty, "suppress_tag_keyword", CXPrintingPolicy_SuppressTagKeyword);
    enum_field(rb_PrintingPolicyProperty, "include_tag_definition", CXPrintingPolicy_IncludeTagDefinition);
    enum_field(rb_PrintingPolicyProperty, "suppress_scope", CXPrintingPolicy_SuppressScope);
    enum_field(rb_PrintingPolicyProperty, "suppress_unwritten_scope", CXPrintingPolicy_SuppressUnwrittenScope);
    enum_field(rb_PrintingPolicyProperty, "suppress_initializers", CXPrintingPolicy_SuppressInitializers);
    enum_field(rb_PrintingPolicyProperty, "constant_array_size_as_written", CXPrintingPolicy_ConstantArraySizeAsWritten);
    enum_field(rb_PrintingPolicyProperty, "anonymous_tag_locations", CXPrintingPolicy_AnonymousTagLocations);
    enum_field(rb_PrintingPolicyProperty, "suppress_strong_lifetime", CXPrintingPolicy_SuppressStrongLifetime);
    enum_field(rb_PrintingPolicyProperty, "suppress_lifetime_qualifiers", CXPrintingPolicy_SuppressLifetimeQualifiers);
    enum_field(rb_PrintingPolicyProperty, "suppress_template_args_in_cxx_constructors", CXPrintingPolicy_SuppressTemplateArgsInCXXConstructors);
    enum_field(rb_PrintingPolicyProperty, "bool", CXPrintingPolicy_Bool);
    enum_field(rb_PrintingPolicyProperty, "restrict", CXPrintingPolicy_Restrict);
    enum_field(rb_PrintingPolicyProperty, "alignof", CXPrintingPolicy_Alignof);
    enum_field(rb_PrintingPolicyProperty, "underscore_alignof", CXPrintingPolicy_UnderscoreAlignof);
    enum_field(rb_PrintingPolicyProperty, "use_void_for_zero_params", CXPrintingPolicy_UseVoidForZeroParams);
    enum_field(rb_PrintingPolicyProperty, "terse_output", CXPrintingPolicy_TerseOutput);
    enum_field(rb_PrintingPolicyProperty, "polish_for_declaration", CXPrintingPolicy_PolishForDeclaration);
    enum_field(rb_PrintingPolicyProperty, "half", CXPrintingPolicy_Half);
    enum_field(rb_PrintingPolicyProperty, "msw_char", CXPrintingPolicy_MSWChar);
    enum_field(rb_PrintingPolicyProperty, "include_newlines", CXPrintingPolicy_IncludeNewlines);
    enum_field(rb_PrintingPolicyProperty, "msvc_formatting", CXPrintingPolicy_MSVCFormatting);
    enum_field(rb_PrintingPolicyProperty, "constants_as_written", CXPrintingPolicy_ConstantsAsWritten);
    enum_field(rb_PrintingPolicyProperty, "suppress_implicit_base", CXPrintingPolicy_SuppressImplicitBase);
    enum_field(rb_PrintingPolicyProperty, "fully_qualified_name", CXPrintingPolicy_FullyQualifiedName);
    enum_field(rb_PrintingPolicyProperty, "last_property", CXPrintingPolicy_LastProperty);

    // CXObjCPropertyAttrKind
    enum_create(&rb_ObjCPropertyAttrKind, "ObjCPropertyAttrKind");
    enum_field(rb_ObjCPropertyAttrKind, "noattr", CXObjCPropertyAttr_noattr);
    enum_field(rb_ObjCPropertyAttrKind, "readonly", CXObjCPropertyAttr_readonly);
    enum_field(rb_ObjCPropertyAttrKind, "getter", CXObjCPropertyAttr_getter);
    enum_field(rb_ObjCPropertyAttrKind, "assign", CXObjCPropertyAttr_assign);
    enum_field(rb_ObjCPropertyAttrKind, "readwrite", CXObjCPropertyAttr_readwrite);
    enum_field(rb_ObjCPropertyAttrKind, "retain", CXObjCPropertyAttr_retain);
    enum_field(rb_ObjCPropertyAttrKind, "copy", CXObjCPropertyAttr_copy);
    enum_field(rb_ObjCPropertyAttrKind, "nonatomic", CXObjCPropertyAttr_nonatomic);
    enum_field(rb_ObjCPropertyAttrKind, "setter", CXObjCPropertyAttr_setter);
    enum_field(rb_ObjCPropertyAttrKind, "atomic", CXObjCPropertyAttr_atomic);
    enum_field(rb_ObjCPropertyAttrKind, "weak", CXObjCPropertyAttr_weak);
    enum_field(rb_ObjCPropertyAttrKind, "strong", CXObjCPropertyAttr_strong);
    enum_field(rb_ObjCPropertyAttrKind, "cxobjcpropertyattr_unsafe_unretained", CXObjCPropertyAttr_unsafe_unretained);
    enum_field(rb_ObjCPropertyAttrKind, "class", CXObjCPropertyAttr_class);

    // CXObjCDeclQualifierKind
    enum_create(&rb_ObjCDeclQualifierKind, "ObjCDeclQualifierKind");
    enum_field(rb_ObjCDeclQualifierKind, "none", CXObjCDeclQualifier_None);
    enum_field(rb_ObjCDeclQualifierKind, "in", CXObjCDeclQualifier_In);
    enum_field(rb_ObjCDeclQualifierKind, "inout", CXObjCDeclQualifier_Inout);
    enum_field(rb_ObjCDeclQualifierKind, "out", CXObjCDeclQualifier_Out);
    enum_field(rb_ObjCDeclQualifierKind, "bycopy", CXObjCDeclQualifier_Bycopy);
    enum_field(rb_ObjCDeclQualifierKind, "byref", CXObjCDeclQualifier_Byref);
    enum_field(rb_ObjCDeclQualifierKind, "oneway", CXObjCDeclQualifier_Oneway);

    // CXNameRefFlags
    enum_create(&rb_NameRefFlags, "NameRefFlags");
    enum_field(rb_NameRefFlags, "want_qualifier", CXNameRange_WantQualifier);
    enum_field(rb_NameRefFlags, "want_template_args", CXNameRange_WantTemplateArgs);
    enum_field(rb_NameRefFlags, "want_single_piece", CXNameRange_WantSinglePiece);

    // CXTokenKind
    enum_create(&rb_TokenKind, "TokenKind");
    enum_field(rb_TokenKind, "punctuation", CXToken_Punctuation);
    enum_field(rb_TokenKind, "keyword", CXToken_Keyword);
    enum_field(rb_TokenKind, "identifier", CXToken_Identifier);
    enum_field(rb_TokenKind, "literal", CXToken_Literal);
    enum_field(rb_TokenKind, "comment", CXToken_Comment);

    // CXCompletionChunkKind
    enum_create(&rb_CompletionChunkKind, "CompletionChunkKind");
    enum_field(rb_CompletionChunkKind, "optional", CXCompletionChunk_Optional);
    enum_field(rb_CompletionChunkKind, "typed_text", CXCompletionChunk_TypedText);
    enum_field(rb_CompletionChunkKind, "text", CXCompletionChunk_Text);
    enum_field(rb_CompletionChunkKind, "placeholder", CXCompletionChunk_Placeholder);
    enum_field(rb_CompletionChunkKind, "informative", CXCompletionChunk_Informative);
    enum_field(rb_CompletionChunkKind, "current_parameter", CXCompletionChunk_CurrentParameter);
    enum_field(rb_CompletionChunkKind, "left_paren", CXCompletionChunk_LeftParen);
    enum_field(rb_CompletionChunkKind, "right_paren", CXCompletionChunk_RightParen);
    enum_field(rb_CompletionChunkKind, "left_bracket", CXCompletionChunk_LeftBracket);
    enum_field(rb_CompletionChunkKind, "right_bracket", CXCompletionChunk_RightBracket);
    enum_field(rb_CompletionChunkKind, "left_brace", CXCompletionChunk_LeftBrace);
    enum_field(rb_CompletionChunkKind, "right_brace", CXCompletionChunk_RightBrace);
    enum_field(rb_CompletionChunkKind, "left_angle", CXCompletionChunk_LeftAngle);
    enum_field(rb_CompletionChunkKind, "right_angle", CXCompletionChunk_RightAngle);
    enum_field(rb_CompletionChunkKind, "comma", CXCompletionChunk_Comma);
    enum_field(rb_CompletionChunkKind, "result_type", CXCompletionChunk_ResultType);
    enum_field(rb_CompletionChunkKind, "colon", CXCompletionChunk_Colon);
    enum_field(rb_CompletionChunkKind, "semi_colon", CXCompletionChunk_SemiColon);
    enum_field(rb_CompletionChunkKind, "equal", CXCompletionChunk_Equal);
    enum_field(rb_CompletionChunkKind, "horizontal_space", CXCompletionChunk_HorizontalSpace);
    enum_field(rb_CompletionChunkKind, "vertical_space", CXCompletionChunk_VerticalSpace);

    // CXCodeComplete_Flags
    enum_create(&rb_CodeCompleteFlags, "CodeCompleteFlags");
    enum_field(rb_CodeCompleteFlags, "include_macros", CXCodeComplete_IncludeMacros);
    enum_field(rb_CodeCompleteFlags, "include_code_patterns", CXCodeComplete_IncludeCodePatterns);
    enum_field(rb_CodeCompleteFlags, "include_brief_comments", CXCodeComplete_IncludeBriefComments);
    enum_field(rb_CodeCompleteFlags, "skip_preamble", CXCodeComplete_SkipPreamble);
    enum_field(rb_CodeCompleteFlags, "include_completions_with_fix_its", CXCodeComplete_IncludeCompletionsWithFixIts);

    // CXCompletionContext
    enum_create(&rb_CompletionContext, "CompletionContext");
    enum_field(rb_CompletionContext, "unexposed", CXCompletionContext_Unexposed);
    enum_field(rb_CompletionContext, "any_type", CXCompletionContext_AnyType);
    enum_field(rb_CompletionContext, "any_value", CXCompletionContext_AnyValue);
    enum_field(rb_CompletionContext, "obj_c_object_value", CXCompletionContext_ObjCObjectValue);
    enum_field(rb_CompletionContext, "obj_c_selector_value", CXCompletionContext_ObjCSelectorValue);
    enum_field(rb_CompletionContext, "cxx_class_type_value", CXCompletionContext_CXXClassTypeValue);
    enum_field(rb_CompletionContext, "dot_member_access", CXCompletionContext_DotMemberAccess);
    enum_field(rb_CompletionContext, "arrow_member_access", CXCompletionContext_ArrowMemberAccess);
    enum_field(rb_CompletionContext, "obj_c_property_access", CXCompletionContext_ObjCPropertyAccess);
    enum_field(rb_CompletionContext, "enum_tag", CXCompletionContext_EnumTag);
    enum_field(rb_CompletionContext, "union_tag", CXCompletionContext_UnionTag);
    enum_field(rb_CompletionContext, "struct_tag", CXCompletionContext_StructTag);
    enum_field(rb_CompletionContext, "class_tag", CXCompletionContext_ClassTag);
    enum_field(rb_CompletionContext, "namespace", CXCompletionContext_Namespace);
    enum_field(rb_CompletionContext, "nested_name_specifier", CXCompletionContext_NestedNameSpecifier);
    enum_field(rb_CompletionContext, "obj_c_interface", CXCompletionContext_ObjCInterface);
    enum_field(rb_CompletionContext, "obj_c_protocol", CXCompletionContext_ObjCProtocol);
    enum_field(rb_CompletionContext, "obj_c_category", CXCompletionContext_ObjCCategory);
    enum_field(rb_CompletionContext, "obj_c_instance_message", CXCompletionContext_ObjCInstanceMessage);
    enum_field(rb_CompletionContext, "obj_c_class_message", CXCompletionContext_ObjCClassMessage);
    enum_field(rb_CompletionContext, "obj_c_selector_name", CXCompletionContext_ObjCSelectorName);
    enum_field(rb_CompletionContext, "macro_name", CXCompletionContext_MacroName);
    enum_field(rb_CompletionContext, "natural_language", CXCompletionContext_NaturalLanguage);
    enum_field(rb_CompletionContext, "included_file", CXCompletionContext_IncludedFile);
    enum_field(rb_CompletionContext, "unknown", CXCompletionContext_Unknown);

    // CXVisitorResult
    enum_create(&rb_VisitorResult, "VisitorResult");
    enum_field(rb_VisitorResult, "break", CXVisit_Break);
    enum_field(rb_VisitorResult, "continue", CXVisit_Continue);

    // CXResult
    enum_create(&rb_Result, "Result");
    enum_field(rb_Result, "success", CXResult_Success);
    enum_field(rb_Result, "invalid", CXResult_Invalid);
    enum_field(rb_Result, "visit_break", CXResult_VisitBreak);
}