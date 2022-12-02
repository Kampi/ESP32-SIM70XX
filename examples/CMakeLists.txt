# Read all source files from the 'src' directory.
FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

# Register all source files.
idf_component_register(SRCS ${app_sources}
                       REQUIRES json)

# Include the files to the build system.
target_add_binary_data(${COMPONENT_TARGET} "files/ISRG_Root_X1.pem" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "files/ImageSmall.jpg" BINARY)
target_add_binary_data(${COMPONENT_TARGET} "files/ImageVeryLarge.jpg" BINARY)