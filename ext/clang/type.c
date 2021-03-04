#include "clang.h"

static VALUE type_spelling(VALUE self)
{
    CXType *type = DATA_PTR(self);
    CXString str = clang_getTypeSpelling(*type);
    return RUBYSTR(str);
}

static VALUE type_equal(VALUE self, VALUE other)
{
    if (CLASS_OF(self) != CLASS_OF(other))
        return Qfalse;

    CXType *t1 = DATA_PTR(self), *t2 = DATA_PTR(other);
    return RB_BOOL(clang_equalTypes(*t1, *t2));
}

static VALUE type_canonical(VALUE self)
{
    CXType *t = DATA_PTR(self), *canonical = ALLOC(CXType);
    *canonical = clang_getCanonicalType(*t);
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, canonical);
}

static VALUE type_is_const(VALUE self)
{
    CXType *t = DATA_PTR(self);
    return RB_BOOL(clang_isConstQualifiedType(*t));
}

static VALUE type_is_volatile(VALUE self)
{
    CXType *t = DATA_PTR(self);
    return RB_BOOL(clang_isVolatileQualifiedType(*t));
}

static VALUE type_is_restrict(VALUE self)
{
    CXType *t = DATA_PTR(self);
    return RB_BOOL(clang_isRestrictQualifiedType(*t));
}

static VALUE type_typedef_name(VALUE self)
{
    CXType *t = DATA_PTR(self);
    return RUBYSTR(clang_getTypedefName(*t));
}

static VALUE type_pointee(VALUE self)
{
    CXType *type = DATA_PTR(self), *pointee = ALLOC(CXType);
    *pointee = clang_getPointeeType(*type);
    if (pointee->kind == CXType_Invalid)
    {
        xfree(pointee);
        return Qnil;
    }
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, pointee);
}

static VALUE type_declaration(VALUE self)
{
    CXType *type = DATA_PTR(self);
    CXCursor *cursor = ALLOC(CXCursor);
    *cursor = clang_getTypeDeclaration(*type);
    return Data_Wrap_Struct(rb_cCXCursor, NULL, RUBY_DEFAULT_FREE, cursor);
}

static VALUE type_kind_spelling(VALUE self)
{
    CXString str = clang_getTypeKindSpelling(((CXType*)DATA_PTR(self))->kind);
    return RUBYSTR(str);
}

static VALUE type_calling_conv(VALUE self)
{
    enum CXCallingConv conv = clang_getFunctionTypeCallingConv(*((CXType*)DATA_PTR(self)));
    return rb_enum_symbol(rb_CallingConv, conv);
}

static VALUE type_result_type(VALUE self)
{
    CXType *type = DATA_PTR(self), *result = ALLOC(CXType);
    *result = clang_getResultType(*type);
    if (result->kind == CXType_Invalid)
    {
        xfree(result);
        return Qnil;
    }
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, result);
}

static VALUE type_arg_count(VALUE self)
{
    CXType *type = DATA_PTR(self);
    return INT2NUM(clang_getNumArgTypes(*type));
}

static VALUE type_arg_types(VALUE self)
{
    CXType type = *(CXType*) DATA_PTR(self);
    int n = clang_getNumArgTypes(type);
    if (n <= 0)
        return rb_ary_new_capa(0);

    VALUE ary = rb_ary_new_capa(n);
    for (int i = 0; i < n; i++)
    {
        CXType *arg = ALLOC(CXType);
        *arg = clang_getArgType(type, i);
        rb_ary_store(ary, i, Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, arg));
    }
    return ary;
}

static VALUE type_is_variadic_args(VALUE self)
{
    CXType *type = DATA_PTR(self);
    return RB_BOOL(clang_isFunctionTypeVariadic(*type));
}

static VALUE type_is_pod(VALUE self)
{
    CXType *type = DATA_PTR(self);
    return RB_BOOL(clang_isPODType(*type));
}

static VALUE type_element_type(VALUE self)
{
    CXType *type = DATA_PTR(self), *element = ALLOC(CXType);
    *element = clang_getElementType(*type);
    if (element->kind == CXType_Invalid)
    {
        xfree(element);
        return Qnil;
    }
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, element);
}

static VALUE type_element_count(VALUE self)
{
    CXType *type = DATA_PTR(self);
    return LL2NUM(clang_getNumElements(*type));
}

static VALUE type_array_type(VALUE self)
{
    CXType *type = DATA_PTR(self), *element = ALLOC(CXType);
    *element = clang_getArrayElementType(*type);
    if (element->kind == CXType_Invalid)
    {
        xfree(element);
        return Qnil;
    }
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, element);
}

