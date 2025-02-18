option(ENABLE_ASAN "Enable AddressSanitizer" NO)

if(MSVC)
    if(ENABLE_ASAN)
        string(REPLACE "/RTC1" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
        string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
        add_compile_options(/fsanitize=address /fsanitize-address-use-after-return)
    endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    option(ENABLE_LSAN "Enable LeakSanitizer" NO)
    option(ENABLE_TSAN "Enable ThreadSanitizer" NO)
    option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" NO)
    if(NOT APPLE)
        option(ENABLE_MSAN "Enable MemorySanitizer" NO)
    endif()

    if (ENABLE_ASAN OR ENABLE_LSAN OR ENABLE_MSAN OR ENABLE_TSAN OR ENABLE_UBSAN)
        add_compile_options(
            -fno-omit-frame-pointer
            $<$<BOOL:${ENABLE_ASAN}>:-fsanitize=address>
            $<$<BOOL:${ENABLE_LSAN}>:-fsanitize=leak>
            $<$<BOOL:${ENABLE_MSAN}>:-fsanitize=memory>
            $<$<BOOL:${ENABLE_TSAN}>:-fsanitize=thread>
            $<$<BOOL:${ENABLE_UBSAN}>:-fsanitize=undefined>
        )
    endif()
endif()
