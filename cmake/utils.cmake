include(CMakeParseArguments)

find_package(Qt5Test REQUIRED)

find_program(GCOV_COMMAND gcov)
find_program(LCOV_COMMAND lcov)
find_program(GENHTML_COMMAND genhtml)

function(add_unit_test)
    set(options)
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
    
    file(GLOB test_sources *.cpp)
    find_package(Qt5Test REQUIRED)
    
    add_executable(test_${TEST_NAME} ${TEST_SOURCES})
    target_link_libraries(test_${TEST_NAME} OpenTodoListCore Qt5::Test)
    target_include_directories(test_${TEST_NAME} 
        PRIVATE 
            ../../lib
            ../../lib/datamodel
    )
    add_test(NAME ${TEST_NAME} COMMAND $<TARGET_FILE:test_${TEST_NAME}>)
    
    # Enable C++11:
    set_property(TARGET test_${TEST_NAME} PROPERTY CXX_STANDARD 11)
    set_property(TARGET test_${TEST_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
    
endfunction(add_unit_test)


add_custom_target(coverage)

if(WITH_COVERAGE)
    if(NOT GCOV_COMMAND)
        message(FATAL_ERROR "Running with coverage but 'gcov' command has not been found")
    endif(NOT GCOV_COMMAND)
    
    if(NOT LCOV_COMMAND)
        message(FATAL_ERROR "Running with coverage but 'lcov' command has not been found")
    endif(NOT LCOV_COMMAND)
    
    if(NOT GENHTML_COMMAND)
        message(FATAL_ERROR "Running with coverage but 'genhtml' command has not been found")
    endif(NOT GENHTML_COMMAND)
    
    add_custom_command(
        TARGET
            coverage
        COMMAND
            ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/coverage"
    )
    add_custom_command(
        TARGET
            coverage
        COMMAND
            ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/coverage"
    )
    add_custom_command(
        TARGET
            coverage
        COMMAND
            ${LCOV_COMMAND} 
                --no-external
                --capture
                --initial
                --directory ${CMAKE_BINARY_DIR}
                --directory ${CMAKE_SOURCE_DIR}
                --output-file ${CMAKE_BINARY_DIR}/opentodolist_base.info
        WORKING_DIRECTORY
            ${CMAKE_BINARY_DIR}/coverage
    )
    add_custom_command(
        TARGET
            coverage
        COMMAND
            ${CMAKE_COMMAND} --build . --target test
        WORKING_DIRECTORY
            ${CMAKE_BINARY_DIR}
    )
    add_custom_command(
        TARGET
            coverage
        COMMAND
            ${LCOV_COMMAND} 
                --no-external
                --capture 
                --directory ${CMAKE_BINARY_DIR}
                --directory ${CMAKE_SOURCE_DIR}
                --output-file ${CMAKE_BINARY_DIR}/opentodolist_test.info
        WORKING_DIRECTORY
            ${CMAKE_BINARY_DIR}/coverage
    )
    add_custom_command(
        TARGET
            coverage
        COMMAND
            ${LCOV_COMMAND} 
                --add-tracefile ${CMAKE_BINARY_DIR}/opentodolist_base.info
                --add-tracefile ${CMAKE_BINARY_DIR}/opentodolist_test.info
                --output-file ${CMAKE_BINARY_DIR}/opentodolist.info
        WORKING_DIRECTORY
            ${CMAKE_BINARY_DIR}/coverage
    )
    add_custom_command(
        TARGET
            coverage
        COMMAND
            ${LCOV_COMMAND}
                --remove ${CMAKE_BINARY_DIR}/opentodolist.info
                '${CMAKE_BINARY_DIR}/test/*'
                '${CMAKE_SOURCE_DIR}/test/*'
                '*/moc_*'
                '*/qrc_*'
                '*.moc'
                --output-file ${CMAKE_BINARY_DIR}/opentodolist_filtered.info
        WORKING_DIRECTORY
            ${CMAKE_BINARY_DIR}/coverage
    )
    add_custom_command(
        TARGET
            coverage
        COMMAND
            ${GENHTML_COMMAND} 
                --prefix ${CMAKE_BINARY_DIR} 
                --ignore-errors 
                source ${CMAKE_BINARY_DIR}/opentodolist_filtered.info
                --legend 
                --title "OpenTodoList ${OPENTODOLIST_VERSION}"
                --output-directory=${CMAKE_BINARY_DIR}/coverage/html
        WORKING_DIRECTORY
            ${CMAKE_BINARY_DIR}/coverage
    )
else(WITH_COVERAGE)
    add_custom_command(
        TARGET
            coverage
        COMMAND
            ${CMAKE_COMMAND} -E echo "Coverage is not enabled - configure with -DWITH_COVERAGE=ON to enable it"
        VERBATIM
    )
endif(WITH_COVERAGE)
