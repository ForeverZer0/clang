module Clang
  ##
  # Module containing routines for create USR strings.
  #
  # A Unified Symbol Resolution (USR) is a string that identifies a particular
  # entity (function, class, variable, etc.) within a program. USRs can be
  # compared across translation units to determine, e.g., when references in
  # one translation refer to an entity defined in another translation unit.
  module USR

    ##
    # Retrieve a Unified Symbol Resolution (USR) for the entity referenced by the given cursor.
    #
    # @param cursor [Cursor] The cursor to create the USR from.
    # @return [String] the USR string.
    def self.from_cursor(cursor)
    end

    ##
    # Construct a USR for a specified Objective-C class.
    #
    # @param class_name [String] The name of an Objective-C class.
    # @return [String] the USR string.
    def self.obj_c_class(class_name)
    end

    ##
    # Construct a USR for a specified Objective-C category.
    #
    # @param class_name [String] The name of an Objective-C class.
    # @param category_name [String] The name of an Objective-C category.
    # @return [String] the USR string.
    def self.obj_c_category(class_name, category_name)
    end

    ##
    # Construct a USR for a specified Objective-C protocol.
    #
    # @param protocol_name [String] The name of an Objective-C protocol.
    # @return [String] the USR string.
    def self.obj_c_protocol(protocol_name)
    end

    ##
    # Construct a USR for a specified Objective-C instance variable and the USR for its containing class.
    #
    # @param class_name [String] The name of an Objective-C class.
    # @param ivar_name [String] The name of an Objective-C variable name.
    # @return [String] the USR string.
    def self.obj_c_ivar(class_name, ivar_name)
    end


    ##
    # Construct a USR for a specified Objective-C property and the USR for its containing class.
    #
    # @param class_name [String] The name of an Objective-C class.
    # @param property_name [String] The name of an Objective-C property name.
    # @return [String] the USR string.
    def self.obj_c_property(class_name, property_name)
    end

    ##
    # Construct a USR for a specified Objective-C method and the USR for its containing class.
    #
    # @param class_name [String] The name of an Objective-C class.
    # @param method_name [String] The name of an Objective-C method name.
    # @param instance [Boolean] Flag indicating if the method is an instance method.
    # @return [String] the USR string.
    def self.obj_c_method(class_name, method_name, instance)
    end
  end

end