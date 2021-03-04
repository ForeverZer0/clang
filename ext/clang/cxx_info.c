#include "clang.h"

#define CXXINFO_GET_BOOL(name, func)                                                                                   \
    static VALUE name(VALUE self)                                                                                      \
    {                                                                                                                  \
        return RB_BOOL(func(*(CXCursor *)DATA_PTR(self)));                                                             \
    }

VALUE rb_cCXXInfo;
VALUE rb_mUSR;

CXXINFO_GET_BOOL(cxx_converting_ctor, clang_CXXConstructor_isConvertingConstructor)
CXXINFO_GET_BOOL(cxx_copy_ctor, clang_CXXConstructor_isCopyConstructor)
CXXINFO_GET_BOOL(cxx_move_ctor, clang_CXXConstructor_isMoveConstructor)
CXXINFO_GET_BOOL(cxx_mutable_field, clang_CXXField_isMutable)
CXXINFO_GET_BOOL(cxx_static_method, clang_CXXMethod_isStatic)
CXXINFO_GET_BOOL(cxx_abstract, clang_CXXRecord_isAbstract)
CXXINFO_GET_BOOL(cxx_scoped, clang_EnumDecl_isScoped)
CXXINFO_GET_BOOL(cxx_const, clang_CXXMethod_isConst)

static VALUE cxx_virtual_method(int argc, VALUE *argv, VALUE self)
{
    VALUE pure;
    rb_scan_args(argc, argv, "01", &pure);

    CXCursor cursor = *(CXCursor*)DATA_PTR(self);
    unsigned int result = RTEST(pure) ? clang_CXXMethod_isPureVirtual(cursor): clang_CXXMethod_isVirtual(cursor);
    return RB_BOOL(result);
}

static VALUE cxx_is_default(VALUE self)
{
    CXCursor c = *(CXCursor*)DATA_PTR(self);
    return RB_BOOL(clang_CXXMethod_isDefaulted(c) || clang_CXXConstructor_isDefaultConstructor(c));
}

static VALUE cxx_alloc(VALUE klass)
{
    CXCursor *c = ALLOC(CXCursor);
    *c = clang_getNullCursor();
    return Data_Wrap_Struct(rb_cCXXInfo, NULL, RUBY_DEFAULT_FREE, c);
}

static VALUE cxx_initialize(VALUE self, VALUE cursor)
{
    rb_assert_type(cursor, rb_cCXCursor);
    memcpy(DATA_PTR(self), DATA_PTR(cursor), sizeof(CXCursor));
    return self;
}

static VALUE cxx_template_kind(VALUE self)
{
    enum CXCursorKind kind = clang_getTemplateCursorKind(*(CXCursor*)DATA_PTR(self));
    return rb_enum_symbol(rb_CursorKind, kind);
}

static VALUE cxx_spec_template(VALUE self)
{
    CXCursor *c = DATA_PTR(self), *s = ALLOC(CXCursor);
    *s = clang_getSpecializedCursorTemplate(*c);
    return Data_Wrap_Struct(rb_cCXCursor, NULL, RUBY_DEFAULT_FREE, s);
}

static VALUE cxx_reference_range(int argc, VALUE *argv, VALUE self)
{
    VALUE index, flags;
    rb_scan_args(argc, argv, "1*", &index, &flags);

    CXCursor *c = DATA_PTR(self);
    CXSourceRange *range = ALLOC(CXSourceRange);
    unsigned int i = UINT2NUM(index);
    unsigned int mask = rb_enum_mask(rb_NameRefFlags, flags);
    
    *range = clang_getCursorReferenceNameRange(*c, mask, i);
    return Data_Wrap_Struct(rb_cCXSourceRange, NULL, RUBY_DEFAULT_FREE, range);
}

static VALUE cxx_access_specifier(VALUE self)
{
    enum CX_CXXAccessSpecifier spec = clang_getCXXAccessSpecifier(*(CXCursor*)DATA_PTR(self));
    return rb_enum_symbol(rb_CXXAccessSpecifier, spec);
}

static VALUE usr_from_cursor(VALUE usr, VALUE cursor)
{
    rb_assert_type(cursor, rb_cCXCursor);
    CXString str = clang_getCursorUSR(*(CXCursor*)DATA_PTR(cursor));
    return RUBYSTR(str);
}

