add_library(tsf STATIC
    tsf.cpp
    TextInputProcessorEx.cpp
    ThreadMgrEventSink.cpp
    TextEditSink.cpp
    KeyEventSink.cpp
    EditSession.cpp
    CompositionSink.cpp
    Dictionary.cpp
    InputEngine.cpp
    CandidateWindow.cpp
)

# Add Windows-specific compile definitions
target_compile_definitions(tsf PRIVATE
    _WINDOWS
    _CRT_SECURE_NO_WARNINGS
)

# Add warning suppression for COM-related code (for GCC/clang only)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(tsf PRIVATE
        -Wno-reorder-ctor
        -Wno-microsoft-cast
    )
endif()
