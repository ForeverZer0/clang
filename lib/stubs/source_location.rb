module Clang
  ##
  # Identifies a specific source location within a translation unit.
  class SourceLocation

    ##
    # Retrieve a `NULL` (invalid) {SourceLocation} instance.
    # @return [SourceLocation] The new instance.
    def self.null
    end

    ##
    # Creates a new instance of the {SourceLocation} class.
    #
    # @overload initialize(translation_unit, file, line, column)
    #   @param translation_unit [TranslationUnit] The parent translation unit this location resides within.
    #   @param file [File] The parent file this location resides within.
    #   @param line [Integer] The line number of the location.
    #   @param column [Integer] The column number of the location.
    #
    # @overload initialize(translation_unit, file, offset)
    #   @param translation_unit [TranslationUnit] The parent translation unit this location resides within.
    #   @param file [File] The parent file this location resides within.
    #   @param offset [Integer] The character offset of the location.
    def initialize
    end

    ##
    # Determines if if the source location is in a system header.
    # @return [Boolean] `true` if location is within a system header, otherwise `false`.
    def system_header?
    end

    ##
    # Determines if the source location is in the main file of the corresponding translation unit.
    # @return [Boolean] `true` if location is within the main file, otherwise `false`.
    def main_file?
    end

    ##
    # Retrieves the file the location resides in.
    #
    # @param type [Symbol] A flag indicating the strategy used to determine the result in the case of macros.
    #   <ul>
    #     <li><b>:file</b> If the location refers into a macro expansion, return where the macro was expanded or where the macro argument was written, if the location points at a macro argument.</li>
    #     <li><b>:presumed</b> The location as described by a <code>#line</code> directive.</li>
    #     <li><b>:spelling</b> If the location refers to a macro instantiation, return where the location was originally  spelled in the source file.</li>
    #     <li><b>:expansion</b> If the location refers into a macro expansion, retrieves the location of the macro expansion.</li>
    #   </ul>
    # @return [File] The source file for the location.
    def file(type = :file)
    end

    ##
    # Retrieves the line number the location resides at.
    #
    # @param type [Symbol] A flag indicating the strategy used to determine the result in the case of macros.
    #   <ul>
    #     <li><b>:file</b> If the location refers into a macro expansion, return where the macro was expanded or where the macro argument was written, if the location points at a macro argument.</li>
    #     <li><b>:presumed</b> The location as described by a <code>#line</code> directive.</li>
    #     <li><b>:spelling</b> If the location refers to a macro instantiation, return where the location was originally  spelled in the source file.</li>
    #     <li><b>:expansion</b> If the location refers into a macro expansion, retrieves the location of the macro expansion.</li>
    #   </ul>
    # @return [Integer] The line number for the location.
    def line(type = :file)
    end

    ##
    # Retrieves the column number the location resides at.
    #
    # @param type [Symbol] A flag indicating the strategy used to determine the result in the case of macros.
    #   <ul>
    #     <li><b>:file</b> If the location refers into a macro expansion, return where the macro was expanded or where the macro argument was written, if the location points at a macro argument.</li>
    #     <li><b>:presumed</b> The location as described by a <code>#line</code> directive.</li>
    #     <li><b>:spelling</b> If the location refers to a macro instantiation, return where the location was originally  spelled in the source file.</li>
    #     <li><b>:expansion</b> If the location refers into a macro expansion, retrieves the location of the macro expansion.</li>
    #   </ul>
    # @return [Integer] The column number for the location.
    def column(type = :file)
    end

    ##
    # Retrieves the character offset the location resides at.
    #
    # @param type [Symbol] A flag indicating the strategy used to determine the result in the case of macros.
    #   <ul>
    #     <li><b>:file</b> If the location refers into a macro expansion, return where the macro was expanded or where the macro argument was written, if the location points at a macro argument.</li>
    #     <li><b>:spelling</b> If the location refers to a macro instantiation, return where the location was originally  spelled in the source file.</li>
    #     <li><b>:expansion</b> If the location refers into a macro expansion, retrieves the location of the macro expansion.</li>
    #   </ul>
    # @return [Integer] The line number for the location.
    def offset(type = :file)
    end

    ##
    # Determines if this instance is equal to another. If the other object is another {SourceLocation} instance,
    # determines if both refer to the same location.
    #
    # @param other [Object] The object to compare for equality.
    # @return [Boolean] `true` of objects are equal, otherwise `false`.
    def ==(other)
    end

    alias_method :eql?, :==
  end
end
