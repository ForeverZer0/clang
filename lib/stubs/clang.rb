
##
# The C Interface to Clang provides a relatively small API that exposes
# facilities for parsing source code into an abstract syntax tree (AST),
# loading already-parsed ASTs, traversing the AST, associating
# physical source locations with elements within the AST, and other
# facilities that support Clang-based development tools.
#
# This C interface to Clang will never provide all of the information
# representation stored in Clang's C++ AST, nor should it: the intent is to
# maintain an API that is relatively stable from one release to the next,
# providing only the basic functionality needed to support development tools.
# 
# @author Eric Freed
module Clang
  
  ##
  # Return a version string, suitable for showing to a user, but not intended to be parsed (the format is not guaranteed to be stable).
  # @return [String] The version string.
  def self.version
  end

  ##
  # Enable/disable crash recovery.
  # @param enabled [Boolean] `true` to enable recovery, otherwise `false` to disable it.
  # @return [void]
  def self.crash_recovery(enabled)
  end

  ##
  # Describes the availability of a particular entity, which indicates whether the use of this entity will result in a warning or error due to it being deprecated or unavailable.
  #
  # Member | Description
  # --- | ---
  # `:available` |  The entity is available.
  # `:deprecated` |  The entity is available, but has been deprecated (and its use is not recommended).
  # `:not_available` |  The entity is not available; any use of it will be an error.
  # `:not_accessible` |  The entity is available, but not accessible; any use of it will be an error.
  # @return [Enum]
  AvailabilityKind = Enum.new

  ##
  # Describes the exception specification of a cursor.
  #
  # Member | Description
  # --- | ---
  # `:none` |  The cursor has no exception specification.
  # `:dynamic_none` |  The cursor has exception specification throw()
  # `:dynamic` |  The cursor has exception specification throw(T1, T2)
  # `:ms_any` |  The cursor has exception specification throw(...).
  # `:basic_noexcept` |  The cursor has exception specification basic noexcept.
  # `:computed_noexcept` |  The cursor has exception specification computed noexcept.
  # `:unevaluated` |  The exception specification has not yet been evaluated.
  # `:uninstantiated` |  The exception specification has not yet been instantiated.
  # `:unparsed` |  The exception specification has not been parsed yet.
  # `:no_throw` |  The cursor has a __declspec(nothrow) exception specification.
  # @return [Enum]
  CursorExceptionSpecificationKind = Enum.new

  ##
  # Member | Description
  # --- | ---
  # `:none` |  Used to indicate that no special CXIndex options are needed.
  # `:thread_background_priority_for_indexing` |  Used to indicate that threads that libclang creates for indexing purposes should use background priority.
  # `:thread_background_priority_for_editing` |  Used to indicate that threads that libclang creates for editing purposes should use background priority.
  # `:thread_background_priority_for_all` |  Used to indicate that all threads that libclang creates should use background priority.
  # @return [Enum]
  GlobalOptFlags = Enum.new

  ##
  # Describes the severity of a particular diagnostic.
  #
  # Member | Description
  # --- | ---
  # `:ignored` |  A diagnostic that has been suppressed, e.g., by a command-line option.
  # `:note` |  This diagnostic is a note that should be attached to the previous (non-note) diagnostic.
  # `:warning` |  This diagnostic indicates suspicious code that may not be wrong.
  # `:error` |  This diagnostic indicates that the code is ill-formed.
  # `:fatal` |  This diagnostic indicates that the code is ill-formed such that future parser recovery is unlikely to produce useful results.
  # @return [Enum]
  DiagnosticSeverity = Enum.new

  ##
  # Options to control the display of diagnostics.
  #
  # Member | Description
  # --- | ---
  # `:display_source_location` |  Display the source-location information where the diagnostic was located.
  # `:display_column` |  If displaying the source-location information of the diagnostic, also include the column number.
  # `:display_source_ranges` |  If displaying the source-location information of the diagnostic, also include information about source ranges in a machine-parsable format.
  # `:display_option` |  Display the option name associated with this diagnostic, if any.
  # `:display_category_id` |  Display the category number associated with this diagnostic, if any.
  # `:display_category_name` |  Display the category name associated with this diagnostic, if any.
  # @return [Enum]
  DiagnosticDisplayOptions = Enum.new

  ##
  # Flags that control the creation of translation units.
  #
  # Member | Description
  # --- | ---
  # `:none` |  Used to indicate that no special translation-unit options are needed.
  # `:detailed_preprocessing_record` |  Used to indicate that the parser should construct a "detailed" preprocessing record, including all macro definitions and instantiations.
  # `:incomplete` |  Used to indicate that the translation unit is incomplete.
  # `:precompiled_preamble` |  Used to indicate that the translation unit should be built with an implicit precompiled header for the preamble.
  # `:cache_completion_results` |  Used to indicate that the translation unit should cache some code-completion results with each reparse of the source file.
  # `:for_serialization` |  Used to indicate that the translation unit will be serialized with clang_saveTranslationUnit.
  # `:cxx_chained_pch` |  DEPRECATED: Enabled chained precompiled preambles in C++.
  # `:skip_function_bodies` |  Used to indicate that function/method bodies should be skipped while parsing.
  # `:include_brief_comments_in_code_completion` |  Used to indicate that brief documentation comments should be included into the set of code completions returned from this translation unit.
  # `:create_preamble_on_first_parse` |  Used to indicate that the precompiled preamble should be created on the first parse. Otherwise it will be created on the first reparse. This trades runtime on the first parse (serializing the preamble takes time) for reduced runtime on the second parse (can now reuse the preamble).
  # `:keep_going` |  Do not stop processing when fatal errors are encountered.
  # `:single_file_parse` |  Sets the preprocessor in a mode for parsing a single file only.
  # `:limit_skip_function_bodies_to_preamble` |  Used in combination with CXTranslationUnit_SkipFunctionBodies to constrain the skipping of function bodies to the preamble.
  # `:include_attributed_types` |  Used to indicate that attributed types should be included in CXType.
  # `:visit_implicit_attributes` |  Used to indicate that implicit attributes should be visited.
  # `:ignore_non_errors_from_included_files` |  Used to indicate that non-errors from included files should be ignored.
  # `:retain_excluded_conditional_blocks` |  Tells the preprocessor not to skip excluded conditional blocks.
  # @return [Enum]
  TranslationUnitFlags = Enum.new

  ##
  # Flags that control how translation units are saved.
  #
  # Member | Description
  # --- | ---
  # `:none` |  Used to indicate that no special saving options are needed.
  # @return [Enum]
  SaveTranslationUnitFlags = Enum.new

  ##
  # Flags that control the reparsing of translation units.
  #
  # Member | Description
  # --- | ---
  # `:none` |  Used to indicate that no special reparsing options are needed.
  # @return [Enum]
  ReparseFlags = Enum.new

  ##
  # Categorizes how memory is being used by a translation unit.
  #
  # Member | Description
  # --- | ---
  # `:ast` | 
  # `:identifiers` | 
  # `:selectors` | 
  # `:global_completion_results` | 
  # `:source_manager_content_cache` | 
  # `:ast_side_tables` | 
  # `:source_manager_membuffer_malloc` | 
  # `:source_manager_membuffer_m_map` | 
  # `:external_ast_source_membuffer_malloc` | 
  # `:external_ast_source_membuffer_m_map` | 
  # `:preprocessor` | 
  # `:preprocessing_record` | 
  # `:source_manager_data_structures` | 
  # `:preprocessor_header_search` | 
  # `:memory_in_bytes_begin` | 
  # `:memory_in_bytes_end` | 
  # `:first` | 
  # `:last` | 
  # @return [Enum]
  TUResourceUsageKind = Enum.new

  ##
  # Describes the kind of entity that a cursor refers to.
  #
  # Member | Description
  # --- | ---
  # `:unexposed_decl` |  A declaration whose specific kind is not exposed via this interface.
  # `:struct_decl` |  A C or C++ struct.
  # `:union_decl` |  A C or C++ union.
  # `:class_decl` |  A C++ class.
  # `:enum_decl` |  An enumeration.
  # `:field_decl` |  A field (in C) or non-static data member (in C++) in a struct, union, or C++ class.
  # `:enum_constant_decl` |  An enumerator constant.
  # `:function_decl` |  A function.
  # `:var_decl` |  A variable.
  # `:parm_decl` |  A function or method parameter.
  # `:obj_c_interface_decl` |  An Objective-C @interface.
  # `:obj_c_category_decl` |  An Objective-C @interface for a category.
  # `:obj_c_protocol_decl` |  An Objective-C @protocol declaration.
  # `:obj_c_property_decl` |  An Objective-C @property declaration.
  # `:obj_c_ivar_decl` |  An Objective-C instance variable.
  # `:obj_c_instance_method_decl` |  An Objective-C instance method.
  # `:obj_c_class_method_decl` |  An Objective-C class method.
  # `:obj_c_implementation_decl` |  An Objective-C @implementation.
  # `:obj_c_category_impl_decl` |  An Objective-C @implementation for a category.
  # `:typedef_decl` |  A typedef.
  # `:cxx_method` |  A C++ class method.
  # `:namespace` |  A C++ namespace.
  # `:linkage_spec` |  A linkage specification, e.g. 'extern "C"'.
  # `:constructor` |  A C++ constructor.
  # `:destructor` |  A C++ destructor.
  # `:conversion_function` |  A C++ conversion function.
  # `:template_type_parameter` |  A C++ template type parameter.
  # `:non_type_template_parameter` |  A C++ non-type template parameter.
  # `:template_template_parameter` |  A C++ template template parameter.
  # `:function_template` |  A C++ function template.
  # `:class_template` |  A C++ class template.
  # `:class_template_partial_specialization` |  A C++ class template partial specialization.
  # `:namespace_alias` |  A C++ namespace alias declaration.
  # `:using_directive` |  A C++ using directive.
  # `:using_declaration` |  A C++ using declaration.
  # `:type_alias_decl` |  A C++ alias declaration
  # `:obj_c_synthesize_decl` |  An Objective-C @synthesize definition.
  # `:obj_c_dynamic_decl` |  An Objective-C @dynamic definition.
  # `:cxx_access_specifier` |  An access specifier.
  # `:first_decl` | 
  # `:last_decl` | 
  # `:first_ref` | 
  # `:obj_c_super_class_ref` | 
  # `:obj_c_protocol_ref` | 
  # `:obj_c_class_ref` | 
  # `:type_ref` |  A reference to a type declaration.
  # `:cxx_base_specifier` | 
  # `:template_ref` |  A reference to a class template, function template, template template parameter, or class template partial specialization.
  # `:namespace_ref` |  A reference to a namespace or namespace alias.
  # `:member_ref` |  A reference to a member of a struct, union, or class that occurs in some non-expression context, e.g., a designated initializer.
  # `:label_ref` |  A reference to a labeled statement.
  # `:overloaded_decl_ref` |  A reference to a set of overloaded functions or function templates that has not yet been resolved to a specific function or function template.
  # `:variable_ref` |  A reference to a variable that occurs in some non-expression context, e.g., a C++ lambda capture list.
  # `:last_ref` | 
  # `:first_invalid` | 
  # `:invalid_file` | 
  # `:no_decl_found` | 
  # `:not_implemented` | 
  # `:invalid_code` | 
  # `:last_invalid` | 
  # `:first_expr` | 
  # `:unexposed_expr` |  An expression whose specific kind is not exposed via this interface.
  # `:decl_ref_expr` |  An expression that refers to some value declaration, such as a function, variable, or enumerator.
  # `:member_ref_expr` |  An expression that refers to a member of a struct, union, class, Objective-C class, etc.
  # `:call_expr` |  An expression that calls a function.
  # `:obj_c_message_expr` |  An expression that sends a message to an Objective-C object or class.
  # `:block_expr` |  An expression that represents a block literal.
  # `:integer_literal` |  An integer literal.
  # `:floating_literal` |  A floating point number literal.
  # `:imaginary_literal` |  An imaginary number literal.
  # `:string_literal` |  A string literal.
  # `:character_literal` |  A character literal.
  # `:paren_expr` |  A parenthesized expression, e.g. "(1)".
  # `:unary_operator` |  This represents the unary-expression's (except sizeof and alignof).
  # `:array_subscript_expr` |  [C99 6.5.2.1] Array Subscripting.
  # `:binary_operator` |  A builtin binary operation expression such as "x + y" or "x <= y".
  # `:compound_assign_operator` |  Compound assignment such as "+=".
  # `:conditional_operator` |  The ?: ternary operator.
  # `:c_style_cast_expr` |  An explicit cast in C (C99 6.5.4) or a C-style cast in C++ (C++ [expr.cast]), which uses the syntax (Type)expr.
  # `:compound_literal_expr` |  [C99 6.5.2.5]
  # `:init_list_expr` |  Describes an C or C++ initializer list.
  # `:addr_label_expr` |  The GNU address of label extension, representing &&label.
  # `:stmt_expr` |  This is the GNU Statement Expression extension: (`{int X=4; X;}`)
  # `:generic_selection_expr` |  Represents a C11 generic selection.
  # `:gnu_null_expr` |  Implements the GNU __null extension, which is a name for a null pointer constant that has integral type (e.g., int or long) and is the same size and alignment as a pointer.
  # `:cxx_static_cast_expr` |  C++'s static_cast<> expression.
  # `:cxx_dynamic_cast_expr` |  C++'s dynamic_cast<> expression.
  # `:cxx_reinterpret_cast_expr` |  C++'s reinterpret_cast<> expression.
  # `:cxx_const_cast_expr` |  C++'s const_cast<> expression.
  # `:cxx_functional_cast_expr` |  Represents an explicit C++ type conversion that uses "functional" notion (C++ [expr.type.conv]).
  # `:cxx_typeid_expr` |  A C++ typeid expression (C++ [expr.typeid]).
  # `:cxx_bool_literal_expr` |  [C++ 2.13.5] C++ Boolean Literal.
  # `:cxx_null_ptr_literal_expr` |  [C++0x 2.14.7] C++ Pointer Literal.
  # `:cxx_this_expr` |  Represents the "this" expression in C++
  # `:cxx_throw_expr` |  [C++ 15] C++ Throw Expression.
  # `:cxx_new_expr` |  A new expression for memory allocation and constructor calls, e.g: "new CXXNewExpr(foo)".
  # `:cxx_delete_expr` |  A delete expression for memory deallocation and destructor calls, e.g. "delete[] pArray".
  # `:unary_expr` |  A unary expression. (noexcept, sizeof, or other traits)
  # `:obj_c_string_literal` |  An Objective-C string literal i.e. "foo".
  # `:obj_c_encode_expr` |  An Objective-C @encode expression.
  # `:obj_c_selector_expr` |  An Objective-C @selector expression.
  # `:obj_c_protocol_expr` |  An Objective-C @protocol expression.
  # `:obj_c_bridged_cast_expr` |  An Objective-C "bridged" cast expression, which casts between Objective-C pointers and C pointers, transferring ownership in the process.
  # `:pack_expansion_expr` |  Represents a C++0x pack expansion that produces a sequence of expressions.
  # `:size_of_pack_expr` |  Represents an expression that computes the length of a parameter pack.
  # `:lambda_expr` |  
  # `:obj_c_bool_literal_expr` |  Objective-c Boolean Literal.
  # `:obj_c_self_expr` |  Represents the "self" expression in an Objective-C method.
  # `:omp_array_section_expr` |  OpenMP 5.0 [2.1.5, Array Section].
  # `:obj_c_availability_check_expr` |  Represents an (...) check.
  # `:fixed_point_literal` |  Fixed point literal
  # `:omp_array_shaping_expr` |  OpenMP 5.0 [2.1.4, Array Shaping].
  # `:omp_iterator_expr` |  OpenMP 5.0 [2.1.6 Iterators]
  # `:cxx_addrspace_cast_expr` |  OpenCL's addrspace_cast<> expression.
  # `:last_expr` | 
  # `:first_stmt` | 
  # `:unexposed_stmt` |  A statement whose specific kind is not exposed via this interface.
  # `:label_stmt` |  A labelled statement in a function.
  # `:compound_stmt` |  A group of statements like { stmt stmt }.
  # `:case_stmt` |  A case statement.
  # `:default_stmt` |  A default statement.
  # `:if_stmt` |  An if statement
  # `:switch_stmt` |  A switch statement.
  # `:while_stmt` |  A while statement.
  # `:do_stmt` |  A do statement.
  # `:for_stmt` |  A for statement.
  # `:goto_stmt` |  A goto statement.
  # `:indirect_goto_stmt` |  An indirect goto statement.
  # `:continue_stmt` |  A continue statement.
  # `:break_stmt` |  A break statement.
  # `:return_stmt` |  A return statement.
  # `:gcc_asm_stmt` |  A GCC inline assembly statement extension.
  # `:asm_stmt` | 
  # `:obj_c_at_try_stmt` |  Objective-C's overall @try-@catch-@finally statement.
  # `:obj_c_at_catch_stmt` |  Objective-C's @catch statement.
  # `:obj_c_at_finally_stmt` |  Objective-C's @finally statement.
  # `:obj_c_at_throw_stmt` |  Objective-C's @throw statement.
  # `:obj_c_at_synchronized_stmt` |  Objective-C's @synchronized statement.
  # `:obj_c_autorelease_pool_stmt` |  Objective-C's autorelease pool statement.
  # `:obj_c_for_collection_stmt` |  Objective-C's collection statement.
  # `:cxx_catch_stmt` |  C++'s catch statement.
  # `:cxx_try_stmt` |  C++'s try statement.
  # `:cxx_for_range_stmt` |  C++'s for (* : *) statement.
  # `:seh_try_stmt` |  Windows Structured Exception Handling's try statement.
  # `:seh_except_stmt` |  Windows Structured Exception Handling's except statement.
  # `:seh_finally_stmt` |  Windows Structured Exception Handling's finally statement.
  # `:ms_asm_stmt` |  A MS inline assembly statement extension.
  # `:null_stmt` |  The null statement ";": C99 6.8.3p3.
  # `:decl_stmt` |  Adaptor class for mixing declarations with statements and expressions.
  # `:omp_parallel_directive` |  OpenMP parallel directive.
  # `:omp_simd_directive` |  OpenMP SIMD directive.
  # `:omp_for_directive` |  OpenMP for directive.
  # `:omp_sections_directive` |  OpenMP sections directive.
  # `:omp_section_directive` |  OpenMP section directive.
  # `:omp_single_directive` |  OpenMP single directive.
  # `:omp_parallel_for_directive` |  OpenMP parallel for directive.
  # `:omp_parallel_sections_directive` |  OpenMP parallel sections directive.
  # `:omp_task_directive` |  OpenMP task directive.
  # `:omp_master_directive` |  OpenMP master directive.
  # `:omp_critical_directive` |  OpenMP critical directive.
  # `:omp_taskyield_directive` |  OpenMP taskyield directive.
  # `:omp_barrier_directive` |  OpenMP barrier directive.
  # `:omp_taskwait_directive` |  OpenMP taskwait directive.
  # `:omp_flush_directive` |  OpenMP flush directive.
  # `:seh_leave_stmt` |  Windows Structured Exception Handling's leave statement.
  # `:omp_ordered_directive` |  OpenMP ordered directive.
  # `:omp_atomic_directive` |  OpenMP atomic directive.
  # `:omp_for_simd_directive` |  OpenMP for SIMD directive.
  # `:omp_parallel_for_simd_directive` |  OpenMP parallel for SIMD directive.
  # `:omp_target_directive` |  OpenMP target directive.
  # `:omp_teams_directive` |  OpenMP teams directive.
  # `:omp_taskgroup_directive` |  OpenMP taskgroup directive.
  # `:omp_cancellation_point_directive` |  OpenMP cancellation point directive.
  # `:omp_cancel_directive` |  OpenMP cancel directive.
  # `:omp_target_data_directive` |  OpenMP target data directive.
  # `:omp_task_loop_directive` |  OpenMP taskloop directive.
  # `:omp_task_loop_simd_directive` |  OpenMP taskloop simd directive.
  # `:omp_distribute_directive` |  OpenMP distribute directive.
  # `:omp_target_enter_data_directive` |  OpenMP target enter data directive.
  # `:omp_target_exit_data_directive` |  OpenMP target exit data directive.
  # `:omp_target_parallel_directive` |  OpenMP target parallel directive.
  # `:omp_target_parallel_for_directive` |  OpenMP target parallel for directive.
  # `:omp_target_update_directive` |  OpenMP target update directive.
  # `:omp_distribute_parallel_for_directive` |  OpenMP distribute parallel for directive.
  # `:omp_distribute_parallel_for_simd_directive` |  OpenMP distribute parallel for simd directive.
  # `:omp_distribute_simd_directive` |  OpenMP distribute simd directive.
  # `:omp_target_parallel_for_simd_directive` |  OpenMP target parallel for simd directive.
  # `:omp_target_simd_directive` |  OpenMP target simd directive.
  # `:omp_teams_distribute_directive` |  OpenMP teams distribute directive.
  # `:omp_teams_distribute_simd_directive` |  OpenMP teams distribute simd directive.
  # `:omp_teams_distribute_parallel_for_simd_directive` |  OpenMP teams distribute parallel for simd directive.
  # `:omp_teams_distribute_parallel_for_directive` |  OpenMP teams distribute parallel for directive.
  # `:omp_target_teams_directive` |  OpenMP target teams directive.
  # `:omp_target_teams_distribute_directive` |  OpenMP target teams distribute directive.
  # `:omp_target_teams_distribute_parallel_for_directive` |  OpenMP target teams distribute parallel for directive.
  # `:omp_target_teams_distribute_parallel_for_simd_directive` |  OpenMP target teams distribute parallel for simd directive.
  # `:omp_target_teams_distribute_simd_directive` |  OpenMP target teams distribute simd directive.
  # `:builtin_bit_cast_expr` |  C++2a std::bit_cast expression.
  # `:omp_master_task_loop_directive` |  OpenMP master taskloop directive.
  # `:omp_parallel_master_task_loop_directive` |  OpenMP parallel master taskloop directive.
  # `:omp_master_task_loop_simd_directive` |  OpenMP master taskloop simd directive.
  # `:omp_parallel_master_task_loop_simd_directive` |  OpenMP parallel master taskloop simd directive.
  # `:omp_parallel_master_directive` |  OpenMP parallel master directive.
  # `:omp_depobj_directive` |  OpenMP depobj directive.
  # `:omp_scan_directive` |  OpenMP scan directive.
  # `:last_stmt` | 
  # `:translation_unit` |  Cursor that represents the translation unit itself.
  # `:first_attr` | 
  # `:unexposed_attr` |  An attribute whose specific kind is not exposed via this interface.
  # `:ib_action_attr` | 
  # `:ib_outlet_attr` | 
  # `:ib_outlet_collection_attr` | 
  # `:cxx_final_attr` | 
  # `:cxx_override_attr` | 
  # `:annotate_attr` | 
  # `:asm_label_attr` | 
  # `:packed_attr` | 
  # `:pure_attr` | 
  # `:const_attr` | 
  # `:no_duplicate_attr` | 
  # `:cuda_constant_attr` | 
  # `:cuda_device_attr` | 
  # `:cuda_global_attr` | 
  # `:cuda_host_attr` | 
  # `:cuda_shared_attr` | 
  # `:visibility_attr` | 
  # `:dll_export` | 
  # `:dll_import` | 
  # `:ns_returns_retained` | 
  # `:ns_returns_not_retained` | 
  # `:ns_returns_autoreleased` | 
  # `:ns_consumes_self` | 
  # `:ns_consumed` | 
  # `:obj_c_exception` | 
  # `:obj_cns_object` | 
  # `:obj_c_independent_class` | 
  # `:obj_c_precise_lifetime` | 
  # `:obj_c_returns_inner_pointer` | 
  # `:obj_c_requires_super` | 
  # `:obj_c_root_class` | 
  # `:obj_c_subclassing_restricted` | 
  # `:obj_c_explicit_protocol_impl` | 
  # `:obj_c_designated_initializer` | 
  # `:obj_c_runtime_visible` | 
  # `:obj_c_boxable` | 
  # `:flag_enum` | 
  # `:convergent_attr` | 
  # `:warn_unused_attr` | 
  # `:warn_unused_result_attr` | 
  # `:aligned_attr` | 
  # `:last_attr` | 
  # `:preprocessing_directive` | 
  # `:macro_definition` | 
  # `:macro_expansion` | 
  # `:macro_instantiation` | 
  # `:inclusion_directive` | 
  # `:first_preprocessing` | 
  # `:last_preprocessing` | 
  # `:module_import_decl` |  A module import declaration.
  # `:type_alias_template_decl` | 
  # `:static_assert` |  A static_assert or _Static_assert node
  # `:friend_decl` |  a friend declaration.
  # `:first_extra_decl` | 
  # `:last_extra_decl` | 
  # `:overload_candidate` |  A code completion overload candidate.
  # @return [Enum]
  CursorKind = Enum.new

  ##
  # Describe the linkage of the entity referred to by a cursor.
  #
  # Member | Description
  # --- | ---
  # `:invalid` |  This value indicates that no linkage information is available for a provided CXCursor.
  # `:no_linkage` |  This is the linkage for variables, parameters, and so on that have automatic storage. This covers normal (non-extern) local variables.
  # `:internal` |  This is the linkage for static variables and static functions.
  # `:unique_external` |  This is the linkage for entities with external linkage that live in C++ anonymous namespaces.
  # `:external` |  This is the linkage for entities with true, external linkage.
  # @return [Enum]
  LinkageKind = Enum.new

  ##
  # Member | Description
  # --- | ---
  # `:invalid` |  This value indicates that no visibility information is available for a provided CXCursor.
  # `:hidden` |  Symbol not seen by the linker.
  # `:protected` |  Symbol seen by the linker but resolves to a symbol inside this object.
  # `:default` |  Symbol seen by the linker and acts like a normal symbol.
  # @return [Enum]
  VisibilityKind = Enum.new

  ##
  # Describe the "language" of the entity referred to by a cursor.
  #
  # Member | Description
  # --- | ---
  # `:invalid` | 
  # `:c` | 
  # `:obj_c` | 
  # `:c_plus_plus` | 
  # @return [Enum]
  LanguageKind = Enum.new

  ##
  # Describe the "thread-local storage (TLS) kind" of the declaration referred to by a cursor.
  #
  # Member | Description
  # --- | ---
  # `:none` | 
  # `:dynamic` | 
  # `:static` | 
  # @return [Enum]
  TLSKind = Enum.new

  ##
  # Describes the kind of type
  #
  # Member | Description
  # --- | ---
  # `:invalid` |  Represents an invalid type (e.g., where no type is available).
  # `:unexposed` |  A type whose specific kind is not exposed via this interface.
  # `:void` | 
  # `:bool` | 
  # `:char_u` | 
  # `:u_char` | 
  # `:char16` | 
  # `:char32` | 
  # `:u_short` | 
  # `:u_int` | 
  # `:u_long` | 
  # `:u_long_long` | 
  # `:u_int128` | 
  # `:char_s` | 
  # `:s_char` | 
  # `:w_char` | 
  # `:short` | 
  # `:int` | 
  # `:long` | 
  # `:long_long` | 
  # `:int128` | 
  # `:float` | 
  # `:double` | 
  # `:long_double` | 
  # `:null_ptr` | 
  # `:overload` | 
  # `:dependent` | 
  # `:obj_c_id` | 
  # `:obj_c_class` | 
  # `:obj_c_sel` | 
  # `:float128` | 
  # `:half` | 
  # `:float16` | 
  # `:short_accum` | 
  # `:accum` | 
  # `:long_accum` | 
  # `:u_short_accum` | 
  # `:u_accum` | 
  # `:u_long_accum` | 
  # `:b_float16` | 
  # `:first_builtin` | 
  # `:last_builtin` | 
  # `:complex` | 
  # `:pointer` | 
  # `:block_pointer` | 
  # `:l_value_reference` | 
  # `:r_value_reference` | 
  # `:record` | 
  # `:enum` | 
  # `:typedef` | 
  # `:obj_c_interface` | 
  # `:obj_c_object_pointer` | 
  # `:function_no_proto` | 
  # `:function_proto` | 
  # `:constant_array` | 
  # `:vector` | 
  # `:incomplete_array` | 
  # `:variable_array` | 
  # `:dependent_sized_array` | 
  # `:member_pointer` | 
  # `:auto` | 
  # `:elaborated` |  Represents a type that was referred to using an elaborated type keyword.
  # `:pipe` | 
  # `:ocl_image1d_ro` | 
  # `:ocl_image1d_array_ro` | 
  # `:ocl_image1d_buffer_ro` | 
  # `:ocl_image2d_ro` | 
  # `:ocl_image2d_array_ro` | 
  # `:ocl_image2d_depth_ro` | 
  # `:ocl_image2d_array_depth_ro` | 
  # `:ocl_image2d_msaaro` | 
  # `:ocl_image2d_array_msaaro` | 
  # `:ocl_image2d_msaa_depth_ro` | 
  # `:ocl_image2d_array_msaa_depth_ro` | 
  # `:ocl_image3d_ro` | 
  # `:ocl_image1d_wo` | 
  # `:ocl_image1d_array_wo` | 
  # `:ocl_image1d_buffer_wo` | 
  # `:ocl_image2d_wo` | 
  # `:ocl_image2d_array_wo` | 
  # `:ocl_image2d_depth_wo` | 
  # `:ocl_image2d_array_depth_wo` | 
  # `:ocl_image2d_msaawo` | 
  # `:ocl_image2d_array_msaawo` | 
  # `:ocl_image2d_msaa_depth_wo` | 
  # `:ocl_image2d_array_msaa_depth_wo` | 
  # `:ocl_image3d_wo` | 
  # `:ocl_image1d_rw` | 
  # `:ocl_image1d_array_rw` | 
  # `:ocl_image1d_buffer_rw` | 
  # `:ocl_image2d_rw` | 
  # `:ocl_image2d_array_rw` | 
  # `:ocl_image2d_depth_rw` | 
  # `:ocl_image2d_array_depth_rw` | 
  # `:ocl_image2d_msaarw` | 
  # `:ocl_image2d_array_msaarw` | 
  # `:ocl_image2d_msaa_depth_rw` | 
  # `:ocl_image2d_array_msaa_depth_rw` | 
  # `:ocl_image3d_rw` | 
  # `:ocl_sampler` | 
  # `:ocl_event` | 
  # `:ocl_queue` | 
  # `:ocl_reserve_id` | 
  # `:obj_c_object` | 
  # `:obj_c_type_param` | 
  # `:attributed` | 
  # `:ocl_intel_subgroup_avc_mce_payload` | 
  # `:ocl_intel_subgroup_avc_ime_payload` | 
  # `:ocl_intel_subgroup_avc_ref_payload` | 
  # `:ocl_intel_subgroup_avc_sic_payload` | 
  # `:ocl_intel_subgroup_avc_mce_result` | 
  # `:ocl_intel_subgroup_avc_ime_result` | 
  # `:ocl_intel_subgroup_avc_ref_result` | 
  # `:ocl_intel_subgroup_avc_sic_result` | 
  # `:ocl_intel_subgroup_avc_ime_result_single_ref_streamout` | 
  # `:ocl_intel_subgroup_avc_ime_result_dual_ref_streamout` | 
  # `:ocl_intel_subgroup_avc_ime_single_ref_streamin` | 
  # `:ocl_intel_subgroup_avc_ime_dual_ref_streamin` | 
  # `:ext_vector` | 
  # `:atomic` | 
  # @return [Enum]
  TypeKind = Enum.new

  ##
  # Describes the calling convention of a function type
  #
  # Member | Description
  # --- | ---
  # `:default` | 
  # `:c` | 
  # `:x86_std_call` | 
  # `:x86_fast_call` | 
  # `:x86_this_call` | 
  # `:x86_pascal` | 
  # `:aapcs` | 
  # `:aapcs_vfp` | 
  # `:x86_reg_call` | 
  # `:intel_ocl_bicc` | 
  # `:win64` | 
  # `:x86_64_win64` | 
  # `:x86_64_sys_v` | 
  # `:x86_vector_call` | 
  # `:swift` | 
  # `:preserve_most` | 
  # `:preserve_all` | 
  # `:a_arch64_vector_call` | 
  # `:invalid` | 
  # `:unexposed` | 
  # @return [Enum]
  CallingConv = Enum.new

  ##
  # Describes the kind of a template argument.
  #
  # Member | Description
  # --- | ---
  # `:null` | 
  # `:type` | 
  # `:declaration` | 
  # `:null_ptr` | 
  # `:integral` | 
  # `:template` | 
  # `:template_expansion` | 
  # `:expression` | 
  # `:pack` | 
  # `:invalid` | 
  # @return [Enum]
  TemplateArgumentKind = Enum.new

  ##
  # Member | Description
  # --- | ---
  # `:non_null` |  Values of this type can never be null.
  # `:nullable` |  Values of this type can be null.
  # `:unspecified` |  Whether values of this type can be null is (explicitly) unspecified. This captures a (fairly rare) case where we can't conclude anything about the nullability of the type even though it has been considered.
  # `:invalid` |  Nullability is not applicable to this type.
  # @return [Enum]
  TypeNullabilityKind = Enum.new

  ##
  # Member | Description
  # --- | ---
  # `:none` |  No ref-qualifier was provided.
  # `:l_value` |  An lvalue ref-qualifier was provided ( &).
  # `:r_value` |  An rvalue ref-qualifier was provided ( &&).
  # @return [Enum]
  RefQualifierKind = Enum.new

  ##
  # Represents the C++ access control level to a base class for a cursor with kind CX_CXXBaseSpecifier.
  #
  # Member | Description
  # --- | ---
  # `:cxx_invalid_access_specifier` | 
  # `:cxx_public` | 
  # `:cxx_protected` | 
  # `:cxx_private` | 
  # @return [Enum]
  CXXAccessSpecifier = Enum.new

  ##
  # Represents the storage classes as declared in the source. CX_SC_Invalid was added for the case that the passed cursor in not a declaration.
  #
  # Member | Description
  # --- | ---
  # `:invalid` | 
  # `:none` | 
  # `:extern` | 
  # `:static` | 
  # `:private_extern` | 
  # `:open_cl_work_group_local` | 
  # `:auto` | 
  # `:register` | 
  # @return [Enum]
  StorageClass = Enum.new

  ##
  # Describes how the traversal of the children of a particular cursor should proceed after visiting a particular child cursor.
  #
  # Member | Description
  # --- | ---
  # `:break` |  Terminates the cursor traversal.
  # `:continue` |  Continues the cursor traversal with the next sibling of the cursor just visited, without visiting its children.
  # `:recurse` |  Recursively traverse the children of this cursor, using the same visitor and client data.
  # @return [Enum]
  ChildVisitResult = Enum.new

  ##
  # Properties for the printing policy.
  #
  # Member | Description
  # --- | ---
  # `:indentation` | 
  # `:suppress_specifiers` | 
  # `:suppress_tag_keyword` | 
  # `:include_tag_definition` | 
  # `:suppress_scope` | 
  # `:suppress_unwritten_scope` | 
  # `:suppress_initializers` | 
  # `:constant_array_size_as_written` | 
  # `:anonymous_tag_locations` | 
  # `:suppress_strong_lifetime` | 
  # `:suppress_lifetime_qualifiers` | 
  # `:suppress_template_args_in_cxx_constructors` | 
  # `:bool` | 
  # `:restrict` | 
  # `:alignof` | 
  # `:underscore_alignof` | 
  # `:use_void_for_zero_params` | 
  # `:terse_output` | 
  # `:polish_for_declaration` | 
  # `:half` | 
  # `:msw_char` | 
  # `:include_newlines` | 
  # `:msvc_formatting` | 
  # `:constants_as_written` | 
  # `:suppress_implicit_base` | 
  # `:fully_qualified_name` | 
  # `:last_property` | 
  # @return [Enum]
  PrintingPolicyProperty = Enum.new

  ##
  # Property attributes for a CXCursor_ObjCPropertyDecl.
  #
  # Member | Description
  # --- | ---
  # `:noattr` | 
  # `:readonly` | 
  # `:getter` | 
  # `:assign` | 
  # `:readwrite` | 
  # `:retain` | 
  # `:copy` | 
  # `:nonatomic` | 
  # `:setter` | 
  # `:atomic` | 
  # `:weak` | 
  # `:strong` | 
  # `:cxobjcpropertyattr_unsafe_unretained` | 
  # `:class` | 
  # @return [Enum]
  ObjCPropertyAttrKind = Enum.new

  ##
  # 'Qualifiers' written next to the return and parameter types in Objective-C method declarations.
  #
  # Member | Description
  # --- | ---
  # `:none` | 
  # `:in` | 
  # `:inout` | 
  # `:out` | 
  # `:bycopy` | 
  # `:byref` | 
  # `:oneway` | 
  # @return [Enum]
  ObjCDeclQualifierKind = Enum.new

  ##
  # Member | Description
  # --- | ---
  # `:want_qualifier` |  Include the nested-name-specifier, e.g. Foo:: in x.Foo::y, in the range.
  # `:want_template_args` |  Include the explicit template arguments, e.g. <int> in x.f<int>, in the range.
  # `:want_single_piece` |  If the name is non-contiguous, return the full spanning range.
  # @return [Enum]
  NameRefFlags = Enum.new

  ##
  # Describes a kind of token.
  #
  # Member | Description
  # --- | ---
  # `:punctuation` |  A token that contains some kind of punctuation.
  # `:keyword` |  A language keyword.
  # `:identifier` |  An identifier (that is not a keyword).
  # `:literal` |  A numeric, string, or character literal.
  # `:comment` |  A comment.
  # @return [Enum]
  TokenKind = Enum.new

  ##
  # Describes a single piece of text within a code-completion string.
  #
  # Member | Description
  # --- | ---
  # `:optional` |  A code-completion string that describes "optional" text that could be a part of the template (but is not required).
  # `:typed_text` |  Text that a user would be expected to type to get this code-completion result.
  # `:text` |  Text that should be inserted as part of a code-completion result.
  # `:placeholder` |  Placeholder text that should be replaced by the user.
  # `:informative` |  Informative text that should be displayed but never inserted as part of the template.
  # `:current_parameter` |  Text that describes the current parameter when code-completion is referring to function call, message send, or template specialization.
  # `:left_paren` |  A left parenthesis ('('), used to initiate a function call or signal the beginning of a function parameter list.
  # `:right_paren` |  A right parenthesis (')'), used to finish a function call or signal the end of a function parameter list.
  # `:left_bracket` |  A left bracket ('[').
  # `:right_bracket` |  A right bracket (']').
  # `:left_brace` |  A left brace ('{').
  # `:right_brace` |  A right brace ('}').
  # `:left_angle` |  A left angle bracket ('<').
  # `:right_angle` |  A right angle bracket ('>').
  # `:comma` |  A comma separator (',').
  # `:result_type` |  Text that specifies the result type of a given result.
  # `:colon` |  A colon (':').
  # `:semi_colon` |  A semicolon (';').
  # `:equal` |  An '=' sign.
  # `:horizontal_space` |  Horizontal space (' ').
  # `:vertical_space` |  Vertical space ('\n'), after which it is generally a good idea to perform indentation.
  # @return [Enum]
  CompletionChunkKind = Enum.new

  ##
  # Flags that can be passed to clang_codeCompleteAt() to modify its behavior.
  #
  # Member | Description
  # --- | ---
  # `:include_macros` |  Whether to include macros within the set of code completions returned.
  # `:include_code_patterns` |  Whether to include code patterns for language constructs within the set of code completions, e.g., for loops.
  # `:include_brief_comments` |  Whether to include brief documentation within the set of code completions returned.
  # `:skip_preamble` |  Whether to speed up completion by omitting top- or namespace-level entities defined in the preamble. There's no guarantee any particular entity is omitted. This may be useful if the headers are indexed externally.
  # `:include_completions_with_fix_its` |  Whether to include completions with small fix-its, e.g. change '.' to '->' on member access, etc.
  # @return [Enum]
  CodeCompleteFlags = Enum.new

  ##
  # Bits that represent the context under which completion is occurring.
  #
  # Member | Description
  # --- | ---
  # `:unexposed` |  The context for completions is unexposed, as only Clang results should be included. (This is equivalent to having no context bits set.)
  # `:any_type` |  Completions for any possible type should be included in the results.
  # `:any_value` |  Completions for any possible value (variables, function calls, etc.) should be included in the results.
  # `:obj_c_object_value` |  Completions for values that resolve to an Objective-C object should be included in the results.
  # `:obj_c_selector_value` |  Completions for values that resolve to an Objective-C selector should be included in the results.
  # `:cxx_class_type_value` |  Completions for values that resolve to a C++ class type should be included in the results.
  # `:dot_member_access` |  Completions for fields of the member being accessed using the dot operator should be included in the results.
  # `:arrow_member_access` |  Completions for fields of the member being accessed using the arrow operator should be included in the results.
  # `:obj_c_property_access` |  Completions for properties of the Objective-C object being accessed using the dot operator should be included in the results.
  # `:enum_tag` |  Completions for enum tags should be included in the results.
  # `:union_tag` |  Completions for union tags should be included in the results.
  # `:struct_tag` |  Completions for struct tags should be included in the results.
  # `:class_tag` |  Completions for C++ class names should be included in the results.
  # `:namespace` |  Completions for C++ namespaces and namespace aliases should be included in the results.
  # `:nested_name_specifier` |  Completions for C++ nested name specifiers should be included in the results.
  # `:obj_c_interface` |  Completions for Objective-C interfaces (classes) should be included in the results.
  # `:obj_c_protocol` |  Completions for Objective-C protocols should be included in the results.
  # `:obj_c_category` |  Completions for Objective-C categories should be included in the results.
  # `:obj_c_instance_message` |  Completions for Objective-C instance messages should be included in the results.
  # `:obj_c_class_message` |  Completions for Objective-C class messages should be included in the results.
  # `:obj_c_selector_name` |  Completions for Objective-C selector names should be included in the results.
  # `:macro_name` |  Completions for preprocessor macro names should be included in the results.
  # `:natural_language` |  Natural language completions should be included in the results.
  # `:included_file` |  #include file completions should be included in the results.
  # `:unknown` |  The current context is unknown, so set all contexts.
  # @return [Enum]
  CompletionContext = Enum.new

  ##
  # Member | Description
  # --- | ---
  # `:break` | 
  # `:continue` | 
  # @return [Enum]
  VisitorResult = Enum.new

  ##
  # Member | Description
  # --- | ---
  # `:success` |  Function returned successfully.
  # `:invalid` |  One of the parameters was invalid for the function.
  # `:visit_break` |  The function was terminated by a callback (e.g. it returned CXVisit_Break)
  # @return [Enum]
  Result = Enum.new

end
