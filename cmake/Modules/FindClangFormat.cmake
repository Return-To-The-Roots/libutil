# Finds the clang-format executable and
# sets ClangFormat_FOUND and ClangFormat_Version
#
# Defines the following macros:
#   add_ClangFormat_files(files)
#       Add files to the list of files to be formatted.
#   add_ClangFormat_folder(folder recursive)
#       Add all files matching the glob in ClangFormat_SRC_EXTENSIONS to the
#       list of files to be formatted.
#       If recursive is true, subfolders will be added too.
#   print_ClangFormat_files()
#       Print the current list of files as a STATUS message.
#   add_ClangFormat_target(style)
#       Adds a new target "clangFormat" that will format all files in the list
#       Allowed values for style are e.g.: LLVM, Google, Chromium, Mozilla, Webkit
#       and file to use a .clang-format file in one of the srcs parent directory

set(names clang-format)
set(min 4)
set(max 7)
if(ClangFormat_FIND_VERSION_MAJOR)
    set(min ${ClangFormat_FIND_VERSION_MAJOR})
    if(max LESS ClangFormat_FIND_VERSION_MAJOR OR ClangFormat_FIND_VERSION_EXACT)
        set(max ${ClangFormat_FIND_VERSION_MAJOR})
    endif()
endif()
foreach(version RANGE ${min} ${max})
    list(INSERT names 0 clang-format-${version}.0)
endforeach()

find_program(ClangFormat_BINARY
    NAMES ${names}
    PATHS ${ClangTools_PATH} /usr/local/bin /usr/bin ENV CLANG_TOOLS_PATH
)
if(ClangFormat_BINARY)
    execute_process(COMMAND ${ClangFormat_BINARY} --version OUTPUT_VARIABLE out)
    string(REGEX REPLACE ".*clang-format version ([^ \n]+).*" "\\1" ClangFormat_VERSION_STRING "${out}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ClangFormat
    FOUND_VAR ClangFormat_FOUND
    REQUIRED_VARS ClangFormat_BINARY
    VERSION_VAR ClangFormat_VERSION_STRING)

if(ClangFormat_FOUND)
    set(ClangFormat_FILES "" CACHE INTERNAL "")
    set(ClangFormat_SRC_EXTENSIONS "*.cpp;*.h;*.hpp;*.c;*.tpp" CACHE STRING "Extensions to consider for add_ClangFormat_folder")
    mark_as_advanced(ClangFormat_SRC_EXTENSIONS)
    function(add_ClangFormat_files)
        foreach(curFile ${ARGN})
            get_filename_component(absCurFile ${curFile} ABSOLUTE)
            set(ClangFormat_FILES ${ClangFormat_FILES} "${absCurFile}" CACHE INTERNAL "")
        endforeach()
    endfunction()
    function(add_ClangFormat_folder folder recursive)
        set(globExpr "")
        string(REPLACE " " ";" exts "${ClangFormat_SRC_EXTENSIONS}")
        foreach(ext ${exts})
            list(APPEND globExpr "${folder}/${ext}")
        endforeach()
        if(recursive)
            file(GLOB_RECURSE files ${globExpr})
        else()
            file(GLOB files ${globExpr})
        endif()
        if(files)
            add_ClangFormat_files(${files})
        endif()
    endfunction()
    function(print_ClangFormat_files)
        message(STATUS "Files for clang-format: ${ClangFormat_FILES}")
    endfunction()
    function(add_ClangFormat_target style)
        if(ClangFormat_BINARY)
            set(format_files "")
            set(clangFormatFolder ${CMAKE_CURRENT_BINARY_DIR}/clangFormat)
            # Cache last folder. To be more usefull sort the list first
            list(SORT ClangFormat_FILES)
            set(lastFolder "")
            set(curClangFormatFolder ${clangFormatFolder})
            foreach(source IN LISTS ClangFormat_FILES)
                get_filename_component(source_file_name ${source} NAME)
                get_filename_component(source_file_dir ${source} DIRECTORY)
                # If the source folder has changed calculate the new curClangFormatFolder
                if(NOT lastFolder STREQUAL source_file_dir)
                    file(RELATIVE_PATH relDir ${CMAKE_SOURCE_DIR} ${source_file_dir})
                    if(relDir MATCHES "\\.\\.")
                        message(FATAL_ERROR "${source} is not in a sub directory of ${CMAKE_SOURCE_DIR}")
                    endif()
                    set(curClangFormatFolder ${clangFormatFolder}/${relDir})
                    file(MAKE_DIRECTORY ${curClangFormatFolder})
                    set(lastFolder ${source_file_dir})
                endif()
                set(format_file ${curClangFormatFolder}/${source_file_name}.format)

                add_custom_command(OUTPUT ${format_file}
                        DEPENDS ${source}
                        COMMENT "Clang-Format ${source}"
                        COMMAND ${ClangFormat_BINARY} -style=${style} -i ${source}
                        COMMAND ${CMAKE_COMMAND} -E touch ${format_file}
                )

                list(APPEND format_files ${format_file})
            endforeach()
     
            if(format_files)
                add_custom_target(clangFormat DEPENDS ${format_files})
            endif()
        endif()
    endfunction()
endif()
