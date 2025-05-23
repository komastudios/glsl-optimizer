if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fwrapv -fno-strict-aliasing")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fwrapv -fno-strict-aliasing -fno-exceptions -fno-rtti")
    add_compile_options(-Wno-register)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    string(REGEX REPLACE "/GR" "/GR-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/EHsc" "/EHs-c-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    add_definitions(-D_HAS_EXCEPTIONS=0) # for STL
endif()

set(glcpp-library_sources
    glsl/glcpp/glcpp-parse.c
    glsl/glcpp/pp.c
    glsl/glcpp/glcpp-lex.c
    util/ralloc.c
    util/hash_table.c
)

set(mesa_sources
    mesa/program/hash_table.h
    mesa/program/prog_hash_table.c
    mesa/program/prog_instruction.h
    mesa/program/prog_parameter.h
    mesa/program/prog_statevars.h
    mesa/program/symbol_table.c
    mesa/program/symbol_table.h
    mesa/main/compiler.h
    mesa/main/config.h
    mesa/main/context.h
    mesa/main/core.h
    mesa/main/dd.h
    mesa/main/errors.h
    mesa/main/glheader.h
    mesa/main/glminimal.h
    mesa/main/imports.c
    mesa/main/imports.h
    mesa/main/macros.h
    mesa/main/mtypes.h
    mesa/main/simple_list.h
)

set(glsl_sources
    glsl/ast.h
    glsl/ast_array_index.cpp
    glsl/ast_expr.cpp
    glsl/ast_function.cpp
    glsl/ast_to_hir.cpp
    glsl/ast_type.cpp
    glsl/builtin_functions.cpp
    glsl/builtin_type_macros.h
    glsl/builtin_types.cpp
    glsl/builtin_variables.cpp
    glsl/glcpp/glcpp-lex.c
    glsl/glcpp/glcpp-parse.c
    glsl/glcpp/glcpp-parse.h
    glsl/glcpp/glcpp.h
    glsl/glcpp/pp.c
    glsl/glsl_lexer.cpp
    glsl/glsl_optimizer.cpp
    glsl/glsl_optimizer.h
    glsl/glsl_parser.cpp
    glsl/glsl_parser.h
    glsl/glsl_parser_extras.cpp
    glsl/glsl_parser_extras.h
    glsl/glsl_symbol_table.cpp
    glsl/glsl_symbol_table.h
    glsl/glsl_types.cpp
    glsl/glsl_types.h
    glsl/hir_field_selection.cpp
    glsl/ir.cpp
    glsl/ir.h
    glsl/ir_basic_block.cpp
    glsl/ir_basic_block.h
    glsl/ir_builder.cpp
    glsl/ir_builder.h
    glsl/ir_clone.cpp
    glsl/ir_constant_expression.cpp
    glsl/ir_equals.cpp
    glsl/ir_expression_flattening.cpp
    glsl/ir_expression_flattening.h
    glsl/ir_function.cpp
    glsl/ir_function_can_inline.cpp
    glsl/ir_function_detect_recursion.cpp
    glsl/ir_function_inlining.h
    glsl/ir_hierarchical_visitor.cpp
    glsl/ir_hierarchical_visitor.h
    glsl/ir_hv_accept.cpp
    glsl/ir_import_prototypes.cpp
    glsl/ir_optimization.h
    glsl/ir_print_glsl_visitor.cpp
    glsl/ir_print_glsl_visitor.h
    glsl/ir_print_metal_visitor.cpp
    glsl/ir_print_metal_visitor.h
    glsl/ir_print_visitor.cpp
    glsl/ir_print_visitor.h
    glsl/ir_rvalue_visitor.cpp
    glsl/ir_rvalue_visitor.h
    glsl/ir_stats.cpp
    glsl/ir_stats.h
    glsl/ir_uniform.h
    glsl/ir_unused_structs.cpp
    glsl/ir_unused_structs.h
    glsl/ir_validate.cpp
    glsl/ir_variable_refcount.cpp
    glsl/ir_variable_refcount.h
    glsl/ir_visitor.h
    glsl/link_atomics.cpp
    glsl/link_functions.cpp
    glsl/link_interface_blocks.cpp
    glsl/link_uniform_block_active_visitor.cpp
    glsl/link_uniform_block_active_visitor.h
    glsl/link_uniform_blocks.cpp
    glsl/link_uniform_initializers.cpp
    glsl/link_uniforms.cpp
    glsl/link_varyings.cpp
    glsl/link_varyings.h
    glsl/linker.cpp
    glsl/linker.h
    glsl/list.h
    glsl/loop_analysis.cpp
    glsl/loop_analysis.h
    glsl/loop_controls.cpp
    glsl/loop_unroll.cpp
    glsl/lower_clip_distance.cpp
    glsl/lower_discard.cpp
    glsl/lower_discard_flow.cpp
    glsl/lower_if_to_cond_assign.cpp
    glsl/lower_instructions.cpp
    glsl/lower_jumps.cpp
    glsl/lower_mat_op_to_vec.cpp
    glsl/lower_named_interface_blocks.cpp
    glsl/lower_noise.cpp
    glsl/lower_offset_array.cpp
    glsl/lower_output_reads.cpp
    glsl/lower_packed_varyings.cpp
    glsl/lower_packing_builtins.cpp
    glsl/lower_ubo_reference.cpp
    glsl/lower_variable_index_to_cond_assign.cpp
    glsl/lower_vec_index_to_cond_assign.cpp
    glsl/lower_vec_index_to_swizzle.cpp
    glsl/lower_vector.cpp
    glsl/lower_vector_insert.cpp
    glsl/lower_vertex_id.cpp
    glsl/opt_algebraic.cpp
    glsl/opt_array_splitting.cpp
    glsl/opt_constant_folding.cpp
    glsl/opt_constant_propagation.cpp
    glsl/opt_constant_variable.cpp
    glsl/opt_copy_propagation.cpp
    glsl/opt_copy_propagation_elements.cpp
    glsl/opt_cse.cpp
    glsl/opt_dead_builtin_variables.cpp
    glsl/opt_dead_builtin_varyings.cpp
    glsl/opt_dead_code.cpp
    glsl/opt_dead_code_local.cpp
    glsl/opt_dead_functions.cpp
    glsl/opt_flatten_nested_if_blocks.cpp
    glsl/opt_flip_matrices.cpp
    glsl/opt_function_inlining.cpp
    glsl/opt_if_simplification.cpp
    glsl/opt_minmax.cpp
    glsl/opt_noop_swizzle.cpp
    glsl/opt_rebalance_tree.cpp
    glsl/opt_redundant_jumps.cpp
    glsl/opt_structure_splitting.cpp
    glsl/opt_swizzle_swizzle.cpp
    glsl/opt_tree_grafting.cpp
    glsl/opt_vectorize.cpp
    glsl/program.h
    glsl/s_expression.cpp
    glsl/s_expression.h
    glsl/standalone_scaffolding.cpp
    glsl/standalone_scaffolding.h
    glsl/strtod.c
    glsl/strtod.h
)

add_library(glsl_optimizer
    ${glcpp-library_sources}
    ${mesa_sources}
    ${glsl_sources}
)
target_include_directories(glsl_optimizer PRIVATE . ../include mesa)
target_include_directories(glsl_optimizer PUBLIC glsl)

if (GLSL_OPTIMIZER_EXTRACT_SYMBOLS)
    target_extract_symbols(glsl_optimizer "${CMAKE_CURRENT_BINARY_DIR}/glsl_optimizer.symbols" "${CMAKE_SOURCE_DIR}")
endif ()

if (GLSL_OPTIMIZER_TOOLS)
    add_executable(glsl_compiler glsl/main.cpp)
    target_link_libraries(glsl_compiler PRIVATE glsl_optimizer)
endif ()
