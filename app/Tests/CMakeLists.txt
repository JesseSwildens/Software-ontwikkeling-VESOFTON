include(CTest)

file (GLOB TestToRun ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

list(APPEND TestSources ${TestToRun})
list(FILTER TestSources EXCLUDE REGEX ".*main\\.cpp$")

foreach (TEST ${TestSources})
    get_filename_component(TestName ${TEST} NAME_WE)

    add_executable (${TestName} main.cpp ${TEST})
    target_include_directories(${TestName} PRIVATE ${CMAKE_SOURCE_DIR}/API/bitmaps)

    target_compile_definitions(${TestName} PRIVATE 
        CMAKE_UNIT_TEST
        $<$<CONFIG:Debug>:DEBUG>
    )

    target_link_libraries(${TestName} Catch2 ${CMAKE_LIB_NAME})

    add_test (NAME ${TestName} COMMAND "$<TARGET_FILE:${TestName}>" "--reporter junit::out=${CMAKE_BINARY_DIR}/results_${TestName}.xml" "--success")
endforeach ()
