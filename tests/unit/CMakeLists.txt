FIND_PACKAGE(Jinja2)

# Find GTEST and GMOCK frameworks
include(external/GoogleTestNMock)
ExternalProject_Get_Property(GoogleMock source_dir)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
LINK_DIRECTORIES(${THIRD_PARTY_DIR}/libs/gmock)

# Generate automatic unittest from jinja2 templates
ADD_CUSTOM_COMMAND(OUTPUT clone_unittest.cc
	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/base/clone_unittest.cc.py
	${CMAKE_CURRENT_SOURCE_DIR}/base/clone_unittest.cc.jinja2
	clone_unittest.cc
	${CMAKE_BINARY_DIR}/src/shogun/base/class_list.cpp
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/base/clone_unittest.cc.py
	${CMAKE_CURRENT_SOURCE_DIR}/base/clone_unittest.cc.jinja2
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Generating clone_unittest.cc")
LIST(APPEND TEMPLATE_GENERATED_UNITTEST clone_unittest.cc)

ADD_CUSTOM_COMMAND(OUTPUT DynamicObjectArray_unittest.cc
	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/base/clone_unittest.cc.py
	${CMAKE_CURRENT_SOURCE_DIR}/base/DynamicObjectArray_unittest.cc.jinja2
	DynamicObjectArray_unittest.cc
	${CMAKE_BINARY_DIR}/src/shogun/base/class_list.cpp
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/base/clone_unittest.cc.py
	${CMAKE_CURRENT_SOURCE_DIR}/base/DynamicObjectArray_unittest.cc.jinja2
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Generating DynamicObjectArray_unittest.cc")
LIST(APPEND TEMPLATE_GENERATED_UNITTEST DynamicObjectArray_unittest.cc)

ADD_CUSTOM_COMMAND(OUTPUT SerializationAscii_unittest.cc
	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/base/clone_unittest.cc.py
	${CMAKE_CURRENT_SOURCE_DIR}/io/SerializationAscii_unittest.cc.jinja2
	SerializationAscii_unittest.cc
	${CMAKE_BINARY_DIR}/src/shogun/base/class_list.cpp
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/base/clone_unittest.cc.py
	${CMAKE_CURRENT_SOURCE_DIR}/io/SerializationAscii_unittest.cc.jinja2
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Generating SerializationAscii_unittest.cc")
LIST(APPEND TEMPLATE_GENERATED_UNITTEST SerializationAscii_unittest.cc)

ADD_CUSTOM_COMMAND(OUTPUT SerializationHDF5_unittest.cc
	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/base/clone_unittest.cc.py
	${CMAKE_CURRENT_SOURCE_DIR}/io/SerializationHDF5_unittest.cc.jinja2
	SerializationHDF5_unittest.cc
	${CMAKE_BINARY_DIR}/src/shogun/base/class_list.cpp
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/base/clone_unittest.cc.py
	${CMAKE_CURRENT_SOURCE_DIR}/io/SerializationHDF5_unittest.cc.jinja2
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Generating SerializationHDF5_unittest.cc")
LIST(APPEND TEMPLATE_GENERATED_UNITTEST SerializationHDF5_unittest.cc)

ADD_CUSTOM_COMMAND(OUTPUT SerializationJSON_unittest.cc
	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/base/clone_unittest.cc.py
	${CMAKE_CURRENT_SOURCE_DIR}/io/SerializationJSON_unittest.cc.jinja2
	SerializationJSON_unittest.cc
	${CMAKE_BINARY_DIR}/src/shogun/base/class_list.cpp
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/base/clone_unittest.cc.py
	${CMAKE_CURRENT_SOURCE_DIR}/io/SerializationJSON_unittest.cc.jinja2
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Generating SerializationJSON_unittest.cc")
LIST(APPEND TEMPLATE_GENERATED_UNITTEST SerializationJSON_unittest.cc)

ADD_CUSTOM_COMMAND(OUTPUT SerializationXML_unittest.cc
	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/base/clone_unittest.cc.py
	${CMAKE_CURRENT_SOURCE_DIR}/io/SerializationXML_unittest.cc.jinja2
	SerializationXML_unittest.cc
	${CMAKE_BINARY_DIR}/src/shogun/base/class_list.cpp
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/base/clone_unittest.cc.py
	${CMAKE_CURRENT_SOURCE_DIR}/io/SerializationXML_unittest.cc.jinja2
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Generating SerializationXML_unittest.cc")
LIST(APPEND TEMPLATE_GENERATED_UNITTEST SerializationXML_unittest.cc)

add_executable (discover_gtest_tests ${CMAKE_CURRENT_SOURCE_DIR}/discover_gtest_tests.cpp)
set_target_properties (discover_gtest_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties (discover_gtest_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set_target_properties (discover_gtest_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

# collect the unittests
FILE(GLOB_RECURSE UNITTEST_SRC *_unittest.cc)

ADD_EXECUTABLE(shogun-unit-test ${UNITTEST_SRC} ${TEMPLATE_GENERATED_UNITTEST})
add_dependencies(shogun-unit-test GoogleMock shogun::shogun)
target_include_directories(shogun-unit-test PRIVATE ${source_dir}/googlemock/include ${INCLUDES} ${source_dir}/googletest/include)
target_link_libraries(shogun-unit-test PRIVATE shogun_deps gmock gtest ${SANITIZER_LIBRARY})
IF (SANITIZER_FLAGS)
	set_target_properties(shogun-unit-test PROPERTIES COMPILE_FLAGS ${SANITIZER_FLAGS})
ENDIF()
set_target_properties (shogun-unit-test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties (shogun-unit-test PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set_target_properties (shogun-unit-test PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

# In some cases gTest segfaults with GCC >= 6.0.0.  This is a dirty fix.
# TODO: Update to gTest-release with proper support for GCC >= 6.0.0.
# See:  https://github.com/google/googletest/issues/705
IF(CMAKE_COMPILER_IS_GNUCXX)
	# in order to support cmake 2.8.7 and older
	IF(NOT CMAKE_CXX_COMPILER_VERSION)
		include(CheckCompiler)
	ENDIF(NOT CMAKE_CXX_COMPILER_VERSION)
	IF(NOT "${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS "6.0.0")
		target_compile_options(shogun-unit-test
			PUBLIC -fno-delete-null-pointer-checks)
	ENDIF(NOT "${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS "6.0.0")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_CUSTOM_TARGET(unit-tests
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/shogun-unit-test
	DEPENDS shogun-unit-test)

shogun_discover_tests(shogun-unit-test)

# Add unittests to the dependencies of modular interfaces to make sure nothing
# will infer with them being build single-threaded.
IF(SWIG_SINGLE_THREADED)
	FOREACH(SG_MODULAR_INTERFACE_TARGET ${SG_MODULAR_INTERFACE_TARGETS})
		ADD_DEPENDENCIES(${SG_MODULAR_INTERFACE_TARGET} shogun-unit-test)
	ENDFOREACH(SG_MODULAR_INTERFACE_TARGET ${SG_MODULAR_INTERFACE_TARGETS})
ENDIF(SWIG_SINGLE_THREADED)

unset(CMAKE_DEFINITIONS)
