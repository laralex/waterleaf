function(extended_target_link_libraries name link_against)
	string (REPLACE " " ";" link_against "${link_against}")
	foreach (lib ${link_against})
		target_link_libraries(${name} PUBLIC ${lib})
	endforeach()
endfunction()

function(wlf_target_properties name folder cxx_flags)
	set_target_properties(${name} PROPERTIES
		FOLDER ${folder}
		COMPILE_FLAGS "${cxx_flags}"
		DEBUG_POSTFIX "_d"
		RUNTIME_OUTPUT_DIRECTORY "${WLF_BINARY_DIR}"
		PDB_OUTPUT_DIRECTORY "${WLF_BINARY_DIR}"
		LIBRARY_OUTPUT_DIRECTORY "${WLF_ARTIFACTS_DIR}"
		ARCHIVE_OUTPUT_DIRECTORY "${WLF_ARTIFACTS_DIR}")
endfunction()

function(make_library name lib_type cxx_flags link_against)
	add_library(${name} ${lib_type} ${ARGN})
	add_library(${cmake_package_name}::${name} ALIAS ${name})
	wlf_target_properties(${name} "libs" "${cxx_flags}")
	extended_target_link_libraries(${name} "${link_against}")
	target_include_directories(${name} PUBLIC "include" PRIVATE "include/${name}")
endfunction()

function(make_executable name cxx_flags link_against)
	add_executable(${name} ${ARGN})
	wlf_target_properties(${name} "apps" "${cxx_flags}")
	extended_target_link_libraries(${name} "${link_against}")
endfunction()