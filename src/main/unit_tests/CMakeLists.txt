project(unit_tests)
 
file(GLOB_RECURSE proj_SOURCES "*.cpp")

file(GLOB_RECURSE strat_eng_HEADERS "../strategy_engine_common/*.h")

set (strat_eng_INC "")
foreach (_headerFile ${strat_eng_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND strat_eng_INC ${_dir})
endforeach()
list(REMOVE_DUPLICATES strat_eng_INC)


include_directories(${PANTHEIOS_INC} ${strat_eng_INC} UnitTest ../atu_tcp_library ../logger ../util)
link_directories(/usr/local/pantheios/lib)

add_executable(${PROJECT_NAME} Main.cpp ${proj_SOURCES})
target_link_libraries(${PROJECT_NAME} logger strategy_engine_common util atu_tcp_library ${Boost_LIBRARIES} pthread ${GSL_LIBRARIES} ${ZeroMQ_LIBRARY} cppunit pantheios.1.core.gcc46 pantheios.1.be.file.gcc46 pantheios.1.bec.file.gcc46 pantheios.1.util.gcc46)
