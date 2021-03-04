module Clang
  ##
  # Object encapsulating information about a module.map file.
  class ModuleMapDescriptor

    ##
    # Creates a new instance of the {ModuleMapDescriptor} class.
    #
    # @param module_name [String] The framework module name that the module.map describes.
    # @param umbrella_header [String] The umbrella header name that the module.map describes.
    def initialize(module_name, umbrella_header)
    end

    ##
    # Reads the contents of the {ModuleMapDescriptor} object.
    # @return [String] The module map contents.
    def read
    end

    alias_method :to_s, :read
  end
end