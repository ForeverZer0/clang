module Clang

  ##
  # Object encapsulating information about overlaying virtual file/directories over the real file system.
  class VirtualFileOverlay

    ##
    # Creates a new instance of the {VirtualFileOverlay} class.
    # 
    # @param case_sensitive [Boolean] Flag indicating if paths are case sensitive.
    def initialize(case_sensitive = true)
    end

    ##
    # Map an absolute virtual file path to an absolute real one. The virtual path must be canonicalized (not contain "."/"..").
    # 
    # @param virtual_path [String] A virtual path to map to.
    # @param real_path [String] A real path to map from.
    # 
    # @return [Boolean] `true` on success, otherwise `false` if an error occurred.
    def map(virtual_path, real_path)
    end

    ##
    # Reads the contents of the {VirtualFileOverlay} object.
    # @return [String] The overlay contents.
    def read
    end

    alias_method :to_s, :read

  end
end