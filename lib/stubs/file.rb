module Clang
  ##
  # Provides the contents of a file that has not yet been saved to disk.
  #
  # Each {UnsavedFile} instance provides the name of a file on the
  # system along with the current contents of that file that have not
  # yet been saved to disk.
  class UnsavedFile
    ##
    # Creates a new instance of the {UnsavedFile} class.
    #
    # @param path [String] The file whose contents have not yet been saved, which must already exist in the file system.
    # @param contents [String] The unsaved contents of this file.
    def initialize(path, contents)
    end
  end

  ##
  # A particular source file that is part of a translation unit.
  class File

    ##
    # @return [String] the complete file and path name of the given file.
    attr_reader :name

    ##
    # Returns the real path name of file. In case of of error, will return an empty string, in which case use {#name}.
    # @return [String] The real path of the file.
    attr_reader :real_name

    ##
    # @return [Time] the last modification time of the given file.
    attr_reader :time

    ##
    # Creates a new instance of the {File} class.
    #
    # @param filename [String] The name of the file.
    # @param translation_unit [TranslationUnit] The parent translation unit.
    def initialize(filename, translation_unit)
    end

    # Determine whether the given header is guarded against  multiple inclusions, either with the conventional
    # `#ifndef`, `#define`, and `#endif` macro guards or with `#pragma once`.
    # @return [Boolean] `true` if include-guarded, otherwise `false`.
    def guarded?
    end

    ##
    # Retrieve the string contents associated with the file.
    def contents
    end

    ##
    # Determines if this instance is equal to another. If the other object is another {File} instance, determines
    # if both refer to the same file.
    #
    # @param other [Object] The object to compare for equality.
    # @return [Boolean] `true` of objects are equal, otherwise `false`.
    def ==(other)
    end

    alias_method :eql?, :==

  end
end