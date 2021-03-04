module Clang
  ##
  # A group of {Diagnostic} objects.
  class DiagnosticSet

    include Enumerable

    ##
    # Deserialize a set of diagnostics from a Clang diagnostics bitcode file.
    #
    # @param path [String] The name of the file to deserialize.
    #
    # @return [DiagnosticSet] The loaded {DiagnosticSet},
    def self.load(path)
    end

    ##
    # Retrieves the child {Diagnostic} at the specified index.
    #
    # @param index [Integer] The zero-based index of the diagnostic to retrieve.
    #
    # @return [Diagnostic?] The requested {Diagnostic} instance, or `nil` if index was out of range.
    def [](index)
    end

    ##
    # @return [Integer] the number of child {Diagnostic} objects within the set.
    def size
    end

    ##
    # Enumerates through each child diagnostic in the set.
    #
    # @overload each(&block)
    #   When called with a block, yields each child to the block before returning `self`.
    #   @yieldparam diagnostic [Diagnostic] The child diagnostic object.
    #   @return [self]
    #
    # @overload each
    #   When called without a block, returns an Enumerator for the set.
    #   @return [Enumerator] The set enumerator.
    def each
    end

    alias_method :length, :size
  end

  ##
  # A single diagnostic, containing the information about severity, location, text, source ranges, and fix-it hints.
  class Diagnostic

    ##
    # Retrieve the set of display options most similar to the default behavior of the clang compiler.
    # @return [Array<Symbol>] A set of display options suitable for use with {#format}.
    # @see DiagnosticDisplayOptions
    def self.default_format
    end

    ##
    # Determine the number of source ranges associated with the diagnostic.
    # @return [Integer] the number of ranges within the diagnostic.
    def range_count
    end

    ##
    # Determine the number of fix-it hints associated with the diagnostic.
    # @return [Integer] the number of fixit hints within the diagnostic.
    def fixit_count
    end

    ##
    # Retrieve the category number for this diagnostic.
    #
    # Diagnostics can be categorized into groups along with other, related
    # diagnostics (e.g., diagnostics under the same warning flag).
    #
    # @return [Integer] the number of the category that contains this diagnostic, or `0` if uncategorized.
    def category
    end

    ##
    # Retrieve the source location of the given diagnostic.
    #
    # This location is where Clang would print the caret ('^') when displaying the diagnostic on the command line.
    # @return [SourceLocation]
    def location
    end

    ##
    # Retrieve the text of the given diagnostic.
    # @return [String] the diagnostic text.
    def spelling
    end

    ##
    # Retrieves the severity of the given diagnostic.
    # @return [Symbol] teh diagnostic severity.
    # @see DiagnosticSeverity
    def severity
    end

    ##
    # Format the given diagnostic in a manner that is suitable for display.
    #
    # This routine will format the given diagnostic to a string, rendering  the diagnostic according to the various
    # options given. The  #{default_format} function returns the set of  options that most closely mimics the behavior
    # of the clang compiler.
    #
    # @param options [Symbol,Array<Symbol>] A set of options that control the diagnostic display.
    # @return [void]
    # @see DiagnosticDisplayOptions
    def format(*options)
    end

    ##
    # Retrieve the child diagnostics (if any).
    # @return [DiagnosticSet?] a child set of diagnostics, or `nil` if none exist.
    def children
    end

    ##
    # Retrieve the name of the command-line option that enabled this diagnostic.
    #
    # @param disable [Boolean] When 'true, will return the command-line option that disables this diagnostic (if any).
    # @return [String] A string that contains the command-line option used to enable this warning, such
    #   as `-Wconversion`  or `-pedantic`.
    def option_str(disable = false)
    end

    ##
    # Retrieve the diagnostic category text for this diagnostic.
    # @return [String] the text for the diagnostic category.
    def category_name
    end

    ##
    # Retrieves the source ranges associated with the diagnostic.
    #
    # A diagnostic's source ranges highlight important elements in the source code. On the command line,
    # Clang displays source ranges by underlining them with '~' characters.
    #
    # @return [Array<SourceRange>] the set of source ranges.
    # @see each_range
    def ranges
    end

    ##
    # Enumerates through the source ranges associated with the diagnostic.
    #
    # A diagnostic's source ranges highlight important elements in the source code. On the command line,
    # Clang displays source ranges by underlining them with '~' characters.
    #
    # @overload each_range(&block)
    #   When called with a block, yields each range object to the block before returning `self`.
    #   @yieldparam range [SourceRange] Yields a {SourceRange} object to the block.
    #   @return [self]
    # @overload each_range
    #   When called without a block, returns an Enumerator object for iterating the ranges.
    #   @return [Enumerator] An Enumerator for iterating the ranges.
    # @see ranges
    def each_range
    end

    ##
    # Retrieve the replacement information for a given fix-it.
    #
    # Fix-its are described in terms of a source range whose contents
    # should be replaced by a string. This approach generalizes over
    # three kinds of operations: removal of source code (the range covers
    # the code to be removed and the replacement string is empty),
    # replacement of source code (the range covers the code to be
    # replaced and the replacement string provides the new code), and
    # insertion (both the start and end of the range point at the
    # insertion location, and the replacement string provides the text to
    # insert).
    #
    # @return [Array<Array(String,SourceRange)>] an array of fixit information.
    # @see each_fixit
    def fixits
    end

    ##
    # Retrieve the replacement information for a given fix-it.
    #
    # Fix-its are described in terms of a source range whose contents
    # should be replaced by a string. This approach generalizes over
    # three kinds of operations: removal of source code (the range covers
    # the code to be removed and the replacement string is empty),
    # replacement of source code (the range covers the code to be
    # replaced and the replacement string provides the new code), and
    # insertion (both the start and end of the range point at the
    # insertion location, and the replacement string provides the text to
    # insert).
    #
    # @overload each_fixit(&block)
    #   When called with a block, yields the information for each fixit to the block before returning `self`.
    #   @yieldparam text [String] A string containing text that should be replace the source code indicated by the the range.
    #   @yieldparam range [SourceRange] The source range whose contents will be replaced with the returned replacement string.
    #   @return [self]
    #
    # @overload each_fixit
    #   When called without a block, returns an Enumerator object for the fixit information.
    #   @return [Enumerator] An enumerator for iterating over the fixit information.
    #
    # @note Note that source ranges are half-open ranges [a, b), so the source code should be replaced from a and up
    #   to (but not including) b.
    # @see fixits
    def each_fixit
    end

    ##
    # @return [String] the String representation of the {Diagnostic}.
    def to_s
    end

    alias_method :to_str, :to_s
  end
end