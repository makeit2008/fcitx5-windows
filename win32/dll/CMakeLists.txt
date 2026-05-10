# Determine the library name based on architecture
if(ARCH STREQUAL "AMD64")
    set(FCITX5_DLL_NAME fcitx5-x86_64)
elseif(ARCH STREQUAL "ARM64")
    set(FCITX5_DLL_NAME fcitx5-arm64)
else()
    set(FCITX5_DLL_NAME fcitx5-x86_64)
endif()

add_library(${FCITX5_DLL_NAME} SHARED
    main.cpp
    register.cpp
    util.cpp
)

# Add warning suppression for COM-related code (for GCC/clang only)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(${FCITX5_DLL_NAME} PRIVATE "-Wno-dll-attribute-on-redeclaration")
endif()

# Link with necessary Windows libraries
target_link_libraries(${FCITX5_DLL_NAME}
    tsf
    ole32
    oleaut32
    uuid
)

# Add include directories for tsf
target_include_directories(${FCITX5_DLL_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/win32/tsf)

# Copy icon dependency
add_dependencies(${FCITX5_DLL_NAME} GenerateIco)
