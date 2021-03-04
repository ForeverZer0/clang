module Clang
  # TODO
  ##
  # @abstract
  class Comment

    include Enumerable

    ##
    # Retrieve the number of child {Comment} objects within this comment.
    # @return [Integer] the number of child comments.
    def child_count
    end

    ##
    # Retrieve an array of child comments within this comment.
    # @return [Array<Comment>] an array of child {Comment} instances.
    def children
    end

    ##
    # Enumerates through the child comments of the this comment.
    #
    # @overload each(&block)
    #   When called with a block, yields each child comment to the block before returning `self`.
    #   @yieldparam child [Comment] Yields a child comment to the block.
    #   @return [self]
    #
    # @overload each
    #   When called without a block, returns an {Enumerator} for the child comments.
    #   @return [Enumerator] an enumerator for the child comments.
    def each
    end

    ##
    # @return [Boolean] flag indicating if this {Comment} has child comments.
    def has_children?
    end

    ##
    # 
    def whitespace?
    end

    def trailing_newline?
    end

    def text
    end

    ##
    # Retrieves the {TranslationUnit} the comment is a child of.
    # @return [TranslationUnit] the parent translation unit.
    def translation_unit
    end

    alias_method :to_s, :text
    alias_method :to_str, :text
    alias_method :to_a, :children
    alias_method :each_child, :each

    class Text < Comment
    end

    class InlineCommand < Comment

      # rb_define_const(cInlineCommand, "RenderKind", render_kind);
      # render_kind = enum_allocate(rb_cEnum);
      # enum_field(render_kind, "normal", CXCommentInlineCommandRenderKind_Normal);
      # enum_field(render_kind, "bold", CXCommentInlineCommandRenderKind_Bold);
      # enum_field(render_kind, "monospaced", CXCommentInlineCommandRenderKind_Monospaced);
      # enum_field(render_kind, "emphasized", CXCommentInlineCommandRenderKind_Emphasized);
      # enum_field(render_kind, "anchor", CXCommentInlineCommandRenderKind_Anchor);

      def name
      end

      def render_kind
      end

      def arg_count
      end

      def args
      end

      def each_arg
      end
    end

    ##
    # @abstract
    class HTMLTag < Comment

      def name
      end
    end

    class HTMLStartTag < HTMLTag

      def self_closing?
      end

      def has_attr?
      end

      def [](name)
      end

      def attrs
      end

      def each_attr
      end

      alias_method :attributes, :attrs
      alias_method :each_attribute, :each_attr
    end

    class HTMLEndTag < HTMLTag
    end

    class BlockCommand < Comment

      def name
      end

      def arg_count
      end

      def args
      end

      def each_arg
      end

      def paragraph
      end
    end

    class ParamCommand < Comment

      def name
      end

      def index
      end

      def valid?
      end

      def direction
      end
    end

    class TParamCommand < Comment

      def name
      end

      def depth
      end

      def index(depth)
      end

      def valid?
      end
    end

    class VerbatimBlockLine < Comment
    end

    class VerbatimLine < Comment
    end

    class FullComment < Comment

      def xml
      end

      def html
      end
    end

    class Paragraph < Comment
    end

    class VerbatimBlockCommand < Comment
    end
  end
end