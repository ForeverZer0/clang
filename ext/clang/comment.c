#include "clang.h"
#include <clang-c/Documentation.h>

VALUE enum_allocate(VALUE klass);
void enum_field(VALUE enumeration, const char *name, unsigned int value);

#define COMMENT(v) (*(CXComment*) DATA_PTR(v))
#define COMMENT_SUBCLASS(klass, name) klass = rb_define_class_under(rb_cCXComment, name, rb_cCXComment)

#define cText                 comment_classes[CXComment_Text]
#define cInlineCommand        comment_classes[CXComment_InlineCommand]
#define cHTMLStartTag         comment_classes[CXComment_HTMLStartTag]
#define cHTMLEndTag           comment_classes[CXComment_HTMLEndTag]
#define cParagraph            comment_classes[CXComment_Paragraph]
#define cBlockCommand         comment_classes[CXComment_BlockCommand]
#define cParamCommand         comment_classes[CXComment_ParamCommand]
#define cTParamCommand        comment_classes[CXComment_TParamCommand]
#define cVerbatimBlockCommand comment_classes[CXComment_VerbatimBlockCommand]
#define cVerbatimBlockLine    comment_classes[CXComment_VerbatimBlockLine]
#define cVerbatimLine         comment_classes[CXComment_VerbatimLine]
#define cFullComment          comment_classes[CXComment_FullComment]

#define COMMENT_FIRST CXComment_Text
#define COMMENT_LAST CXComment_FullComment

VALUE rb_cCXComment;
VALUE render_kind;
static VALUE comment_classes[COMMENT_LAST + 1];

static VALUE wrap_comment(CXComment *comment)
{
    if (!comment)
        return Qnil;

    VALUE klass;
    enum CXCommentKind kind = clang_Comment_getKind(*comment);
    if (kind < COMMENT_FIRST)
    {
        xfree(comment);
        return Qnil;
    }

    klass = (kind > COMMENT_LAST) ? rb_cCXComment : comment_classes[kind];
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, comment);     
}

static VALUE comment_alloc(VALUE klass)
{
    CXComment *comment = ALLOC(CXComment);
    memset(comment, 0, sizeof(CXComment));
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, comment);
}

static VALUE cursor_comment(VALUE self)
{
    CXComment *comment = ALLOC(CXComment);
    *comment = clang_Cursor_getParsedComment(*(CXCursor*) DATA_PTR(self));
    return wrap_comment(comment);
}

static VALUE comment_has_children(VALUE self)
{
    return RB_BOOL(clang_Comment_getNumChildren(COMMENT(self)));
}

static VALUE comment_child_count(VALUE self)
{
    return UINT2NUM(clang_Comment_getNumChildren(COMMENT(self)));
}

static VALUE comment_children(VALUE self)
{
    CXComment comment = COMMENT(self);
    unsigned int n = clang_Comment_getNumChildren(comment);
    VALUE ary = rb_ary_new_capa(n);

    for (unsigned i = 0; i < n; i++)
    {
        CXComment *child = ALLOC(CXComment);
        *child = clang_Comment_getChild(comment, i);
        rb_ary_store(ary, i, wrap_comment(child));
    }
    return ary;
}

static VALUE comment_each_child(VALUE self)
{
    RETURN_ENUMERATOR(self, 0, NULL);

    CXComment comment = COMMENT(self);
    unsigned int n = clang_Comment_getNumChildren(comment);

    for (unsigned i = 0; i < n; i++)
    {
        CXComment *child = ALLOC(CXComment);
        *child = clang_Comment_getChild(comment, i);
        rb_yield(wrap_comment(child));
    }
    return self;
}

static VALUE comment_is_whitespace(VALUE self)
{
    return RB_BOOL(clang_Comment_isWhitespace(COMMENT(self)));
}

static VALUE comment_trailing_newline(VALUE self)
{
    return RB_BOOL(clang_InlineContentComment_hasTrailingNewline(COMMENT(self)));
}

static VALUE comment_text(VALUE self)
{
    return rb_str_new_cstr("");
}

static VALUE text_text(VALUE self)
{
    return RUBYSTR(clang_TextComment_getText(COMMENT(self)));
}

static VALUE inline_command_name(VALUE self)
{
    return RUBYSTR(clang_InlineCommandComment_getCommandName(COMMENT(self)));
}

static VALUE inline_command_render_kind(VALUE self)
{
    unsigned int kind = clang_InlineCommandComment_getRenderKind(COMMENT(self));
    return rb_enum_symbol(render_kind, kind);
}

