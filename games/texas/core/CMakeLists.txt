aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SUB_SOURCES)

set(TEXAS_SRCS
    ${TEXAS_SRCS}
    ${SUB_SOURCES}
    PARENT_SCOPE
)