static VALUE type_array_size(VALUE self)
{
    CXType *type = DATA_PTR(self);
    return LL2NUM(clang_getArraySize(*type));
}

static VALUE type_nullability(VALUE self)
{
    CXType *type = DATA_PTR(self);
    enum CXTypeNullabilityKind kind = clang_Type_getNullability(*type);
    return rb_enum_symbol(rb_TypeNullabilityKind, kind);
}

static VALUE type_sizeof(VALUE self)
{
    CXType *type = DATA_PTR(self);
    return LL2NUM(clang_Type_getSizeOf(*type));
}

static VALUE type_alignof(VALUE self)
{
    CXType *type = DATA_PTR(self);
    return LL2NUM(clang_Type_getAlignOf(*type));
}

static VALUE type_offsetof(VALUE self, VALUE field)
{
    if (NIL_P(field))
        rb_raise(rb_eArgError, "field name cannot be nil");

    const char *name = StringValueCStr(field);
    CXType *type = DATA_PTR(self);
    return LL2NUM(clang_Type_getOffsetOf(*type, name));
}

static VALUE type_modified_type(VALUE self)
{
    CXType *type = DATA_PTR(self), *result = ALLOC(CXType);
    *result = clang_Type_getModifiedType(*type);
    if (result->kind == CXType_Invalid)
    {
        xfree(result);
        return Qnil;
    }
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, result);
}

static VALUE type_value_type(VALUE self)
{
    CXType *type = DATA_PTR(self), *result = ALLOC(CXType);
    *result = clang_Type_getValueType(*type);
    if (result->kind == CXType_Invalid)
    {
        xfree(result);
        return Qnil;
    }
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, result);
}

static VALUE type_class_type(VALUE self)
{
    CXType *type = DATA_PTR(self), *result = ALLOC(CXType);
    *result = clang_Type_getClassType(*type);
    if (result->kind == CXType_Invalid)
    {
        xfree(result);
        return Qnil;
    }
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, result);
}

static VALUE type_address_space(VALUE self)
{
    return UINT2NUM(clang_getAddressSpace(*(CXType*)DATA_PTR(self)));
}

static VALUE type_named_type(VALUE self)
{
    CXType *type = DATA_PTR(self), *named = ALLOC(CXType);
    *named = clang_Type_getNamedType(*type);
    if (named->kind == CXType_Invalid)
    {
        xfree(named);
        return Qnil;
    }
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, named);
}

static VALUE type_ref_qualifier(VALUE self)
{
    enum CXRefQualifierKind kind = clang_Type_getCXXRefQualifier(*(CXType*)DATA_PTR(self));
    return rb_enum_symbol(rb_RefQualifierKind, kind);
}

static VALUE type_is_transparent(VALUE self)
{
    return RB_BOOL(clang_Type_isTransparentTagTypedef(*(CXType*)DATA_PTR(self)));
}

static VALUE type_obj_c_encoding(VALUE self)
{
    return RUBYSTR(clang_Type_getObjCEncoding(*(CXType*)DATA_PTR(self)));
}

static enum CXVisitorResult type_field_visitor(CXCursor cursor, CXClientData client_data)
{
    VALUE proc = *(VALUE*)client_data;

    CXCursor *c = ALLOC(CXCursor);
    *c = cursor;
    VALUE result = rb_proc_call(proc, Data_Wrap_Struct(rb_cCXCursor, NULL, RUBY_DEFAULT_FREE, c));

    if (SYMBOL_P(result))
        return SYM2ID(result) == rb_intern("break") ? CXVisit_Break : CXVisit_Continue;
    return CXVisit_Continue;
}

static VALUE type_visit_fields(VALUE self)
{
    rb_need_block();
    VALUE proc = rb_block_proc();
    clang_Type_visitFields(*(CXType*)DATA_PTR(self), type_field_visitor, &proc);
    return Qnil;
}

static VALUE type_exception_type(VALUE self)
{
    CXType *type = DATA_PTR(self);
    enum CXCursor_ExceptionSpecificationKind kind = clang_getExceptionSpecificationType(*type);
    if (kind < 0)
        return STR2SYM("invalid");
    return rb_enum_symbol(rb_CursorExceptionSpecificationKind, kind);
}

static VALUE type_obj_c_base_type(VALUE self)
{
    CXType *type = DATA_PTR(self), *base_type = ALLOC(CXType);
    *base_type = clang_Type_getObjCObjectBaseType(*type);
    if (base_type->kind == CXType_Invalid)
    {
        xfree(base_type);
        return Qnil;
    }
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, base_type);
}

