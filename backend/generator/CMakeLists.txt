cmake_minimum_required(VERSION 3.10)
project(generator VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES_DIR sources)
set(INCLUDE_DIR include)

set(SOURCES 
	${SOURCES_DIR}/generator.cpp
	${SOURCES_DIR}/fillers.cpp
	${SOURCES_DIR}/factory.cpp
)

set(HEADERS 
	${INCLUDE_DIR}/generator.h
)

add_library(
	${PROJECT_NAME} 
	STATIC 
	${SOURCES}
	${HEADERS}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})