static VALUE inline_command_arg_count(VALUE self)
{
    return UINT2NUM(clang_InlineCommandComment_getNumArgs(COMMENT(self)));
}

static VALUE inline_command_args(VALUE self)
{
    CXComment comment = COMMENT(self);
    unsigned int n = clang_InlineCommandComment_getNumArgs(comment);

    VALUE ary = rb_ary_new_capa(n);
    for (unsigned i = 0; i < n; i++)
    {
        CXString str = clang_InlineCommandComment_getArgText(comment, i);
        rb_ary_store(ary, i, RUBYSTR(str));
    }
    return ary;
}

static VALUE inline_command_each_arg(VALUE self)
{
    RETURN_ENUMERATOR(self, 0, NULL);

    CXComment comment = COMMENT(self);
    unsigned int n = clang_InlineCommandComment_getNumArgs(comment);

    for (unsigned i = 0; i < n; i++)
    {
        CXString str = clang_InlineCommandComment_getArgText(comment, i);
        rb_yield(RUBYSTR(str));
    }
    return self;
}

static VALUE inline_command_text(VALUE self)
{
    // TODO
    return rb_ary_join(inline_command_args(self), Qnil);
}

static VALUE html_tag_name(VALUE self)
{
    return RUBYSTR(clang_HTMLTagComment_getTagName(COMMENT(self)));
}

static VALUE html_tag_text(VALUE self)
{
    return RUBYSTR(clang_HTMLTagComment_getAsString(COMMENT(self)));
}

static VALUE html_start_is_self_closing(VALUE self)
{
    return RB_BOOL(clang_HTMLStartTagComment_isSelfClosing(COMMENT(self)));
}

static VALUE html_start_has_attr(VALUE self)
{
    return RB_BOOL(clang_HTMLStartTag_getNumAttrs(COMMENT(self)));
}

static VALUE html_start_get_attr(VALUE self, VALUE key)
{
    CXComment c = COMMENT(self);
    unsigned int n = clang_HTMLStartTag_getNumAttrs(c);
    if (n == 0)
        return Qnil;

    if (SYMBOL_P(key))
        key = rb_sym_to_s(key);

    for (unsigned i = 0; i < n; i++)
    {
        VALUE str = RUBYSTR(clang_HTMLStartTag_getAttrName(c, i));
        if (rb_str_equal(key, str))
            return RUBYSTR(clang_HTMLStartTag_getAttrValue(c, i));
    }

    return Qnil;
}

static VALUE html_start_attrs(int argc, VALUE *argv, VALUE self)
{
    VALUE symbolize;
    rb_scan_args(argc, argv, "01", &symbolize);

    CXComment c = COMMENT(self);
    unsigned int n = clang_HTMLStartTag_getNumAttrs(c);

    CXString name, value;
    VALUE hash = rb_hash_new();

    for (unsigned i = 0; i < n; i++)
    {
        VALUE key;
        name = clang_HTMLStartTag_getAttrName(c, i);
        value = clang_HTMLStartTag_getAttrValue(c, i);

        if (RTEST(symbolize))
        {
            const char *str = clang_getCString(name);
            key = ID2SYM(rb_intern(str));
            clang_disposeString(name);
        }
        else
        {
            key = RUBYSTR(name);
        }
        rb_hash_aset(hash, key, RUBYSTR(value));
    }
    return hash;
}

static VALUE html_start_each_attr(int argc, VALUE *argv, VALUE self)
{
    RETURN_ENUMERATOR(self, argc, argv);

    VALUE symbolize;
    rb_scan_args(argc, argv, "01", &symbolize);

    CXComment c = COMMENT(self);
    unsigned int n = clang_HTMLStartTag_getNumAttrs(c);

    CXString name, value;

    for (unsigned i = 0; i < n; i++)
    {
        VALUE key;
        name = clang_HTMLStartTag_getAttrName(c, i);
        value = clang_HTMLStartTag_getAttrValue(c, i);

        if (RTEST(symbolize))
        {
            const char *str = clang_getCString(name);
            key = ID2SYM(rb_intern(str));
            clang_disposeString(name);
        }
        else
        {
            key = RUBYSTR(name);
        }

        rb_yield(rb_ary_new_from_args(2, key, RUBYSTR(value)));
    }
    return self;
}

static VALUE block_command_name(VALUE self)
{
    return RUBYSTR(clang_BlockCommandComment_getCommandName(COMMENT(self)));
}

static VALUE block_command_arg_count(VALUE self)
{
    return UINT2NUM(clang_BlockCommandComment_getNumArgs(COMMENT(self)));
}

