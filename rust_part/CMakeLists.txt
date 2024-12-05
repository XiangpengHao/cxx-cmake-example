if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CARGO_CMD cargo build --verbose)
    set(TARGET_DIR "debug")
else ()
    set(CARGO_CMD cargo build --release --verbose)
    set(TARGET_DIR "release")
endif ()

# If the user is cross compiling to a different target, update the location of the target directory
# *Note*: Make sure to install the target with rustup, as well as setting the proper environment variables, such as CC, HOST_CC, CARGO_TARGET_<triple>_LINKER, ...
# See https://doc.rust-lang.org/cargo/reference/environment-variables.html#configuration-environment-variables
if (DEFINED ENV{CARGO_BUILD_TARGET})
    set(TARGET_DIR "$ENV{CARGO_BUILD_TARGET}/${TARGET_DIR}")
endif()

if(ENABLE_LTO)
    set(RUST_FLAGS "-Clinker-plugin-lto" "-Clinker=clang-17" "-Clink-arg=-fuse-ld=lld-17")
endif()

set(RUST_PART_LIB "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_DIR}/librust_part.a")

set(RUST_PART_CXX "${CMAKE_CURRENT_BINARY_DIR}/rust_part.cpp")
add_library(rust_part STATIC ${RUST_PART_CXX})
add_custom_command(
    OUTPUT ${RUST_PART_CXX}
    COMMAND CARGO_TARGET_DIR=${CMAKE_CURRENT_BINARY_DIR} RUSTFLAGS="${RUST_FLAGS}" ${CARGO_CMD}
    COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/cxxbridge/rust_part/src/lib.rs.cc ${RUST_PART_CXX}
    COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/cxxbridge/rust_part/src/lib.rs.h ${CMAKE_CURRENT_BINARY_DIR}/rust_part.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(rust_part pthread dl ${RUST_PART_LIB})

add_test(NAME rust_part_test 
    COMMAND cargo test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

