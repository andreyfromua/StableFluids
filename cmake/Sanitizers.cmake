# Enable supported sanitizers in debug builds.
function(stable_fluids_enable_sanitizers target)
    if(NOT STABLE_FLUIDS_ENABLE_SANITIZERS)
        return()
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        set(_flags -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_compile_options(${target} PRIVATE ${_flags})
        target_link_options(${target} PRIVATE ${_flags})
    endif()
endfunction()
