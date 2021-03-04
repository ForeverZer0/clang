module Clang
  ##
  # A single translation unit, which resides in an {Index}.
  class TranslationUnit

    ##
    # Returns the set of flags that is suitable for parsing a translation unit that is being edited.
    #
    # The set of flags returned provide options for {parse} to indicate that the translation unit is likely to be
    # reparsed many times, either explicitly (via {#reparse}) or implicitly  (e.g., by code completion
    # ({#code_complete}). The returned flag set contains an unspecified set of optimizations (e.g., the precompiled
    # preamble) geared toward improving the performance of these routines. The set of optimizations enabled may change
    # from one version to the next.
    #
    # @return [Array<Symbol>] the default options.
    # @see TranslationUnitFlags
    # @see parse
    def self.default_options
    end

    ##
    # Returns the set of flags that is suitable for saving a translation unit.
    #
    # The set of flags returned provide options for {#save} by default. The returned flag set contains an unspecified
    # set of options that save translation units with the most commonly-requested data.
    #
    # @return [Array<Symbol>] the default options.
    # @see SaveTranslationUnitFlags
    # @see save
    def self.default_save_options
    end

    ##
    # Returns the set of flags that is suitable for reparsing a translation
    # unit.
    #
    # The set of flags returned provide options for {#reparse} by default. The returned flag set contains an
    # unspecified set of optimizations geared toward common uses of reparsing. The set of optimizations enabled may
    # change from one version to the next.
    #
    # @return [Array<Symbol>] the default options.
    # @see ReparseFlags
    # @see reparse
    def self.default_reparse_options
    end

    ##
    # Retrieves a translation unit for a given source file and the provided command line arguments one would pass to
    # the compiler.
    #
    # @param index [Index] The index object with which the translation unit will be associated.
    # @param source_file [String?] The name of the source file to load, or `nil` if the source file is included in
    #   the `command_args` parameter.
    # @param command_args [Array<String>?] The command-line arguments that would be passed to the `clang` executable if
    #   it were being invoked out-of-process. These command-line options will be parsed and will affect how the
    #   translation unit is parsed.
    # @param unsaved [Array<UnsavedFile>?] The files that have not yet been saved to disk but may be required for code
    #   completion, including the contents of those files.
    #
    # @note The 'source_file' argument is optional, though when `nil`, the name of the source file is expected to
    #   reside in the specified command line arguments.
    #
    # @return [TranslationUnit] the newly created translation unit.
    def self.from_source(index, source_file = nil, command_args = nil, unsaved = nil)
    end

    ##
    # Parse the given source file and the translation unit corresponding to that file.
    #
    # This routine is the main entry point for the Clang C API, providing the
    # ability to parse a source file into a translation unit that can then be
    # queried by other functions in the API. This routine accepts a set of
    # command-line arguments so that the compilation can be configured in the same
    # way that the compiler is configured on the command line.
    #
    # @param index [Index] The index object with which the translation unit will be associated.
    # @param source_file [String?] The name of the source file to load, or `nil` if the source file is included in
    #   the `command_args` parameter.
    # @param command_args [Array<String>?] The command-line arguments that would be passed to the `clang` executable if
    #   it were being invoked out-of-process. These command-line options will be parsed and will affect how the
    #   translation unit is parsed.
    # @param unsaved [Array<UnsavedFile>?] The files that have not yet been saved to disk but may be required for code
    #   completion, including the contents of those files.
    # @param options [Symbol,Array<Symbol>] A set of options that affects how the translation unit is managed but not
    #   its compilation.
    #
    # @note The 'source_file' argument is optional, though when `nil`, the name of the source file is expected to
    #   reside in the specified command line arguments.
    #
    # @return [TranslationUnit] the newly created translation unit.
    # @see TranslationUnitFlags
    def self.parse(index, source_file = nil, command_args = nil, unsaved = nil, *options)
    end

    ##
    # Creates a new instance of the {TranslationUnit} class from the specified AST file (`-emit-ast`).
    #
    # @param index [Index] The index object with which the translation unit will be associated.
    # @param ast_path [String] The path to an AST file.
    def initialize(index, ast_path)
    end

    ##
    # Retrieve all ranges that were skipped by the preprocessor.
    #
    # The preprocessor will skip lines when they are surrounded by an
    # if/ifdef/ifndef directive whose condition does not evaluate to true.
    #
    # @param file [File] When specified, limits the returned ranges to the specified {File}
    #   instead of the entire translation unit.
    #
    # @return [Array<SourceRange>] An array of ranges that were skipped by the preprocessor.
    def skipped_ranges(file = nil)
    end

    ##
    # Determine the number of diagnostics produced for the given translation unit.
    # @return [Integer] the number of child diagnostics.
    def diagnostic_count
    end

    ##
    # Retrieve the complete set of diagnostics associated with a translation unit.
    # @return [DiagnosticSet?] a set of diagnostics, or `nil` if none exist.
    def diagnostics
    end

    ##
    # Enumerates through each child diagnostic in the translation unit.
    #
    # @overload each_diagnostic(&block)
    #   When called with a block, yields each diagnostic to the block before returning `self`.
    #   @yieldparam diagnostic [Diagnostic] The child diagnostic object.
    #   @return [self]
    #
    # @overload each_diagnostic
    #   When called without a block, returns an Enumerator for the diagnostic set.
    #   @return [Enumerator] The set enumerator.
    def each_diagnostic
    end

    ##
    # Get the original translation unit source file name.
    # @return [String] the source file name.
    def spelling
    end

    ##
    # Saves a translation unit into a serialized representation of that translation unit on disk.
    #
    # Any translation unit that was parsed without error can be saved into a file. The translation unit can then be
    # deserialized into a new {TranslationUnit} or, if it is an incomplete translation unit that corresponds to a
    # header, used as a precompiled header when parsing other translation
    # units.
    #
    # @param path [String] The file to which the translation unit will be saved.
    # @param options [Symbol,Array<Symbol>] A set options that affects how the translation unit is saved.
    #
    # @return [self]
    # @see default_save_options
    # @see SaveTranslationUnitFlags
    def save(path, *options)
    end

    ##
    # Reparse the source files that produced this translation unit.
    #
    # This routine can be used to re-parse the source files that originally created the given translation unit, for
    # example because those source files have changed (either on disk or as passed via `unsaved`). The source code will
    # be reparsed with the same command-line options as it was originally parsed.
    #
    # Reparsing a translation unit invalidates all cursors and source locations that refer into that translation unit.
    # This makes reparsing a translation unit semantically equivalent to destroying the translation unit and then
    # creating a new translation unit with the same command-line arguments. However, it may be more efficient to
    # reparse a translation unit using this routine.
    #
    # @param unsaved [Array<UnsavedFile>?] The files that have not yet been saved to disk but may be required for
    #   parsing, including the contents of those files.
    # @param options [Symbol,Array<Symbol>] A set options that affects how the translation unit is saved.
    #
    # @return [self]
    # @see default_reparse_options
    # @see ReparseFlags
    def reparse(unsaved = nil, *options)
    end

    ##
    # Suspend a translation unit in order to free memory associated with it.
    #
    # A suspended translation unit uses significantly less memory but on the other side does not support any calls other
    # than {#reparse}.
    #
    # @return [Boolean] `true` if successfully suspended, otherwise `false` if the operation failed.
    # @see reparse
    def suspend
    end

    ##
    # Return the memory usage of a translation unit broken down into categories.
    #
    # @return [Hash{String => Integer}] a hash describing the resource usage, where the keys are a description of the
    #   type of resource, and the values are the numerical value describing the amount. The units of the values are
    #   dependent on the type of the resource being described.
    def resource_usage
    end

    ##
    # Retrieves a Hash containing target information for this translation unit.
    #
    # The returned Hash can contain the following keys:
    #
    # Key | Value
    # --- | ---
    # `:platform` | A String containing target platform triplet (i.e. "x86_64-linux-gnu")
    # `pointer_width` | An Integer describing the pointer width of the target, in bits.
    #
    # @return [Hash]
    def target_info
    end

    ##
    # Retrieve the cursor that represents the given translation unit. The top-level translation unit cursor can be used
    # to start traversing the various declarations within.
    #
    # @return [Cursor] the top-level cursor of this translation unit.
    def cursor
    end

    ##
    # Visits each included file in the translation unit, invoking the given block each time a file is included.
    #
    # @yieldparam file [File] The file being included.
    # @yieldparam stack [Array<SourceLocation>] The current inclusion stack at the point the current file is being
    #   included, sorted in order of immediate inclusion.
    #
    # @return [void]
    def inclusions
    end

    # Determine whether the given header is guarded against  multiple inclusions, either with the conventional
    # `#ifndef`, `#define`, and `#endif` macro guards or with `#pragma once`.
    #
    # @param file [File] A {File} instance to query.
    # @return [Boolean] `true` if file is include-guarded, otherwise `false`.
    def include_guarded?(file)
    end

    ##
    # Perform code completion at a given location in a translation unit.
    #
    # This function performs code completion at a particular file, line, and
    # column within source code, providing results that suggest potential
    # code snippets based on the context of the completion. The basic model
    # for code completion is that Clang will parse a complete source file,
    # performing syntax checking up to the location where code-completion has
    # been requested. At that point, a special code-completion token is passed
    # to the parser, which recognizes this token and determines, based on the
    # current location in the C/Objective-C/C++ grammar and the state of
    # semantic analysis, what completions to provide. These completions are
    # returned via a new {CodeCompleteResults} object.
    #
    # Code completion itself is meant to be triggered by the client when the
    # user types punctuation characters or whitespace, at which point the
    # code-completion location will coincide with the cursor. For example, if `p`
    # is a pointer, code-completion might be triggered after the "-" and then
    # after the ">" in `p->`. When the code-completion location is after the ">",
    # the completion results will provide, e.g., the members of the struct that
    # "p" points to. The client is responsible for placing the cursor at the
    # beginning of the token currently being typed, then filtering the results
    # based on the contents of the token. For example, when code-completing for
    # the expression `p->get`, the client should provide the location just after
    # the ">" (e.g., pointing at the "g") to this code-completion hook. Then, the
    # client can filter the results based on the current token text ("get"), only
    # showing those results that start with "get". The intent of this interface
    # is to separate the relatively high-latency acquisition of code-completion
    # results from the filtering of results on a per-character basis, which must
    # have a lower latency.
    #
    # @param filename [String] The name of the source file where code completion should be performed. This filename
    #   may be any file included in the translation unit.
    # @param line [Integer] The line at which code-completion should occur.
    # @param column [Integer] The column at which code-completion should occur. Note that the column should point just
    #   after the syntactic construct that initiated code completion, and not in the middle of a lexical token.
    # @param unsaved [Array<UnsavedFile>?] An optional array of files that have not yet been saved to disk but may be
    #   required for parsing or code completion.
    # @param options [Symbol,Array<Symbol>] Extra options that control the behavior of code completion.
    #
    # @return [CodeCompleteResults?] If successful, a new {CodeCompleteResults} structure containing code-completion
    #   results, otherwise `nil`.
    # @see CodeCompleteFlags
    def code_complete(filename, line, column, unsaved, *options)
    end
  end
end