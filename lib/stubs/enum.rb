
module Clang

  ##
  # Represents a native C-style enumeration.
  #
  # There is little need to ever use this class directly, as it merely provides an interface to an internal
  # mapping of Symbol objects to their integral values. User need only pass in raw `Symbol` objects where they
  # are needed, or an array of symbols in the case of a bitwise set of flags, but this class remains as a
  # reference and for introspection wherever needed.
  class Enum < BasicObject

    include Enumerable

    ##
    # Iterates over the members of the enumeration.
    #
    # @overload each(&block)
    #   When called with a block, yields each name/value pair to the block and then returns `self`.
    #   @yieldparam name [Symbol] The name of the member.
    #   @yieldparam value [Integer] The integral value of the member.
    #   @return [self]
    #
    # @overload each
    #   When called without a block, returns an Enumerator object for the {Enum}.
    #   @return [Enumerator] An Enumerator object.
    def each(&block)
    end

    ##
    # @return [Integer] the number of members in the enumeration.
    def size
    end

    ##
    # @return [Array<Symbol>] the names of the members in the enumeration.
    def names
    end

    ##
    # @return [Array<Integer>] the values of the members in the enumeration.
    def fields
    end

    ##
    # Retrieves the integral value of one or more enum values bitwise OR'ed together.
    #
    # @param symbol [Symbol] A symbol name of a field.
    # @param symbols [Symbol] Additional names to bitwise OR together. 
    # @return [Integer] The integral value of the specified name(s). Returns `0` if the symbol(s) were not found.
    def mask(symbol, *symbols)
    end

    ##
    # Retrieves the individual names of a bitfield values OR'ed toegther.
    #
    # @param integer [Integer] A set of flags OR'ed toegether. 
    # @return [Array<Symbol>] The array of Symbol objects containing the names of the set flags.
    def unmask(integer)
    end

    ##
    # Retrieves the name of the specified value in the enumeration.
    # @param integer [Integer] The value of the enumeration to query the name of.
    # @return [Symbol?] The name of the field as a Symbol, or `nil` if the value was not found.
    # @note For bitfields containing multiple values OR'ed together, use {#unmask}.
    def symbol(integer)
    end

    ##
    # Retrieves the integral value of the specified name.
    # @param name [Symbol,String] the name of the member to retrieve.
    # @return [Integer] The integral value of the member.
    def value(name)
    end

    ##
    # Attempts to map the method name to the name of member in the enumeration, returning its integral value if successful.
    def method_missing
    end

    ##
    # @return [Hash{Symbol => Integer}] the Hash representation of the {Enum}.
    def to_h
    end

    ##
    # @return [String] the String representation of the {Enum}.
    def to_s
    end

    ##
    # @return [String] the String representation of the {Enum}.
    def inspect
    end

    alias_method :length, :size
    alias_method :[], :symbol
    alias_method :to_hash, :to_h
    alias_method :to_str, :to_s

  end
end