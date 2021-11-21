cmake_minimum_required(VERSION 3.19)
project(avakar.mutest)

# This is to allow mutest to be brought in via `add_subdirectory`
# in addition to `FetchContent`: we add a fake `FetchContent` declaration
# and ensure it is populated. In case we're not the selected mutest version,
# we include the selected one.
include(FetchContent)
FetchContent_Declare(avakar.mutest SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
FetchContent_GetProperties(avakar.mutest)
if (NOT avakar.mutest_POPULATED)
	FetchContent_Populate(avakar.mutest)
	if (NOT "${avakar.mutest_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
		add_subdirectory("${avakar.mutest_SOURCE_DIR}" "${avakar.mutest_BINARY_DIR}")
	endif()
endif()

# Check if another version of mutest is already included in the project.
if (COMMAND add_mutest)
	return()
endif()

add_library(avakar.mutest
	include/avakar/mutest.h
	include/avakar/mutest_main.h
	src/main.cpp
	)
target_include_directories(avakar.mutest PUBLIC include)
target_compile_features(avakar.mutest PUBLIC cxx_std_11)
if (MSVC)
	target_compile_options(avakar.mutest PUBLIC /W4)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(avakar.mutest PUBLIC -Wall)
endif()

add_library(avakar::mutest ALIAS avakar.mutest)

set(MUTEST_PROJECTS "${CMAKE_PROJECT_NAME}"
	CACHE STRING "The list of projects to run mutests for")

add_library(_avakar_mutest_opts INTERFACE)

function(add_mutest)
	set(enabled NO)
	foreach(pattern IN ITEMS "${MUTEST_PROJECTS}")
		if("${PROJECT_NAME}" MATCHES "${pattern}")
			set(enabled YES)
			break()
		endif()
	endforeach()

	if (NOT enabled)
		return()
	endif()

	foreach(arg IN LISTS ARGV)
		if (TARGET "${arg}")
			target_link_libraries(_avakar_mutest_opts INTERFACE "${arg}")
		else()
			if (IS_ABSOLUTE "${arg}")
				set(src_path "${arg}")
			else()
				set(src_path "${CMAKE_CURRENT_SOURCE_DIR}/${arg}")
			endif()

			if (NOT EXISTS "${src_path}")
				message(SEND_ERROR "Test source file not found: ${src_path}")
			endif()

			if (NOT TARGET "mutests")
				add_executable(mutests "${src_path}")
				target_link_libraries(mutests PRIVATE _avakar_mutest_opts avakar::mutest)
			else()
				target_sources(mutests PRIVATE "${src_path}")
			endif()
		endif()
	endforeach()
endfunction()

function(_mutest_finalize)
	if (TARGET "mutests")
		if("${CMAKE_TESTING_ENABLED}")
			add_test(NAME mutests COMMAND mutests)
		else()
			add_custom_target(RUN_MUTESTS ALL mutests DEPENDS mutests)
		endif()
	endif()
endfunction()

cmake_language(DEFER DIRECTORY "${CMAKE_SOURCE_DIR}" CALL _mutest_finalize)

add_mutest(test/test.cpp)
