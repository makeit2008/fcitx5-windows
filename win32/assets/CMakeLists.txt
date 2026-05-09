set(PENGUIN_SVG "${CMAKE_CURRENT_SOURCE_DIR}/penguin.svg")
set(PENGUIN_ICO "${PROJECT_BINARY_DIR}/dll/penguin.ico")

# Try to find ImageMagick for SVG to ICO conversion
find_program(MAGICK_CONVERT magick)

if(MAGICK_CONVERT)
    add_custom_command(
        OUTPUT "${PENGUIN_ICO}"
        COMMAND ${MAGICK_CONVERT} -background none "${PENGUIN_SVG}" "${PENGUIN_ICO}"
        DEPENDS "${PENGUIN_SVG}"
    )
    add_custom_target(GenerateIco ALL
        DEPENDS "${PENGUIN_ICO}"
    )
else()
    # Fallback: if magick is not available, warn the user
    message(WARNING "ImageMagick not found. Icon generation skipped. Please install ImageMagick or provide penguin.ico manually.")
    add_custom_target(GenerateIco ALL)
endif()
