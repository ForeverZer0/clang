#include "clang.h"

static VALUE range_null(VALUE klass)
{
    CXSourceRange *range = ALLOC(CXSourceRange);
    *range = clang_getNullRange();
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, range);
}

static VALUE range_is_null(VALUE self)
{
    CXSourceRange *range = DATA_PTR(self);
    return RB_BOOL(clang_Range_isNull(*range));
}

static VALUE range_equal(VALUE self, VALUE other)
{
    if (CLASS_OF(self) != CLASS_OF(other))
        return Qfalse;

    CXSourceRange *range1 = DATA_PTR(self), *range2 = DATA_PTR(other);
    return RB_BOOL(clang_equalRanges(*range1, *range2));
}

static VALUE range_initialize(VALUE self, VALUE start, VALUE end)
{
    rb_assert_type(start, rb_cCXSourceLocation);
    rb_assert_type(end, rb_cCXSourceLocation);

    CXSourceLocation *loc1 = DATA_PTR(start), *loc2 = DATA_PTR(end);
    CXSourceRange range = clang_getRange(*loc1, *loc2);
    memcpy(DATA_PTR(self), &range, sizeof(CXSourceRange));
    return self;
}   

static VALUE range_begin(VALUE self)
{
    CXSourceLocation *loc = ALLOC(CXSourceLocation);
    CXSourceRange *range = DATA_PTR(self);

    *loc = clang_getRangeStart(*range);
    return Data_Wrap_Struct(rb_cCXSourceLocation, NULL, RUBY_DEFAULT_FREE, loc);
}

static VALUE range_end(VALUE self)
{
    CXSourceLocation *loc = ALLOC(CXSourceLocation);
    CXSourceRange *range = DATA_PTR(self);

    *loc = clang_getRangeEnd(*range);
    return Data_Wrap_Struct(rb_cCXSourceLocation, NULL, RUBY_DEFAULT_FREE, loc);
}

void Init_clang_source_range(void)
{
    rb_define_singleton_method0(rb_cCXSourceRange, "null", range_null, 0);

    rb_define_method2(rb_cCXSourceRange, "initialize", range_initialize, 2);
    rb_define_method0(rb_cCXSourceRange, "null?", range_is_null, 0);
    rb_define_method1(rb_cCXSourceRange, "==", range_equal, 1);
    rb_define_method1(rb_cCXSourceRange, "eql?", range_equal, 1);

    rb_define_method0(rb_cCXSourceRange, "begin", range_begin, 0);
    rb_define_method0(rb_cCXSourceRange, "end", range_end, 0);
}