static VALUE block_command_args(VALUE self)
{
    CXComment comment = COMMENT(self);
    unsigned int n = clang_BlockCommandComment_getNumArgs(comment);
    VALUE ary = rb_ary_new_capa(n);

    for (unsigned i = 0; i < n; i++)
    {
        VALUE str = RUBYSTR(clang_BlockCommandComment_getArgText(comment, i));
        rb_ary_store(ary, i, str);
    }
    return ary;
}

static VALUE block_command_each_arg(VALUE self)
{
    RETURN_ENUMERATOR(self, 0, NULL);
    CXComment comment = COMMENT(self);
    unsigned int n = clang_BlockCommandComment_getNumArgs(comment);

    for (unsigned i = 0; i < n; i++)
    {
        VALUE str = RUBYSTR(clang_BlockCommandComment_getArgText(comment, i));
        rb_yield(str);
    }
    return self;
}

static VALUE block_command_paragraph(VALUE self)
{
    CXComment *para = ALLOC(CXComment);
    *para = clang_BlockCommandComment_getParagraph(COMMENT(self));
    return wrap_comment(para);
}

static VALUE block_command_text(VALUE self)
{
    VALUE para = block_command_paragraph(self);
    return rb_funcall(para, rb_intern("to_s"), 0);
}

static VALUE param_command_name(VALUE self)
{
    return RUBYSTR(clang_ParamCommandComment_getParamName(COMMENT(self)));
}

static VALUE param_command_index(VALUE self)
{
    return UINT2NUM(clang_ParamCommandComment_getParamIndex(COMMENT(self)));
}

static VALUE param_command_is_valid(VALUE self)
{
    return RB_BOOL(clang_ParamCommandComment_isParamIndexValid(COMMENT(self)));
}

static VALUE param_command_direction(VALUE self)
{
    CXComment c = COMMENT(self);
    if (!clang_ParamCommandComment_isDirectionExplicit(c))
        return STR2SYM("unspecified");

    enum CXCommentParamPassDirection kind = clang_ParamCommandComment_getDirection(c);
    switch (kind)
    {
        case CXCommentParamPassDirection_In: return STR2SYM("in");
        case CXCommentParamPassDirection_Out: return STR2SYM("out");
        case CXCommentParamPassDirection_InOut: return STR2SYM("in_out");
        default: return STR2SYM("unspecified");
    }
}

static VALUE param_command_text(VALUE self)
{
    VALUE space = rb_str_new_cstr(" ");
    return rb_ary_join(comment_children(self), space);
}

static VALUE tparam_command_name(VALUE self)
{
    return RUBYSTR(clang_TParamCommandComment_getParamName(COMMENT(self)));
}

static VALUE tparam_command_depth(VALUE self)
{
    return UINT2NUM(clang_TParamCommandComment_getDepth(COMMENT(self)));
}

static VALUE tparam_command_index(VALUE self, VALUE depth)
{
    return UINT2NUM(clang_TParamCommandComment_getIndex(COMMENT(self), NUM2INT(depth)));
}

static VALUE tparam_command_is_valid(VALUE self)
{
    return RB_BOOL(clang_TParamCommandComment_isParamPositionValid(COMMENT(self)));
}

static VALUE tparam_command_text(VALUE self)
{
    CXComment child = clang_Comment_getChild(COMMENT(self), 0);
    unsigned int kind = clang_Comment_getKind(child);
    if (kind == CXComment_Null)
        return rb_call_super(0, NULL);

    VALUE comment = Data_Wrap_Struct(comment_classes[kind], NULL, RUBY_NEVER_FREE, &child);
    return rb_funcall(comment, rb_intern("to_s"), 0);
}

static VALUE verbatim_block_line_text(VALUE self)
{
    return RUBYSTR(clang_VerbatimBlockLineComment_getText(COMMENT(self)));
}

static VALUE verbatim_line_text(VALUE self)
{
    return RUBYSTR(clang_VerbatimLineComment_getText(COMMENT(self)));
}

static VALUE full_comment_xml(VALUE self)
{
    return RUBYSTR(clang_FullComment_getAsXML(COMMENT(self)));
}

static VALUE full_comment_html(VALUE self)
{
    return RUBYSTR(clang_FullComment_getAsHTML(COMMENT(self)));
}

