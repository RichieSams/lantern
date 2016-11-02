include(SetSourceGroup)

SetSourceGroup(NAME Root
	PREFIX LMF_COMPILER
	SOURCE_FILES main.cpp
)

# Create exe
add_executable(lmf_compiler ${LMF_COMPILER_ROOT})
target_link_libraries(lmf_compiler lantern_core)
