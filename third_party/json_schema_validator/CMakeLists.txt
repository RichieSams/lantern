project(json_schema_validator CXX)

cmake_minimum_required(VERSION 3.2)

add_library(json_schema_validator
	include/json_schema_validator.hpp
	src/json-schema-draft4.json.cpp
	src/json-uri.cpp
	src/json-validator.cpp
)
target_include_directories(json_schema_validator PUBLIC include)

target_link_libraries(json_schema_validator json)