static VALUE full_comment_text(VALUE self)
{
    VALUE children = comment_children(self);
    
    ID to_s = rb_intern("to_s");
    ID strip = rb_intern("strip");
    VALUE newline = rb_str_new_cstr("\n");

    long len = rb_array_len(children);
    VALUE temp = rb_ary_new_capa(len);
    for (long i = 0; i < len; i++)
    {
        VALUE v = rb_funcall(rb_ary_entry(children, i), to_s, 0);
        rb_ary_store(temp, i, rb_funcall(v, strip, 1, newline));
    }

    return rb_ary_join(temp, newline);
}

static VALUE paragraph_text(VALUE self)
{
    VALUE newline = rb_str_new_cstr("\n");
    return rb_ary_join(comment_children(self), newline);
}

static VALUE comment_translation_unit(VALUE self)
{
    CXTranslationUnit *unit = ALLOC(CXTranslationUnit);
    *unit = COMMENT(self).TranslationUnit;
    return Data_Wrap_Struct(rb_cCXTranslationUnit, NULL, RUBY_NEVER_FREE, unit);
}

void Init_clang_comment(void)
{
    rb_define_method0(rb_cCXCursor, "comment", cursor_comment, 0);

    rb_cCXComment = rb_define_class_under(rb_mClang, "Comment", rb_cObject);
    rb_define_alloc_func(rb_cCXComment, comment_alloc);

    rb_include_module(rb_cCXComment, rb_mEnumerable);
    rb_define_method0(rb_cCXComment, "child_count", comment_child_count, 0);
    rb_define_method0(rb_cCXComment, "children", comment_children, 0);
    rb_define_method0(rb_cCXComment, "each", comment_each_child, 0);
    rb_define_method0(rb_cCXComment, "has_children?", comment_has_children, 0);
    rb_define_method0(rb_cCXComment, "whitespace?", comment_is_whitespace, 0);
    rb_define_method0(rb_cCXComment, "trailing_newline?", comment_trailing_newline, 0);
    rb_define_method0(rb_cCXComment, "text", comment_text, 0);
    rb_define_method0(rb_cCXComment, "to_s", comment_text, 0);
    rb_define_method0(rb_cCXComment, "to_str", comment_text, 0);
    rb_define_method0(rb_cCXComment, "translation_unit", comment_translation_unit, 0);
    rb_define_alias(rb_cCXComment, "each_child", "each");
    rb_define_alias(rb_cCXComment, "to_a", "children");

    COMMENT_SUBCLASS(cText, "Text");
    rb_define_method0(cText, "text",   text_text, 0);
    rb_define_method0(cText, "to_s",   text_text, 0);
    rb_define_method0(cText, "to_str", text_text, 0);

    COMMENT_SUBCLASS(cInlineCommand, "InlineCommand");
    render_kind = enum_allocate(rb_cEnum);
    enum_field(render_kind, "normal", CXCommentInlineCommandRenderKind_Normal);
    enum_field(render_kind, "bold", CXCommentInlineCommandRenderKind_Bold);
    enum_field(render_kind, "monospaced", CXCommentInlineCommandRenderKind_Monospaced);
    enum_field(render_kind, "emphasized", CXCommentInlineCommandRenderKind_Emphasized);
    enum_field(render_kind, "anchor", CXCommentInlineCommandRenderKind_Anchor);
    rb_define_const(cInlineCommand, "RenderKind", render_kind);
    rb_define_method0(cInlineCommand, "name", inline_command_name, 0);
    rb_define_method0(cInlineCommand, "render_kind", inline_command_render_kind, 0);
    rb_define_method0(cInlineCommand, "arg_count", inline_command_arg_count, 0);
    rb_define_method0(cInlineCommand, "args", inline_command_args, 0);
    rb_define_method0(cInlineCommand, "each_arg", inline_command_each_arg, 0);
    rb_define_method0(cInlineCommand, "text",   inline_command_text, 0);
    rb_define_method0(cInlineCommand, "to_s",   inline_command_text, 0);
    rb_define_method0(cInlineCommand, "to_str", inline_command_text, 0);

    COMMENT_SUBCLASS(VALUE html_comment, "HTMLTag");
    rb_define_method0(html_comment, "name",   html_tag_name, 0);
    rb_define_method0(html_comment, "text",   html_tag_text, 0);
    rb_define_method0(html_comment, "to_s",   html_tag_text, 0);
    rb_define_method0(html_comment, "to_str", html_tag_text, 0);

    cHTMLStartTag = rb_define_class_under(rb_cCXComment, "HTMLStartTag", html_comment);
    cHTMLEndTag = rb_define_class_under(rb_cCXComment, "HTMLEndTag", html_comment);
    rb_define_method0(cHTMLStartTag, "self_closing?", html_start_is_self_closing, 0);
    rb_define_method0(cHTMLStartTag, "has_attr?", html_start_has_attr, 0);
    rb_define_method1(cHTMLStartTag, "[]", html_start_get_attr, 1);
    rb_define_methodm1(cHTMLStartTag, "attrs", html_start_attrs, -1);
    rb_define_methodm1(cHTMLStartTag, "each_attr", html_start_each_attr, -1);
    rb_define_alias(cHTMLStartTag, "attributes", "attrs");
    rb_define_alias(cHTMLStartTag, "each_attribute", "each_attr");

    COMMENT_SUBCLASS(cBlockCommand, "BlockCommand");
    rb_define_method0(cBlockCommand, "name", block_command_name, 0);
    rb_define_method0(cBlockCommand, "arg_count", block_command_arg_count, 0);
    rb_define_method0(cBlockCommand, "args", block_command_args, 0);
    rb_define_method0(cBlockCommand, "each_arg", block_command_each_arg, 0);
    rb_define_method0(cBlockCommand, "paragraph", block_command_paragraph, 0);
    rb_define_method0(cBlockCommand, "text",   block_command_text, 0);
    rb_define_method0(cBlockCommand, "to_s",   block_command_text, 0);
    rb_define_method0(cBlockCommand, "to_str", block_command_text, 0);

    COMMENT_SUBCLASS(cParamCommand, "ParamCommand");
    rb_define_method0(cParamCommand, "name", param_command_name, 0);
    rb_define_method0(cParamCommand, "index", param_command_index, 0);
    rb_define_method0(cParamCommand, "valid?", param_command_is_valid, 0);
    rb_define_method0(cParamCommand, "direction", param_command_direction, 0);
    rb_define_method0(cParamCommand, "text",   param_command_text, 0);
    rb_define_method0(cParamCommand, "to_s",   param_command_text, 0);
    rb_define_method0(cParamCommand, "to_str", param_command_text, 0);

    COMMENT_SUBCLASS(cTParamCommand, "TParamCommand");
    rb_define_method0(cTParamCommand, "name", tparam_command_name, 0);
    rb_define_method0(cTParamCommand, "depth", tparam_command_depth, 0);
    rb_define_method1(cTParamCommand, "index", tparam_command_index, 1);
    rb_define_method0(cTParamCommand, "valid?", tparam_command_is_valid, 0);
    rb_define_method0(cTParamCommand, "text",   tparam_command_text, 0);
    rb_define_method0(cTParamCommand, "to_s",   tparam_command_text, 0);
    rb_define_method0(cTParamCommand, "to_str", tparam_command_text, 0);

    COMMENT_SUBCLASS(cVerbatimBlockLine, "VerbatimBlockLine");
    rb_define_method0(cVerbatimBlockLine, "text",   verbatim_block_line_text, 0);
    rb_define_method0(cVerbatimBlockLine, "to_s",   verbatim_block_line_text, 0);
    rb_define_method0(cVerbatimBlockLine, "to_str", verbatim_block_line_text, 0);

    COMMENT_SUBCLASS(cVerbatimLine, "VerbatimLine");
    rb_define_method0(cVerbatimLine, "text",   verbatim_line_text, 0);
    rb_define_method0(cVerbatimLine, "to_s",   verbatim_line_text, 0);
    rb_define_method0(cVerbatimLine, "to_str", verbatim_line_text, 0);

    COMMENT_SUBCLASS(cFullComment, "FullComment");
    rb_define_method0(cFullComment, "xml", full_comment_xml, 0);
    rb_define_method0(cFullComment, "html", full_comment_html, 0);
    rb_define_method0(cFullComment, "text",   full_comment_text, 0);
    rb_define_method0(cFullComment, "to_s",   full_comment_text, 0);
    rb_define_method0(cFullComment, "to_str", full_comment_text, 0);

    COMMENT_SUBCLASS(cParagraph, "Paragraph");
    rb_define_method0(cParagraph, "text",   paragraph_text, 0);
    rb_define_method0(cParagraph, "to_s",   paragraph_text, 0);
    rb_define_method0(cParagraph, "to_str", paragraph_text, 0);

    COMMENT_SUBCLASS(cVerbatimBlockCommand, "VerbatimBlockCommand");
    rb_define_method0(cVerbatimBlockCommand, "text",   paragraph_text, 0);
    rb_define_method0(cVerbatimBlockCommand, "to_s",   paragraph_text, 0);
    rb_define_method0(cVerbatimBlockCommand, "to_str", paragraph_text, 0);


}