static VALUE usr_obj_c_class(VALUE usr, VALUE class_name)
{
    const char *name = StringValueCStr(class_name);
    return RUBYSTR(clang_constructUSR_ObjCClass(name));
}

static VALUE usr_obj_c_category(VALUE usr, VALUE class_name, VALUE category_name)
{
    const char *klass = StringValueCStr(class_name);
    const char *category = StringValueCStr(category_name);
    return RUBYSTR(clang_constructUSR_ObjCCategory(klass, category));
}

static VALUE usr_obj_c_protocol(VALUE usr, VALUE protocol_name)
{
    const char *name = StringValueCStr(protocol_name);
    return RUBYSTR(clang_constructUSR_ObjCProtocol(name));
}

static VALUE usr_obj_c_ivar(VALUE usr, VALUE class_name, VALUE ivar_name)
{
    CXString klass = clang_constructUSR_ObjCClass(StringValueCStr(class_name));
    CXString ivar = clang_constructUSR_ObjCIvar(StringValueCStr(ivar_name), klass);
    clang_disposeString(klass);
    return RUBYSTR(ivar);
}

static VALUE usr_obj_c_property(VALUE usr, VALUE class_name, VALUE property_name)
{
    CXString klass = clang_constructUSR_ObjCClass(StringValueCStr(class_name));
    CXString property = clang_constructUSR_ObjCProperty(StringValueCStr(property_name), klass);
    clang_disposeString(klass);
    return RUBYSTR(property);
}

static VALUE usr_obj_c_method(VALUE usr, VALUE class_name, VALUE method_name, VALUE instance)
{
    CXString klass = clang_constructUSR_ObjCClass(StringValueCStr(class_name));
    CXString method = clang_constructUSR_ObjCMethod(StringValueCStr(method_name), RTEST(instance), klass);
    clang_disposeString(klass);
    return RUBYSTR(method);
}

void Init_clang_cxx_info(void)
{
    rb_mUSR = rb_define_module_under(rb_mClang, "USR");
    rb_define_singleton_method1(rb_mUSR, "from_cursor", usr_from_cursor, 1);
    rb_define_singleton_method1(rb_mUSR, "obj_c_class", usr_obj_c_class, 1);
    rb_define_singleton_method2(rb_mUSR, "obj_c_category", usr_obj_c_category, 2);
    rb_define_singleton_method1(rb_mUSR, "obj_c_protocol", usr_obj_c_protocol, 1);
    rb_define_singleton_method2(rb_mUSR, "obj_c_ivar", usr_obj_c_ivar, 2);
    rb_define_singleton_method2(rb_mUSR, "obj_c_property", usr_obj_c_property, 2);
    rb_define_singleton_method3(rb_mUSR, "obj_c_method", usr_obj_c_method, 3);


    rb_cCXXInfo = rb_define_class_under(rb_cCXCursor, "CXXInfo", rb_cObject);
    rb_define_alloc_func(rb_cCXXInfo, cxx_alloc);
    rb_define_method1(rb_cCXXInfo, "initialize", cxx_initialize, 1);

    rb_define_method0(rb_cCXXInfo, "access_specifier", cxx_access_specifier, 0);
    rb_define_method0(rb_cCXXInfo, "template_kind", cxx_template_kind, 0);
    rb_define_methodm1(rb_cCXXInfo, "reference_range", cxx_reference_range, -1);
    rb_define_method0(rb_cCXXInfo, "specialized_template", cxx_spec_template, 0);
    rb_define_method0(rb_cCXXInfo, "converting?", cxx_converting_ctor, 0);
    rb_define_method0(rb_cCXXInfo, "copy?", cxx_copy_ctor, 0);
    rb_define_method0(rb_cCXXInfo, "default?", cxx_is_default, 0);
    rb_define_method0(rb_cCXXInfo, "move?", cxx_move_ctor, 0);
    rb_define_method0(rb_cCXXInfo, "mutable?", cxx_mutable_field, 0);
    rb_define_method0(rb_cCXXInfo, "static?", cxx_static_method, 0);
    rb_define_methodm1(rb_cCXXInfo, "virtual?", cxx_virtual_method, -1);
    rb_define_method0(rb_cCXXInfo, "abstract?", cxx_abstract, 0);
    rb_define_method0(rb_cCXXInfo, "scoped?", cxx_scoped, 0);
    rb_define_method0(rb_cCXXInfo, "const?", cxx_const, 0);
}