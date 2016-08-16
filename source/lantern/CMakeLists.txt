include(SetSourceGroup)

SetSourceGroup(NAME Root
	PREFIX LANTERN
	SOURCE_FILES main.cpp
)

# Create exe
add_executable(lantern ${LANTERN_ROOT})
target_link_libraries(lantern lantern_core embree ${TBB_LIBRARIES})
