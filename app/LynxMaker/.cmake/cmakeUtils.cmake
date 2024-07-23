function(get_current_date result)
    if(NOT result)
        message(FATAL_ERROR "Variable 'result' not provided to get_current_date function.")
    endif()

	execute_process(
		COMMAND powershell -Command "Get-Date -Format 'dd/MM/yyyy - HH:mm:ss'"
		OUTPUT_VARIABLE output
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
    set(${result} ${output} PARENT_SCOPE)
endfunction()

function(get_commit_date output_variable)
    execute_process(
        COMMAND git -C ${CMAKE_SOURCE_DIR} log -1 --format=%cd --date=format:%d/%m/%Y\ -\ %H:%M:%S
        OUTPUT_VARIABLE last_commit_date
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )	
    set(${output_variable} ${last_commit_date} PARENT_SCOPE)
endfunction()

function(get_commit_author output_variable)
    execute_process(
        COMMAND git -C ${CMAKE_SOURCE_DIR} log -1 --format=%ae
        OUTPUT_VARIABLE last_commit_author
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
	
	string(REPLACE "\\" "/" last_commit_author ${last_commit_author})
    string(REPLACE "\"" "" last_commit_author ${last_commit_author})
    set(${output_variable} "${last_commit_author}" PARENT_SCOPE)
endfunction()

function(get_git_branch output_variable)
    execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE branch
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    set(${output_variable} ${branch} PARENT_SCOPE)
endfunction()

function(get_compiler_user output_variable)
    execute_process(
        COMMAND "whoami"
        OUTPUT_VARIABLE compiler_user
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
	string(REPLACE "\\" "/" compiler_user ${compiler_user})
	string(REPLACE "\"" "" compiler_user ${compiler_user})
    set(${output_variable} "${compiler_user}" PARENT_SCOPE)
endfunction()

function(get_compiler_name output_var)
    if(CMAKE_C_COMPILER_ID)
        set(${output_var} ${CMAKE_C_COMPILER_ID} PARENT_SCOPE)
    elseif(CMAKE_CXX_COMPILER_ID)
        set(${output_var} ${CMAKE_CXX_COMPILER_ID} PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Nom du compilateur non trouvé.")
    endif()
endfunction()

function(setup_version)
	
	get_commit_date(COMMIT_DATE)
	set(COMMIT_DATE ${COMMIT_DATE})
	
	get_current_date(COMPILATION_DATE)
	set(COMPILATION_DATE ${COMPILATION_DATE})

	get_commit_author(COMMIT_AUTHOR)
	set(COMMIT_AUTHOR ${COMMIT_AUTHOR})

	get_compiler_user(COMPILATION_AUTHOR)
	set(COMPILATION_AUTHOR ${COMPILATION_AUTHOR})
	
	get_git_branch(CURRENT_BRANCH)
	set(CURRENT_BRANCH ${CURRENT_BRANCH})
	
	get_compiler_name(COMPILER_NAME)
	set(COMPILER_NAME ${COMPILER_NAME})

	include(${CMAKE_CURRENT_SOURCE_DIR}/.cmake/getGitRevisionDescription.cmake)
	set(HEADER_FILE_VERSION_PATH ${CMAKE_CURRENT_SOURCE_DIR}/res/version.h)
	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/.cmake/version.h.in
		${HEADER_FILE_VERSION_PATH}
	)
	set(EXAMPLE_VERSION_RC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/res/version.rc)
	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/.cmake/version_exe.rc.in
		${EXAMPLE_VERSION_RC_PATH}
	)
endfunction()