# Configure strict warning flags per compiler.
function(stable_fluids_set_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE
            /W4 /permissive- /Zc:__cplusplus
        )
    else()
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic
            -Wshadow -Wnon-virtual-dtor
            -Wold-style-cast -Wcast-align
            -Woverloaded-virtual -Wconversion -Wsign-conversion
            -Wnull-dereference -Wdouble-promotion
            -Wformat=2
        )
    endif()

    if(STABLE_FLUIDS_WARNINGS_AS_ERRORS)
        if(MSVC)
            target_compile_options(${target} PRIVATE /WX)
        else()
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()
endfunction()
