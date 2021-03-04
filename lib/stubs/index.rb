module Clang
  ##
  # An "index" that consists of a set of translation units that would typically be linked together into an executable or library.
  class Index

    ##
    # A set of general options associated with a {Index} object.
    # @return [Array<Symbol>] An array of option flags indicating the {Index} behavior.
    # @see GlobalOptFlags
    attr_accessor :global_opts

    ##
    # Sets the invocation emission path option in a {Index} instance.
    # 
    # The invocation emission path specifies a path which will contain log files for certain libclang invocations. 
    #
    # @param path [String?] The path where to write the log, or `nil` (default) implies that libclang invocations are not logged.
    # @return [void]
    def emission_path(path)
    end


    ##
    # Creates a new {Index} instance, which provides a shared context for creating translation units.
    #
    # @overload create(exclude_pch = false, display = false, &block)
    #   When called with a block, yields the newly created {Index} to the block, then returns `nil`.
    #   @yieldparam index [Index] The newly created {Index} instance. Any API objects that were created
    #     within the block are invalidated and no longer suitable for use when the block exits.
    #   @return [void]
    #
    # @overload create(exclude_pch = false, display = false)
    #   When called without a block, returns the created instance.
    #   @return [Index] The newly create {Index} instance.
    #
    # @param exclude_pch [Boolean] When `true` allows enumeration of "local" declarations (when loading
    #   any new translation units). A "local" declaration is one that belongs in the translation unit
    #   itself and not in a precompiled header that was used by the translation unit. If `false`, all
    #   declarations will be enumerated.
    # @param display [Boolean] When `true`, Clang will output messages to the STDOUT, otherwise `false` to suppress messages.
    # @param global_opts [Symbol,Array<Symbol>] A set of global options ({#global_opts}) to apply to the new instance.
    #
    # @see GlobalOptFlags
    def self.create(exclude_pch = false, display = false, *global_opts, &block)
    end
  end
end