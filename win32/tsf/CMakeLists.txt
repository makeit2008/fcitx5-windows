add_library(tsf STATIC
    tsf.cpp
    TextInputProcessorEx.cpp
    ThreadMgrEventSink.cpp
    TextEditSink.cpp
    KeyEventSink.cpp
    EditSession.cpp
    CompositionSink.cpp
)

# Add Windows-specific compile definitions
target_compile_definitions(tsf PRIVATE
    _WINDOWS
    _CRT_SECURE_NO_WARNINGS
    _ATL_NO_AUTOMATIC_NAMESPACE
)

# Add warning suppression for COM-related code
target_compile_options(tsf PRIVATE
    -Wno-reorder-ctor
    -Wno-microsoft-cast
)
