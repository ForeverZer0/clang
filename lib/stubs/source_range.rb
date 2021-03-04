module Clang
  ##
  # Identifies a half-open character range in the source code.
  class SourceRange
    ##
    # Retrieve a `NULL` (invalid) {SourceRange} instance.
    # @return [SourceRange] The new instance.
    def self.null
    end

    ##
    # @return [SourceLocation] a source location representing the first character within a source range.
    attr_reader :begin

    ##
    # @return [SourceLocation] a source location representing the last character within a source range.
    attr_reader :end

    ##
    # Creates a new instance of the {SourceRange} class.
    #
    # @param start_location [SourceLocation] A source location representing the first character within the range.
    # @param end_location [SourceLocation] A source location representing the last character within the range.
    def initialize(start_location, end_location)
    end

    ##
    # Determines if this instance refers to a null/invalid pair of locations.
    # @return [Boolean] `true` if this instance refers to a `NULL` location, otherwise `false`.
    def null?
    end

    ##
    # Determines if this instance is equal to another. If the other object is another {SourceRange} instance,
    # determines if both refer to the same locations.
    #
    # @param other [Object] The object to compare for equality.
    # @return [Boolean] `true` of objects are equal, otherwise `false`.
    def ==(other)
    end

    alias_method :eql?, :==
  end
end
