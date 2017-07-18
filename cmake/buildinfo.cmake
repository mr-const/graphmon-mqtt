function (generate_build_info infile outfile)
	# Get the current working branch
	execute_process(
	  COMMAND git rev-parse --abbrev-ref HEAD
	  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	  OUTPUT_VARIABLE GIT_BRANCH
	  OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	# Get the latest abbreviated commit hash of the working branch
	execute_process(
	  COMMAND git log -1 --format=%h
	  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	  OUTPUT_VARIABLE GIT_COMMIT_HASH
	  OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	if (NOT DEFINED ENV{BUILD_NUMBER})
		set (BUILD_NUMBER 0)
	else()
		set (BUILD_NUMBER $ENV{BUILD_NUMBER})
	endif()

	string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")

	set (GIT_BRANCH ${GIT_BRANCH})
	set (GIT_COMMIT_HASH ${GIT_COMMIT_HASH})
	set (BUILD_TIMESTAMP ${BUILD_TIMESTAMP})


    configure_file (
    	${infile}
    	${outfile})

endfunction()