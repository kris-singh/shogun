include(ShogunUtils)

# Meta dependencies
FIND_PACKAGE(PLY REQUIRED)
IF(NOT PLY_FOUND)
    message(FATAL_ERROR "Python module ply required for meta examples. Disable meta examples or install python-ply")
ENDIF()

FIND_PACKAGE(Ctags)
IF(NOT CTAGS_FOUND)
    message(FATAL_ERROR "Ctags required for meta examples.")
ENDIF()

# get list of meta examples that can be built
INCLUDE(FindMetaExamples)
find_meta_examples()

# get parser and target languages needed to build the meta examples
SET(GENERATOR_DEPENDENCIES)
LIST(APPEND GENERATOR_DEPENDENCIES ${CMAKE_SOURCE_DIR}/examples/meta/generator/generate.py)
LIST(APPEND GENERATOR_DEPENDENCIES ${CMAKE_SOURCE_DIR}/examples/meta/generator/parse.py)
LIST(APPEND GENERATOR_DEPENDENCIES ${CMAKE_SOURCE_DIR}/examples/meta/generator/translate.py)
FILE(GLOB TARGET_LANGUAGES ${CMAKE_SOURCE_DIR}/examples/meta/generator/targets/*.json)
LIST(APPEND GENERATOR_DEPENDENCIES ${TARGET_LANGUAGES})

# parser output
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/parser_files)

# list of interfaces for which we dont generate meta examples
SET(DISABLED_INTERFACES PerlModular)

FOREACH(META_EXAMPLE ${META_EXAMPLES})
    # assume a structure <target_language>/<category>/listing.sg
    GET_META_EXAMPLE_VARS(${META_EXAMPLE} EXAMPLE_NAME EXAMPLE_REL_DIR EXAMPLE_NAME_WITH_DIR)

    SET(EXAMPLE_LISTINGS)
    SET(BASENAME ${EXAMPLE_REL_DIR}/${EXAMPLE_NAME})
    LIST(APPEND EXAMPLE_LISTINGS ${CMAKE_CURRENT_BINARY_DIR}/cpp/${BASENAME}.cpp)
    FOREACH(interface ${AVAILABLE_INTERFACES})
        list(FIND DISABLED_INTERFACES ${interface} disabled)
        if (${disabled} STREQUAL "-1")
            GET_INTERFACE_VARS(${interface} DIRECTORY EXTENSION)
            LIST(APPEND EXAMPLE_LISTINGS ${CMAKE_CURRENT_BINARY_DIR}/${DIRECTORY}/${BASENAME}.${EXTENSION})
        endif()
    ENDFOREACH()

	# Set generate.py flags
	SET(GENERATOR_FLAGS
		-i ${META_EXAMPLE}
		-o ${CMAKE_CURRENT_BINARY_DIR}
		-t ${CMAKE_CURRENT_SOURCE_DIR}/generator/targets
		-g ${CTAGS_FILE}
		--parser_files_dir=${CMAKE_CURRENT_BINARY_DIR}/parser_files)

	# Enable variable storing only if we have enabled testing
	IF(ENABLE_TESTING)
		LIST(APPEND GENERATOR_FLAGS --store-vars)
	ENDIF()

    # Run example generation script
    SET(TEMP ${CMAKE_CURRENT_BINARY_DIR}/python/${EXAMPLE_REL_DIR}/${EXAMPLE_NAME}.py)
    add_custom_command(OUTPUT ${EXAMPLE_LISTINGS}
        COMMAND ${PYTHON_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/generator/generate.py ${GENERATOR_FLAGS}
		COMMENT "Generating example ${EXAMPLE_NAME_WITH_DIR}"
        DEPENDS ctags ${META_EXAMPLE} ${GENERATOR_DEPENDENCIES})

    LIST(APPEND TRANSLATED_META_EXAMPLES ${EXAMPLE_LISTINGS})
ENDFOREACH()
SET_SOURCE_FILES_PROPERTIES(${TRANSLATED_META_EXAMPLES} PROPERTIES GENERATED TRUE)

add_custom_target(meta_examples
    DEPENDS ${TRANSLATED_META_EXAMPLES})

INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cpp
    DESTINATION share/shogun/examples
    COMPONENT ${DIRECTORY}-examples
    FILES_MATCHING PATTERN "*.cpp"
    PATTERN "CMakeFiles*" EXCLUDE
    )

IF (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/data)
    # copy data symlink to cmake binary dir so that we can execute meta examples in there
    CREATE_DATA_SYMLINK(${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data)
ENDIF()

# c++
IF (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cpp AND NOT ${TRAVIS_DISABLE_META_CPP})
    IF(HAVE_CXX0X OR HAVE_CXX11)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cpp)
    ELSE()
        MESSAGE(WARNING "Skipping C++ meta examples; requires a c++11 compatible compiler.")
    ENDIF()
ENDIF()

########### test all the enabled interfaces & install their files to examples

# FIXME: these are the interfaces which has various problems
# hence need to disable their testing
# temporarily disabled as R and static calls has to be fixed
LIST(APPEND DISABLED_INTERFACES RModular)
# temporarily removed since lua modular currently
# does not support overloaded c++ methods
# see https://github.com/shogun-toolbox/shogun/issues/3018
LIST(APPEND DISABLED_INTERFACES LuaModular)


FOREACH(interface ${AVAILABLE_INTERFACES})
    list(FIND DISABLED_INTERFACES ${interface} disabled)
    if (${interface} AND ${disabled} STREQUAL "-1")
        GET_INTERFACE_VARS(${interface} DIRECTORY EXTENSION)

        INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${DIRECTORY}
            DESTINATION share/shogun/examples
            COMPONENT ${DIRECTORY}-examples
            FILES_MATCHING PATTERN "*.${EXTENSION}"
            PATTERN "CMakeFiles*" EXCLUDE
            )

		add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY})

	endif()
ENDFOREACH()
