# Adding compile definitions
target_compile_definitions("raylib" PUBLIC "${PLATFORM_CPP}")
target_compile_definitions("raylib" PUBLIC "${GRAPHICS}")
if (VULKAN_EXCLUSIVE)
    message(STATUS "Compiling Vulkan exclusive mode")
    target_compile_definitions("raylib" PUBLIC "VULKAN_EXCLUSIVE")
endif ()

function(define_if target variable)
    if(${${variable}})
        message(STATUS "${variable}=${${variable}}")
        target_compile_definitions(${target} PRIVATE "${variable}")
    endif()
endfunction()

if(${CUSTOMIZE_BUILD})
    target_compile_definitions("raylib" PRIVATE EXTERNAL_CONFIG_FLAGS)

    foreach(FLAG IN LISTS CONFIG_HEADER_FLAGS)
        string(REGEX MATCH "([^=]+)=(.+)" _ ${FLAG})
        define_if("raylib" ${CMAKE_MATCH_1})
    endforeach()

    foreach(VALUE IN LISTS CONFIG_HEADER_VALUES)
        target_compile_definitions("raylib" PRIVATE ${VALUE})
    endforeach()
endif()
