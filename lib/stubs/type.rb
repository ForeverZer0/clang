module Clang
  ##
  # The type of an element in the abstract syntax tree.
  class Type
    ##
    # Retrieve the return type associated with a function type.
    # @return [Type?] the result type, or `nil` if a non-function type is passed in
    def result_type
    end

    ##
    # Pretty-print the underlying type using the rules of the language of the translation unit from which it came.
    # @return [Type] the pretty-printed name of the type, or an empty string if type is invalid.
    def spelling
    end

    ##
    # Retrieve the spelling of the cursor kind.
    # @return [String] a string representing the kind of this cursor.
    def kind_spelling
    end

    ##
    # Determines if this instance is equal to another. If the other object is another {Type} instance, determines
    # if both refer to the same type.
    #
    # @param other [Object] The object to compare for equality.
    # @return [Boolean] `true` of objects are equal, otherwise `false`.
    def ==(other)
    end

    ##
    # @return [String] the String representation of the {Type}.
    # @see display_name
    def to_s
    end

    alias_method :to_str, :to_s
    alias_method :eql?, :==

    ##
    # Return the canonical type for the type.
    #
    # Clang's type system explicitly models typedefs and all the ways
    # a specific type can be represented.  The canonical type is the underlying
    # type with all the "sugar" removed.  For example, if 'T' is a typedef
    # for 'int', the canonical type for 'T' would be 'int'.
    #
    # @return [Type] the canonical type.
    def canonical
    end

    ##
    # Determine whether this type has the "const" qualifier set, without looking through typedefs that may have added
    # "const" at a different level.
    # @return [Boolean] `true` if type is "const" qualified, otherwise `false`.
    def const?
    end

    ##
    # Determine whether this type has the "volatile" qualifier set, without looking through typedefs that may have
    # added "volatile" at a different level.
    # @return [Boolean] `true` if type is "volatile" qualified, otherwise `false`.
    def volatile?
    end

    ##
    # Determine whether this type has the "restrict" qualifier set, without looking through typedefs that may have added
    # "restrict" at a different level.
    # @return [Boolean] `true` if type is "restrict" qualified, otherwise `false`.
    def restrict?
    end

    ##
    # Returns the address space of the given type.
    # @return [Integer] the address space.
    def address_space
    end

    ##
    # Returns the typedef name of the given type.
    # @return [String] the typedef name, or an empty string if type is invalid.
    def typedef_name
    end

    ##
    # For pointer types, returns the type of the pointee.
    # @return [Type?] the pointee type, or `nil` if type is not a pointer type.
    def pointee_type
    end

    ##
    # Return the cursor for the declaration of the given type.
    # @return [Cursor] a cursor for the type declaration.
    def declaration
    end

    ##
    # Retrieve the calling convention associated with a function type.
    # @return [Symbol] a Symbol object describing the calling convention, or `:invalid` if type is not a function.
    # @see CallingConv
    def calling_conv
    end

    ##
    # Retrieve the exception specification type associated with a function type.
    # @return [Symbol] a Symbol object describing the exception kind, or `:invalid` it type is not a function.
    # @see CursorExceptionSpecificationKind
    def exception_type
    end

    ##
    # Retrieve the number of non-variadic arguments associated with the type.
    # @return [Integer] the number of arguments, or `-1` if type does not represent a function/method declaration.
    def arg_count
    end

    ##
    # Retrieve the argument types of a function or method.
    # @return [Array<Type>] an array of {Type} objects for each argument, or an empty array if type does not
    #   not represent a function/method declaration.
    def arg_types
    end

    ##
    # Determines if the type is a variadic function type.
    # @return [Boolean] `true` if type is a variadic function, otherwise `false`.
    def variadic?
    end

    ##
    # Determines if this type is comprised of only POD (plain old data) types.
    # @return [Boolean] `true` if this is a POD type, otherwise `false`.
    def pod_type?
    end

    alias_method :plain_data?, :pod_type?

    ##
    # Return the element type of an array, complex, or vector type.
    # @return [Type?] the element type, or `nil` if this type is not an array, complex, or vector type.
    def element_type
    end

    ##
    # Return the element type of an array type.
    # @return [Type?] the array element type, or `nil` if this type is not an array.
    def array_type
    end

    ##
    # Return the number of elements of an array or vector type.
    # @return [Integer] the number of elements, or `-1` if this type is not an array, complex, or vector type.
    def element_count
    end

    ##
    # Return the array size of a constant array.
    # @return [Integer] the array size, or `-1` if this type is not an array.
    def array_size
    end

    ##
    # Determine if a typedef is 'transparent' tag. A typedef is considered 'transparent' if it shares a name and
    # spelling location with its underlying tag type, as is the case with the `NS_ENUM` macro.
    #
    # @return [Boolean] `true` if type is transparent, otherwise `false`.
    def transparent?
    end

    ##
    # Retrieve the nullability kind of a pointer type.
    # @return [Symbol] a Symbol object describing the nullability of the type.
    # @see TypeNullabilityKind
    def nullability
    end

    ##
    # Return the alignment of a type in bytes as per C++[expr.alignof] standard.
    # @return [Integer] the alignment of the type, or `-1` if undetermined or invalid.
    def alignof
    end

    ##
    # Return the size of a type in bytes as per C++[expr.sizeof] standard.
    # @return [Integer] the size of the type, or `-1` if undetermined or invalid.
    def sizeof
    end

    ##
    # Return the offset of a field in this record type in bits as it would be returned by `__offsetof__` as
    # per C++11[18.2p4]
    #
    # @param field [String] The name of a field in the structure this type represents.
    # @return [Integer] the offset of the field, or `-1` if undetermined or invalid.
    def field_offset(field)
    end

    ##
    # Retrieve the type named by the qualified-id.
    # @return [Type?] the named type, or `nil` if this is a non-elaborated type.
    def named_type
    end

    ##
    # Return the type that was modified by this attributed type.
    # @return [Type?] the modified type, or `nil` if this is not an attributed type
    def modified_type
    end

    ##
    # Gets the type contained by this atomic type.
    # @return [Type?] the value type, or `nil` if this is a non-atomic type.
    def value_type
    end

    ##
    # Return the class type of an member pointer type.
    # @return [Type?] the class type, or `nil` if this type is not a non-member-pointer type.
    def class_type
    end

    ##
    # Returns the Objective-C type encoding for the specified declaration.
    # @return [String] The Objective-C encoding.
    def obj_c_encoding
    end

    ##
    # Retrieves the base type of an Objective-C object type.
    # @return [Type?] the base type, or `nil` if type is not an Objective-C object.
    def obj_c_base_type
    end

    ##
    # Enumerates through each Objective-C argument type.
    #
    # @overload each_obj_c_arg(&block)
    #   When called with a block, yields each type to the block before returning self.
    #   @yieldparam arg [Type] Yields a type to the block.
    #   @return [self]
    #
    # @overload each_obj_c_arg
    #   When called without a block, returns an enumerable object for the method.
    #   @return [Enumerator] an Enumerator for the Objective-C argument types.
    #
    def each_obj_c_arg
    end

    ##
    # Enumerates through each Objective-C protocol reference associated with an Objective-C object/id.
    #
    # @overload each_obj_c_protocol(&block)
    #   When called with a block, yields each protocol reference to the block before returning self.
    #   @yieldparam protocol [Cursor] Yields a cursor to the block.
    #   @return [self]
    #
    # @overload each_obj_c_protocol
    #   When called without a block, returns an enumerable object for the method.
    #   @return [Enumerator] an Enumerator for the Objective-C protocols.
    def each_obj_c_protocol
    end

    ##
    # Retrieve the ref-qualifier kind of a function or method.
    # @return [Symbol] the qualifier kind, or `:none` if type is not a C++ declaration.
    # @see RefQualifierKind
    def ref_qualifier
    end
  end
end