static VALUE type_each_obj_c_arg(VALUE self)
{
    CXType type = *(CXType*)DATA_PTR(self);
    unsigned int n = clang_Type_getNumObjCTypeArgs(type);

    for (unsigned i = 0; i < n; i++)
    {
        CXType *arg = ALLOC(CXType);
        *arg = clang_Type_getObjCTypeArg(type, i);
        rb_yield(Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, arg));
    }

    return self;
}

static VALUE type_each_obj_c_protocol(VALUE self)
{
    CXType type = *(CXType*)DATA_PTR(self);
    unsigned int n = clang_Type_getNumObjCProtocolRefs(type);

    for (unsigned i = 0; i < n; i++)
    {
        CXCursor *arg = ALLOC(CXCursor);
        *arg = clang_Type_getObjCProtocolDecl(type, i);
        rb_yield(Data_Wrap_Struct(rb_cCXCursor, NULL, RUBY_DEFAULT_FREE, arg));
    }

    return self;
}

void Init_clang_type(void)
{
    rb_define_method0(rb_cCXType, "modified_type", type_modified_type, 0);
    rb_define_method0(rb_cCXType, "value_type", type_value_type, 0);
    rb_define_method0(rb_cCXType, "class_type", type_class_type, 0);
    rb_define_method0(rb_cCXType, "named_type", type_named_type, 0);

    rb_define_method0(rb_cCXType, "obj_c_encoding", type_obj_c_encoding, 0);
    rb_define_method0(rb_cCXType, "obj_c_base_type", type_obj_c_base_type, 0);
    rb_define_method0(rb_cCXType, "each_obj_c_arg", type_each_obj_c_arg, 0);
    rb_define_method0(rb_cCXType, "each_obj_c_protocol", type_each_obj_c_protocol, 0);

    rb_define_method0(rb_cCXType, "ref_qualifier", type_ref_qualifier, 0);
    rb_define_method0(rb_cCXType, "alignof", type_alignof, 0);
    rb_define_method0(rb_cCXType, "sizeof", type_sizeof, 0);
    rb_define_method1(rb_cCXType, "field_offset", type_offsetof, 1);
    rb_define_method0(rb_cCXType, "arg_count", type_arg_count, 0);
    rb_define_method0(rb_cCXType, "arg_types", type_arg_types, 0);
    rb_define_method0(rb_cCXType, "variadic?", type_is_variadic_args, 0);
    rb_define_method0(rb_cCXType, "element_count", type_element_count, 0);
    rb_define_method0(rb_cCXType, "element_type", type_element_type, 0);
    rb_define_method0(rb_cCXType, "array_size", type_array_size, 0);
    rb_define_method0(rb_cCXType, "array_type", type_array_type, 0);
    rb_define_method0(rb_cCXType, "nullability", type_nullability, 0);
    rb_define_method0(rb_cCXType, "address_space", type_address_space, 0);
    rb_define_method0(rb_cCXType, "result_type", type_result_type, 0);
    rb_define_method0(rb_cCXType, "calling_conv", type_calling_conv, 0);
    rb_define_method0(rb_cCXType, "kind_spelling", type_kind_spelling, 0);
    rb_define_method0(rb_cCXType, "declaration", type_declaration, 0);
    rb_define_method0(rb_cCXType, "canonical", type_canonical, 0);
    rb_define_method0(rb_cCXType, "pointee_type", type_pointee, 0);
    rb_define_method0(rb_cCXType, "declaration", type_declaration, 0);
    rb_define_method0(rb_cCXType, "const?", type_is_const, 0);
    rb_define_method0(rb_cCXType, "volatile?", type_is_volatile, 0);
    rb_define_method0(rb_cCXType, "restrict?", type_is_restrict, 0);
    rb_define_method0(rb_cCXType, "pod?", type_is_pod, 0);
    rb_define_method0(rb_cCXType, "transparent?", type_is_transparent, 0);
    rb_define_method0(rb_cCXCursor, "exception_type", type_exception_type, 0);

    rb_define_method0(rb_cCXType, "typedef_name", type_typedef_name, 0);
    rb_define_method1(rb_cCXType, "==", type_equal, 1);
    rb_define_method1(rb_cCXType, "eql?", type_equal, 1);
    rb_define_method0(rb_cCXType, "spelling", type_spelling, 0);
    rb_define_method0(rb_cCXType, "to_s", type_spelling, 0);
    rb_define_method0(rb_cCXType, "to_str", type_spelling, 0);

    rb_define_alias(rb_cCXType, "plain_data?", "pod?");
}