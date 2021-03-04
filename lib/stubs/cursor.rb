module Clang

  class PlatformAvailability

    attr_reader :platform
    attr_reader :introduced
    attr_reader :deprecated
    attr_reader :obsoleted
    attr_reader :message

    def unavailable?
    end
  end

  ##
  # A cursor representing some element in the abstract syntax tree for a translation unit.
  #
  # The cursor abstraction unifies the different kinds of entities in a
  # program--declaration, statements, expressions, references to declarations,
  # etc.--under a single "cursor" abstraction with a common set of operations.
  # Common operation for a cursor include: getting the physical location in
  # a source file where the cursor points, getting the name associated with a
  # cursor, and retrieving cursors for any child nodes of a particular cursor.
  #
  # @see CursorKind
  class Cursor

    ##
    # Retrieve the `NULL` cursor, which represents no entity.
    # @return [Cursor] the `NULL` cursor instance.
    def self.null
    end

    ##
    # Retrieves a flag indicating if this instance is a `NULL` (invalid) value.
    # @return [Boolean] `true` if cursor is `NULL`, otherwise `false`.
    def null?
    end

    ##
    # Map a source location to the cursor that describes the entity at that location in the source code.
    #
    # Maps an arbitrary source location within a translation unit down to the most specific cursor that describes the
    # entity at that  location. For example, given an expression `x + y`, initializing a cursor with a source location
    # pointing to "x" will return the cursor for "x"; similarly for "y". If the cursor points anywhere between  "x" or
    # "y" (e.g., on the + or the whitespace around it),  will return a cursor referring to the "+" expression.
    #
    # @param translation_unit [TranslationUnit] The parent translation unit.
    # @param location [SourceLocation] A source location where the cursor will be created from.
    def initialize(translation_unit, location)
    end

    ##
    # @return [Integer] a unique hash value, making it suitable for using a {Cursor} as a key in a Hash.
    def hash
    end

    ##
    # Determines if this instance is equal to another. If the other object is another {Cursor} instance, determines
    # if both refer to the same location/entity.
    #
    # @param other [Object] The object to compare for equality.
    # @return [Boolean] `true` of objects are equal, otherwise `false`.
    def ==(other)
    end

    ##
    # @return [String] the String representation of the {Cursor}.
    # @see display_name
    def to_s
    end
    
    alias_method :to_str, :to_s
    alias_method :eql?, :==

    ##
    # Retrieve the kind of the given cursor.
    # @return [Symbol] a Symbol object describing the basic kind of the cursor.
    # @see CursorKind
    def kind
    end

    ##
    # Determine whether the given cursor kind represents a declaration.
    #
    # @param ensure_valid [Boolean] When `true`, will additionally confirm that the declaration is valid and can be
    #   parsed correctly.
    #
    # @return [Boolean] `true` if cursor represents a declaration, otherwise `false`.
    def declaration?(ensure_valid = false)
    end

    ##
    # Determine whether the given cursor kind represents a simple reference.
    #
    # @note Note that other kinds of cursors (such as expressions) can also refer to other cursors.
    # @return [Boolean] `true` if cursor represents a reference, otherwise `false`.
    def reference?
    end

    ##
    # Determine whether the given cursor kind represents an expression.
    # @return [Boolean] `true` if cursor represents an expression, otherwise `false`.
    def expression?
    end

    ##
    # Determine whether the given cursor kind represents a statement.
    # @return [Boolean] `true` if cursor represents a statement, otherwise `false`.
    def statement?
    end

    ##
    # Determine whether the given cursor kind represents an attribute.
    # @return [Boolean] `true` if cursor represents a statement, otherwise `false`.
    def attribute?
    end

    ##
    # Determine whether the given cursor has any attributes.
    # @return [Boolean] `true` if cursor has attributes, otherwise `false`.
    def has_attr?
    end

    ##
    # Determine whether the given cursor kind represents an invalid cursor.
    # @return [Boolean] `true` if cursor is invalid, otherwise `false`.
    def invalid?
    end

    ##
    # Determine whether the given cursor kind represents a translation unit.
    # @return [Boolean] `true` if cursor represents a translation unit, otherwise `false`.
    def translation_unit?
    end

    ##
    # Determine whether the given cursor represents a preprocessing element, such as a preprocessor directive or macro
    # instantiation.
    # @return [Boolean] `true` if cursor represents a preprocessing element, otherwise `false`.
    def preprocessing?
    end

    ##
    # Determine whether the given cursor represents a currently unexposed piece of the AST.
    # @return [Boolean] `true` if cursor represents an unexposed entity, otherwise `false`.
    def unexposed?
    end

    ##
    # Determine the linkage of the entity referred to by the cursor.
    # @return [Symbol] a Symbol object describing the type linkage.
    # @see LinkageKind
    def linkage
    end

    ##
    # Describe the visibility of the entity referred to by a cursor.
    #
    # This returns the default visibility if not explicitly specified by a visibility attribute. The default visibility
    # may be changed by commandline arguments.
    #
    # @return [Symbol] a Symbol object describing the cursor visibility.
    # @see VisibilityKind
    def visibility
    end

    ##
    # Determine the availability of the entity that this cursor refers to on any platforms for which availability
    # information is known.
    #
    # The data is returned as a Hash object that can can contain the following data.
    #
    # Key | Value
    # --- | ---
    # `:deprecated` | Either `true` or `false` to indicate whether the entity is deprecated on all platforms.
    # `:deprecated_msg` | A String message text provided along with the unconditional deprecation of this entity.
    # `:unavailable` | Either `true` or `false` to indicate whether the entity is unavailable on all platforms.
    # `:unavailable_msg` | A String message text provided along with the unconditional unavailability of this entity.
    # `:availability` | An array of {PlatformAvailability} objects that will be populated with platform availability information.
    #
    # @return [Hash{Symbol => Object}]
    def platform_availability
    end

    ##
    # Determine the "language" of the entity referred to by a given cursor.
    # @return [Symbol] a Symbol object describing the language.
    # @see LanguageKind
    def language
    end

    ##
    # Determine the "thread-local storage (TLS) kind" of the declaration referred to by a cursor.
    # @return [Symbol] a Symbol describing the thread-local storage.
    # @see TLSKind
    def tls_kind
    end

    ##
    # Returns the translation unit that a cursor originated from.
    # @return [TranslationUnit?] the parent translation unit this cursor is found within, or `nil` if cursor is invalid.
    def translation_unit
    end

    ##
    # Determine the semantic parent of the given cursor.
    #
    # The semantic parent of a cursor is the cursor that semantically contains the cursor. For many declarations, the
    # lexical and semantic parents are equivalent (the lexical parent is returned by {#lexical_parent}). They diverge
    # when declarations or definitions are provided out-of-line.
    #
    # @example
    #   class C {
    #    void f();
    #   };
    #
    #   void C::f() { }
    #
    # In the given example, out-of-line definition of `C::f`, the semantic parent is
    # the class `C`, of which this function is a member. The lexical parent is
    # the place where the declaration actually occurs in the source code; in this
    # case, the definition occurs in the translation unit. In general, the
    # lexical parent for a given entity can change without affecting the semantics
    # of the program, and the lexical parent of different declarations of the
    # same entity may be different. Changing the semantic parent of a declaration,
    # on the other hand, can have a major impact on semantics, and redeclarations
    # of a particular entity should all have the same semantic context.
    #
    # In the given example, both declarations of `C::f` have `C` as their
    # semantic context, while the lexical context of the first `C::f` is `C`
    # and the lexical context of the second `C::f` is the translation unit.
    #
    # For global declarations, the semantic parent is the translation unit.
    #
    # @return [Cursor] a cursor instance representing the semantic parent.
    # @see lexical_parent
    def semantic_parent
    end

    ##
    # Determine the lexical parent of the given cursor.
    #
    # The lexical parent of a cursor is the cursor in which the given \p cursor
    # was actually written. For many declarations, the lexical and semantic parents
    # are equivalent (the semantic parent is returned by {#semantic_parent}).
    # They diverge when declarations or definitions are provided out-of-line.
    #
    # @example
    #   class C {
    #    void f();
    #   };
    #
    #   void C::f() { }
    #
    # In the given example, the out-of-line definition of `C::f`, the semantic parent is
    # the class `C`, of which this function is a member. The lexical parent is
    # the place where the declaration actually occurs in the source code; in this
    # case, the definition occurs in the translation unit. In general, the
    # lexical parent for a given entity can change without affecting the semantics
    # of the program, and the lexical parent of different declarations of the
    # same entity may be different. Changing the semantic parent of a declaration,
    # on the other hand, can have a major impact on semantics, and redeclarations
    # of a particular entity should all have the same semantic context.
    #
    # In the given example, both declarations of `C::f` have `C` as their
    # semantic context, while the lexical context of the first `C::f` is `C`
    # and the lexical context of the second `C::f` is the translation unit.
    #
    # For declarations written in the global scope, the lexical parent is
    # the translation unit.
    #
    # @return [Cursor] a cursor instance representing the lexical parent.
    # @see semantic_parent
    def lexical_parent
    end

    ##
    # Determine the set of methods that are overridden by the given
    # method.
    #
    # In both Objective-C and C++, a method (aka virtual member function,
    # in C++) can override a virtual method in a base class. For
    # Objective-C, a method is said to override any method in the class's
    # base class, its protocols, or its categories' protocols, that has the same
    # selector and is of the same kind (class or instance).
    # If no such method exists, the search continues to the class's superclass,
    # its protocols, and its categories, and so on. A method from an Objective-C
    # implementation is considered to override the same methods as its
    # corresponding method in the interface.
    #
    # For C++, a virtual member function overrides any virtual member
    # function with the same signature that occurs in its base
    # classes. With multiple inheritance, a virtual member function can
    # override several virtual member functions coming from different
    # base classes.
    #
    # In all cases, this function determines the immediate overridden
    # method, rather than all of the overridden methods. For example, if
    # a method is originally declared in a class A, then overridden in B
    # (which in inherits from A) and also in C (which inherited from B),
    # then the only overridden method returned from this function when
    # invoked on C's method will be B's method. The client may then
    # invoke this function again, given the previously-found overridden
    # methods, to map out the complete method-override set.
    #
    # @return [Array<Cursor>] an array of {Cursor} objects, representing the set of overridden methods.
    def overrides
    end

    ##
    # Retrieve the file that is included by the given inclusion directive cursor.
    # @return [File?] the {File} instance, or `nil` if cursor is not a valid inclusion directive.
    def included_file
    end

    ##
    # Retrieve the physical location of the source constructor referenced by the given cursor.
    #
    # The location of a declaration is typically the location of the name of that
    # declaration, where the name of that declaration would occur if it is
    # unnamed, or some keyword that introduces that particular declaration.
    # The location of a reference is where that reference occurs within the
    # source code.
    #
    # @return [SourceLocation] the location of the cursor.
    def location
    end

    ##
    # Retrieve the physical extent of the source construct referenced by
    # the given cursor.
    #
    # The extent of a cursor starts with the file/line/column pointing at the
    # first character within the source construct that the cursor refers to and
    # ends with the last character within that source construct. For a
    # declaration, the extent covers the declaration itself. For a reference,
    # the extent covers the location of the reference (e.g., where the referenced
    # entity was actually used).
    #
    # @return [SourceRange] the source range of the cursor entity.
    def extent
    end

    ##
    # Retrieve a name for the entity referenced by this cursor.
    # @return [String] the name of the cursor entity.
    def spelling
    end

    ##
    # Pretty print declarations.
    # @param policy [PrintingPolicy] The policy to control the entities being printed. If `nil`, a default policy is used.
    # @return [String] The pretty printed declaration or an empty string for other cursors.
    def pretty_printed(policy) # TODO: Get rid of policy class, use hash and keyword args
    end

    # @!group Types

    ##
    # Retrieve the type of a {Cursor} (if any).
    # @return [Type?] the type of entity of the cursor, or `nil` if none exists.
    def type
    end

    ##
    # Retrieve the return type associated with a given cursor.
    # @return [Type?] the result type, or `nil` if a non-function cursor is passed in
    def result_type
    end

    ##
    # Retrieve the underlying type of a typedef declaration.
    # @return [Type?] the underlying type, or `nil` if cursor does not represent a typedef declaration.
    def underlying_type
    end

    ##
    # Retrieve the integer type of an enum declaration.
    # @return [Type?] the enum integer type, or `nil` if cursor does not represent an enum declaration.
    def enum_int_type
    end

    ##
    # Retrieve the type of the receiver for an Objective-C message or property reference, or C++ method call.
    # @return [Type?] the receiver type, or `nil` if cursor does not represent a C++ method or Objective-C message/property.
    def receiver_type
    end

    ##
    # For cursors representing an `iboutletcollection` attribute, this function returns the collection element type.
    # @return [Type?] the collection element type, or `nil` if the cursor entity does not contain such an attribute.
    def ib_outlet_type
    end

    # @!endgroup

    # @!group Documentation

    ##
    # For a cursor that represents a declaration, return the associated comment text, including comment markers.
    # @return [String] the comment text, or an empty string if no comment is present.
    def raw_comment
    end

    ##
    # For a cursor that represents a documentable entity (e.g. declaration), return the associated `@brief` section,
    # otherwise return the first paragraph.
    # @return [String] the comment text, or an empty string if no comment is present.
    def brief_comment
    end

    ##
    # Given a cursor that represents a declaration, return the associated comment's source range. The range may include
    # multiple consecutive comments with whitespace in between.
    # @return [SourceRange] a {SourceRange} object describing the bounds of the comment.
    def comment_range
    end

    ##
    # Given a cursor that represents a documentable entity (e.g. declaration), return the associated parsed comment as a
    # {Comment::FullComment} AST node.
    # @return [Comment::FullComment] the full parsed comment of the cursor entity.
    def comment
    end

    # @!endgroup

    ##
    # Retrieve the integer value of an enum constant declaration.
    # @param unsigned [Boolean] Flag indicating if value should be interpreted as an unsigned value. This is only
    #   necessary when dealing with an enumeration backed by a 64-bit unsigned value and has field values
    #   exceeding `0x7FFFFFFFFFFFFFFF`.
    # @return [Integer?] the integral value of the enum member, or `nil` if cursor does not represent an enum constant.
    def enum_value(unsigned = false)
    end

    ##
    # Retrieve the bit width of a bit field declaration as an integer.
    # @return [Integer] the bit width of field, or `-1` if cursor does not represent a field declaration.
    def bit_width
    end

    ##
    # Retrieve the number of non-variadic arguments associated with the cursor.
    # @return [Integer] the number of arguments, or `-1` if cursor does not represent a function/method declaration.
    def arg_count
    end

    ##
    # Retrieve the argument cursors of a function or method.
    # @return [Array<Cursor>] an array of {Cursor} objects for each argument, or an empty array if cursor does not
    #   not represent a function/method declaration.
    def args
    end

    ##
    # Enumerates through each argument of a function or method.
    #
    # @overload each_arg(&block)
    #   When called with a block, yields each argument cursor to the block before returning `self`.
    #   @yieldparam arg [Cursor] Yields the cursor for argument to the block.
    #   @return [self]
    #
    # @overload each_arg
    #   When called without a block, returns an enumerator suitable for iterating through the arguments.
    #   @return [Enumerator] an Enumerator for the arguments.
    #
    def each_arg
    end

    ##
    # Enumerates the template arguments of a C++ function/method declaration.
    #
    # @overload each_template_arg(&block)
    #   When called with a block, yields the template argument information to the block.
    #   @yieldparam index [Integer] The index of the template argument.
    #   @yieldparam type [Type] The type of the template argument.
    #   @yieldparam kind [Symbol] A Symbol representing the kind of the template argument.
    #   @yieldparam value [Integer?] The integral value of the template argument, or `nil` if not integral.
    #   @return [self]
    #
    # @overload each_template_arg
    #   When called without a block, returns an enumerator for the template argument information.
    #   @return [Enumerator] the template argument enumerator.
    # @see TemplateArgumentKind
    def each_template_arg
    end

    ##
    # Determine whether cursor is a macro and function-like.
    # @return [Boolean] `true` if cursor is a macro-function, otherwise `false`.
    def macro_func?
    end

    ##
    # Determine whether the cursor is a builtin macro.
    # @return [Boolean] `true` if cursor is a builtin macro, otherwise `false`.
    def macro_builtin?
    end

    ##
    # Determines if the the cursor is an inline function declaration or if it is namespace, indicates if it is inline.
    # @return [Boolean] `true` if cursor is an inline function or namespace, otherwise `false`.
    def inline?
    end

    ##
    # Retrieve the exception specification type associated with a given cursor.
    # @return [Symbol] a Symbol object describing the exception kind, or `:invalid` it cursor is not a function.
    # @see CursorExceptionSpecificationKind
    def exception_type
    end

    ##
    # Determine whether the given cursor represents an anonymous tag, namespace, or record declaration.
    # @return [Boolean] `true` if entity is anonymous, otherwise `false`.
    def anonymous?
    end

    ##
    # Returns non-zero if the cursor specifies a Record member that is a bitfield.
    # @return [Boolean] `true` if entity is a bitfield, otherwise `false`.
    def bitfield?
    end

    ##
    # Determines if the CXX base class for this cursor is virtual.
    # @return [Boolean] `true` if base class is virtual, otherwise `false`.
    def virtual_base?
    end

    ##
    # Visit the children of a particular cursor, invoking the specified block for each.
    #
    # This function visits all the direct children of the given cursor, invoking the given block with the cursors of
    # each  visited child. The traversal may be recursive, if the block returns `:recurse`. The traversal may also be
    # ended prematurely, if the visitor returns `:break`.
    #
    # @yieldparam cursor [Cursor] The cursor being visited.
    # @yieldparam parent [Cursor] The parent visitor for the cursor.
    # @yieldreturn [Symbol?] a Symbol defined in {ChildVisitResult}. An undefined/invalid value is interpreted as `:break`.
    #
    # @example Print Out All Enums
    #   cursor.visit_children do |child, parent|
    #     case child.kind
    #     when :enum_decl
    #       puts child.type.spelling.sub(/^enum /, '')
    #       next :recurse # Visit enum fields
    #     when :enum_constant_decl
    #       puts sprintf("    %s = 0x%X", child.spelling, child.enum_value)
    #       next :continue
    #     when :invalid
    #       warn('Invalid cursor, breaking loop!')
    #       next :break # Or alternatively just "break" normally
    #     else
    #       # Ignore everything else
    #       next :continue
    #     end
    #   end
    # @return [void]
    # @see ChildVisitResult
    def visit_children(&block)
    end

    ##
    # Given a cursor pointing to a C++ method call or an Objective-C
    # message, returns `true` if the method/message is "dynamic", meaning:
    #
    # For a C++ method: the call is virtual.
    # For an Objective-C message: the receiver is an object instance, not 'super'
    # or a specific class.
    #
    # If the method/message is "static" or the cursor does not point to a
    # method/message, it will return `false`.
    #
    # @return [Boolean] `true` if cursor is "dynamic", otherwise `false`.
    def dynamic_call?
    end

    ##
    # Retrieve the string representing the mangled name of the cursor.
    # @return [String] the mangled name.
    def mangling
    end

    ##
    # Retrieve the strings representing the mangled symbols of the C++ constructor or destructor at the cursor.
    # @return [Array<String>] an array of mangled names for a C++ constructor/destructor.
    def cxx_manglings
    end

    ##
    # Determine whether the declaration pointed to by this cursor is also a definition of that entity.
    # @return [Boolean] `true` if cursor is a definition, otherwise `false`.
    # @see definition
    def definition?
    end

    # For a cursor that is either a reference to or a declaration of some entity, retrieve a cursor that describes the
    # definition of that entity.
    #
    # Some entities can be declared multiple times within a translation  unit, but only one of those declarations can
    # also be a definition.
    #
    # @example
    #   int f(int, int);
    #   int g(int x, int y) { return f(x, y); }
    #   int f(int a, int b) { return a + b; }
    #   int f(int, int);
    #
    # In the given example, there are three declarations of the function "f", but only the  second one is a definition.
    # This method will take any cursor pointing to a declaration of "f" (the first or fourth lines of the example) or a
    # cursor referenced that uses "f" (the call to "f' inside "g") and will return a  declaration cursor pointing to the
    # definition (the second "f" declaration).
    #
    # If given a cursor for which there is no corresponding definition, e.g., because there is no definition of that
    # entity within this translation unit, returns a `NULL` cursor.
    #
    # @return [Cursor] the definition cursor, or a `NULL` cursor instance if no definition can be found.
    # @see definition?
    def definition
    end

    ##
    # Retrieve the canonical cursor corresponding to the given cursor.
    #
    # In the C family of languages, many kinds of entities can be declared several  times within a single translation
    # unit. For example, a structure type can be forward-declared (possibly multiple times) and later defined.
    #
    # @example
    #   struct X;
    #   struct X;
    #   struct X {
    #     int member;
    #   };
    #
    # In the given example, the declarations and the definition of `X` are represented by three different cursors, all
    # of which are declarations of the same underlying entity. One of these cursor is considered the "canonical" cursor,
    # which is effectively the representative for the underlying entity. One can determine if two cursors are
    # declarations of the same underlying entity by comparing their canonical cursors.
    #
    # @return [Cursor] The canonical cursor for the entity referred to by the given cursor.
    def canonical
    end

    ##
    # Returns the storage class for a function or variable declaration.
    #
    # @return [Symbol] A symbol describing the storage class, or `:invalid` if cursor is not function or variable
    #   declaration.
    # @see StorageClass
    def storage_class
    end

    ##
    # Given a cursor for a module import declaration, return the associated module.
    # @return [Module?] The module, or `nil` if cursor is not a module declaration.
    def module
    end

    ##
    # Determines if the cursor is a variadic function or method.
    # @return [Boolean] `true` if cursor is variadic, otherwise `false`.
    def variadic?
    end

    ##
    # Determines if the cursor points to a symbol marked with `external_source_symbol` attribute.
    # @return [Boolean] `true` if cursor is marked external, otherwise `false`.
    def external?
    end

    ##
    # Retrieves general information about a symbol marked with `external_source_symbol` attribute.
    #
    # The return value is a Hash object with following members:
    #
    # Key | Description
    # --- | ---
    # `:language` | The 'language' string from the attribute.
    # `:defined` | The 'definedIn' string from the attribute.
    # `:generated` | `true` if the 'generated_declaration' is set in the attribute, otherwise `false`.
    #
    # @return [Hash{Symbol => Object}] a hash containing information about the external symbol.
    def external_info
    end
    
    ##
    # Enumerates through the overloads of the cursor entity.
    # 
    # @overload each_overload(&block)
    #   When called with a block, yields a cursor for each overload of the entity before returning `self`.
    #   @yieldparam overload [Cursor] A cursor representing an overload.
    #   @return [self]
    # 
    # @overload each_overload
    #   When called without a block, returns an enumerator for the overloads.
    #   @return [Enumerator] the overload enumerator.
    def each_overload
    end
    
    ##
    # Find references of a declaration in a specific file.
    # 
    # @param source_file [File] Specifies the source file to search, otherwise `nil` to search the cursor's file.
    # 
    # @yieldparam cursor [Cursor] A cursor pointing to the found reference.
    # @yieldparam range [SourceRange] A range describing the extent of the found reference.
    # @yieldreturn [Symbol] Either `:continue` or `:break`. An undefined/invalid value will be interpreted as `:break`.
    #
    # @return [void]
    # @see Result
    def find_references(source_file = nil)
    end
    
    ##
    # For a cursor that is a reference, retrieve a cursor representing the  entity that it references.
    # 
    # Reference cursors refer to other entities in the AST. For example, an Objective-C superclass reference cursor
    # refers to an Objective-C class. This method produces the cursor for the Objective-C class from the cursor for the
    # superclass reference. If the input cursor is a declaration or definition, it returns that declaration or
    # definition unchanged. Otherwise, returns the `NULL` cursor.
    # 
    # @return [Cursor] the referenced cursor.
    def referenced
    end

    # @!group Objective-C

    ##
    # Given a cursor that represents a property declaration, return the name of the method that implements the getter.
    # @return [String] the Objective-C getter.
    def obj_c_getter
    end

    ##
    # Given a cursor that represents a property declaration, return the name of the method that implements the setter.
    # @return [String] the Objective-C setter.
    def obj_c_setter
    end

    ##
    # Given a cursor that represents an Objective-C method or parameter declaration, return the associated Objective-C
    # qualifiers for the return type or the parameter respectively.
    #
    # @return [Array<Symbol>] an array of Symbol objects describing qualifiers.
    # @see ObjCDeclQualifierKind
    def obj_c_qualifiers
    end

    ##
    # If the cursor points to a selector identifier in an Objective-C method or message expression, this returns the
    # selector index. This can be called to determine if the location points to a selector identifier.
    #
    # @return [Integer] The selector index if the cursor is an Objective-C method or message expression and the cursor
    #   is pointing to a selector identifier, or `-1` otherwise.
    def obj_c_selector_index
    end

    ##
    # Given a cursor that represents a property declaration, return the associated property attributes.
    #
    # @return [Array<Symbol>] an array of Symbol objects describing attributes.
    # @see ObjCPropertyAttrKind
    def obj_c_attributes
    end

    # Given a cursor that represents an Objective-C method or property  declaration, return `true` if the declaration
    # was affected by `@optional`. Returns `false` if the cursor is not such a declaration or it is `@required`.
    #
    # @return [Boolean] `true` if method/property is optional, otherwise `false`.
    def obj_c_optional?
    end

    ##
    # Retrieve the strings representing the mangled symbols of the Objective-C class interface or implementation at
    # the cursor.
    #
    # @return [Array<String>] an array of mangled names for a Objective-C class interface or implementation.
    def obj_c_manglings
    end

    # @!endgroup

    ##
    # If cursor is a statement declaration tries to evaluate the statement and if its variable, tries to evaluate its
    # initializer, into its corresponding type. If it's an expression, tries to evaluate the expression.
    #
    # @return [Integer,Float,String,NilClass] the resulting value of the evaluation, or `nil` if it failed or result was
    #   not a primitive type.
    def eval
    end

    ##
    # Retrieve a completion string for an arbitrary declaration or macro definition cursor.
    #
    # @return [CompletionString?] A non-context-sensitive completion string for declaration and macro definition cursors,
    #   or `nil` for other kinds of cursors.
    def completion_string
    end
  end
end


__END__

    rb_define_method0(rb_cCXCursor, "field_offset", cursor_field_offset, 0); TODO

