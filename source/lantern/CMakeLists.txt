set(LANTERN_ROOT
	main.cpp
)

# Create exe
add_executable(lantern ${LANTERN_ROOT})
target_link_libraries(lantern lantern_core)
