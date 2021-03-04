#include "clang.h"

#define POLICY_BOOL_ATTR(name, param)                                                                                  \
    static VALUE name##_get(VALUE self)                                                                                \
    {                                                                                                                  \
        return RB_BOOL(clang_PrintingPolicy_getProperty(DATA_PTR(self), param));                                       \
    }                                                                                                                  \
    static VALUE name##_set(VALUE self, VALUE value)                                                                   \
    {                                                                                                                  \
        clang_PrintingPolicy_setProperty(DATA_PTR(self), param, RTEST(value));                                         \
        return value;                                                                                                  \
    }

#define ATTR_ACCESSOR(name)\
rb_define_method0(rb_cCXPrintingPolicy, #name, name##_get, 0);\
rb_define_method1(rb_cCXPrintingPolicy, #name"=", name##_set, 1);

POLICY_BOOL_ATTR(suppress_specifiers, CXPrintingPolicy_SuppressSpecifiers)
POLICY_BOOL_ATTR(suppress_tag_keyword, CXPrintingPolicy_SuppressTagKeyword)
POLICY_BOOL_ATTR(suppress_include_tag_definition, CXPrintingPolicy_IncludeTagDefinition)
POLICY_BOOL_ATTR(suppress_scope, CXPrintingPolicy_SuppressScope)
POLICY_BOOL_ATTR(suppress_unwritten_scope, CXPrintingPolicy_SuppressUnwrittenScope)
POLICY_BOOL_ATTR(suppress_initializers, CXPrintingPolicy_SuppressInitializers)
POLICY_BOOL_ATTR(constant_array_size_written, CXPrintingPolicy_ConstantArraySizeAsWritten)
POLICY_BOOL_ATTR(anonymous_tag_location, CXPrintingPolicy_AnonymousTagLocations)
POLICY_BOOL_ATTR(suppress_strong_lifetime, CXPrintingPolicy_SuppressStrongLifetime)
POLICY_BOOL_ATTR(suppress_lifetime_qualifiers, CXPrintingPolicy_SuppressLifetimeQualifiers)
POLICY_BOOL_ATTR(suppress_templat_args_in_ctor, CXPrintingPolicy_SuppressTemplateArgsInCXXConstructors)
POLICY_BOOL_ATTR(bool, CXPrintingPolicy_Bool)
POLICY_BOOL_ATTR(restrict, CXPrintingPolicy_Restrict)
POLICY_BOOL_ATTR(alignof, CXPrintingPolicy_Alignof)
POLICY_BOOL_ATTR(underscore_alignof, CXPrintingPolicy_UnderscoreAlignof)
POLICY_BOOL_ATTR(void_for_zero_params, CXPrintingPolicy_UseVoidForZeroParams)
POLICY_BOOL_ATTR(terse_output, CXPrintingPolicy_TerseOutput)
POLICY_BOOL_ATTR(polish_delcaration, CXPrintingPolicy_PolishForDeclaration)
POLICY_BOOL_ATTR(half, CXPrintingPolicy_Half)
POLICY_BOOL_ATTR(mswchar, CXPrintingPolicy_MSWChar)
POLICY_BOOL_ATTR(include_newlines, CXPrintingPolicy_IncludeNewlines)
POLICY_BOOL_ATTR(msvc_formatting, CXPrintingPolicy_MSVCFormatting)
POLICY_BOOL_ATTR(constant_as_written, CXPrintingPolicy_ConstantsAsWritten)
POLICY_BOOL_ATTR(suppress_implicit_base, CXPrintingPolicy_SuppressImplicitBase)
POLICY_BOOL_ATTR(full_qualified_name, CXPrintingPolicy_FullyQualifiedName)
         
static VALUE indentation_get(VALUE self)                                           
{                                                                             
    return UINT2NUM(clang_PrintingPolicy_getProperty(DATA_PTR(self), CXPrintingPolicy_Indentation)); 
}        
                                                                        
static VALUE indentation_set(VALUE self, VALUE value)                               
{                                                                             
    clang_PrintingPolicy_setProperty(DATA_PTR(self), CXPrintingPolicy_Indentation, NUM2UINT(value)); 
    return value;                                                             
}

static VALUE policy_initialize(VALUE self, VALUE cursor)
{
    rb_assert_type(cursor, rb_cCXCursor);
    RDATA(self)->data = clang_getCursorPrintingPolicy(*(CXCursor*)DATA_PTR(cursor));
    RDATA(self)->dfree = clang_PrintingPolicy_dispose;
    return self;
}

static VALUE policy_to_hash(VALUE self)
{
    VALUE hash = rb_hash_new();
    rb_hash_aset(hash, STR2SYM("indentation"),  indentation_get(self));
    rb_hash_aset(hash, STR2SYM("suppress_specifiers"),  suppress_specifiers_get(self));
    rb_hash_aset(hash, STR2SYM("suppress_tag_keyword"),  suppress_tag_keyword_get(self));
    rb_hash_aset(hash, STR2SYM("suppress_include_tag_definition"),  suppress_include_tag_definition_get(self));
    rb_hash_aset(hash, STR2SYM("suppress_scope"),  suppress_scope_get(self));
    rb_hash_aset(hash, STR2SYM("suppress_unwritten_scope"),  suppress_unwritten_scope_get(self));
    rb_hash_aset(hash, STR2SYM("suppress_initializers"),  suppress_initializers_get(self));
    rb_hash_aset(hash, STR2SYM("constant_array_size_written"),  constant_array_size_written_get(self));
    rb_hash_aset(hash, STR2SYM("anonymous_tag_location"),  anonymous_tag_location_get(self));
    rb_hash_aset(hash, STR2SYM("suppress_strong_lifetime"),  suppress_strong_lifetime_get(self));
    rb_hash_aset(hash, STR2SYM("suppress_lifetime_qualifiers"),  suppress_lifetime_qualifiers_get(self));
    rb_hash_aset(hash, STR2SYM("suppress_templat_args_in_ctor"),  suppress_templat_args_in_ctor_get(self));
    rb_hash_aset(hash, STR2SYM("bool"),  bool_get(self));
    rb_hash_aset(hash, STR2SYM("restrict"),  restrict_get(self));
    rb_hash_aset(hash, STR2SYM("alignof"),  alignof_get(self));
    rb_hash_aset(hash, STR2SYM("underscore_alignof"),  underscore_alignof_get(self));
    rb_hash_aset(hash, STR2SYM("void_for_zero_params"),  void_for_zero_params_get(self));
    rb_hash_aset(hash, STR2SYM("terse_output"),  terse_output_get(self));
    rb_hash_aset(hash, STR2SYM("polish_delcaration"),  polish_delcaration_get(self));
    rb_hash_aset(hash, STR2SYM("half"),  half_get(self));
    rb_hash_aset(hash, STR2SYM("mswchar"),  mswchar_get(self));
    rb_hash_aset(hash, STR2SYM("include_newlines"),  include_newlines_get(self));
    rb_hash_aset(hash, STR2SYM("msvc_formatting"),  msvc_formatting_get(self));
    rb_hash_aset(hash, STR2SYM("constant_as_written"),  constant_as_written_get(self));
    rb_hash_aset(hash, STR2SYM("suppress_implicit_base"),  suppress_implicit_base_get(self));
    rb_hash_aset(hash, STR2SYM("full_qualified_name"),  full_qualified_name_get(self));
    return hash;
}

void Init_clang_printing_policy(void)
{
    rb_define_method1(rb_cCXPrintingPolicy, "initialize", policy_initialize, 1);
    rb_define_method0(rb_cCXPrintingPolicy, "to_h", policy_to_hash, 0);
    rb_define_alias(rb_cCXPrintingPolicy, "to_hash", "to_h");

    ATTR_ACCESSOR(indentation);
    ATTR_ACCESSOR(suppress_specifiers);
    ATTR_ACCESSOR(suppress_tag_keyword);
    ATTR_ACCESSOR(suppress_include_tag_definition);
    ATTR_ACCESSOR(suppress_scope);
    ATTR_ACCESSOR(suppress_unwritten_scope);
    ATTR_ACCESSOR(suppress_initializers);
    ATTR_ACCESSOR(constant_array_size_written);
    ATTR_ACCESSOR(anonymous_tag_location);
    ATTR_ACCESSOR(suppress_strong_lifetime);
    ATTR_ACCESSOR(suppress_lifetime_qualifiers);
    ATTR_ACCESSOR(suppress_templat_args_in_ctor);
    ATTR_ACCESSOR(bool);
    ATTR_ACCESSOR(restrict);
    ATTR_ACCESSOR(alignof);
    ATTR_ACCESSOR(underscore_alignof);
    ATTR_ACCESSOR(void_for_zero_params);
    ATTR_ACCESSOR(terse_output);
    ATTR_ACCESSOR(polish_delcaration);
    ATTR_ACCESSOR(half);
    ATTR_ACCESSOR(mswchar);
    ATTR_ACCESSOR(include_newlines);
    ATTR_ACCESSOR(msvc_formatting);
    ATTR_ACCESSOR(constant_as_written);
    ATTR_ACCESSOR(suppress_implicit_base);
    ATTR_ACCESSOR(full_qualified_name);
}