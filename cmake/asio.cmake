include(FetchContent)

FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        GIT_TAG        asio-1-24-0
)
FetchContent_GetProperties(asio)
if (NOT asio_POPULATED)
    FetchContent_Populate(asio)
endif ()
add_library(asio INTERFACE)

target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
target_compile_definitions(asio INTERFACE ASIO_HEADER_ONLY ASIO_STANDALONE ASIO_NO_DEPRECATED ASIO_NO_TYPEID ASIO_NO_TS_EXECUTORS)