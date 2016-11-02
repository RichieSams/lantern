# Generate main projects

# Core
include_directories(core)
add_subdirectory(core)

# Lantern renderer
add_subdirectory(lantern)


# Generate projects for the tools

# LMF Compiler
add_subdirectory(lmf_compiler)
