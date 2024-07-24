set(FOLDER_NAME app)
set(TARGET_NAME LynxMaker)
set(APPLICATION_VERSION 0.1)

project(${TARGET_NAME} VERSION ${APPLICATION_VERSION})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(TINYXML2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../ext/tinyxml2/10.0.0/)
set(LIBSSH2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../ext/libssh2/1.11.0/)

include(${CMAKE_UTILS_DIR}/utils.cmake)
set(config "FOLDER_NAME:${FOLDER_NAME};TARGET_NAME:${TARGET_NAME}")

include(${CMAKE_CURRENT_SOURCE_DIR}/.cmake/CmakeUtils.cmake)
setup_version()

find_package(Qt6 REQUIRED COMPONENTS Quick Svg Network Widgets Concurrent LinguistTools)
find_package(gArtnet gCore gFtp gLog gMamba gMqtt gMulticastSocket gPromotion gSerializer)
find_package(xMaker xDesignKit lynxV2)

FILE(GLOB_RECURSE header_files ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
FILE(GLOB_RECURSE source_files ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
FILE(GLOB_RECURSE resource_files ${CMAKE_CURRENT_SOURCE_DIR}/res/*.*)
FILE(GLOB_RECURSE qml_files ${CMAKE_CURRENT_SOURCE_DIR}/qml/*.*)

# translation system
set(TRANSLATABLE_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/res/ui)
set(TRANSLATION_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/res/translations/${TARGET_NAME}_fr_FR.ts
	${CMAKE_CURRENT_SOURCE_DIR}/res/translations/${TARGET_NAME}_zh_CN.ts
	${CMAKE_CURRENT_SOURCE_DIR}/res/translations/${TARGET_NAME}_en_US.ts)

qt6_create_translation(qm_files ${CMAKE_CURRENT_SOURCE_DIR} ${TRANSLATION_FILES} OPTIONS -locations none)
qt6_add_translation(qm_files ${TRANSLATION_FILES})

source_group (TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/ PREFIX "Header Files" FILES ${header_files})
source_group (TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/ PREFIX "Source Files" FILES ${source_files})
source_group (TREE ${CMAKE_CURRENT_SOURCE_DIR}/res/ PREFIX "Resource Files" FILES ${resource_files})
source_group (TREE ${CMAKE_CURRENT_SOURCE_DIR}/qml/ PREFIX "Qml Files" FILES ${qml_files})

add_executable(${TARGET_NAME} WIN32
	${source_files}
	${header_files}
	${resource_files}
	${qml_files}
        ${qm_files}
)

if(MSVC_IDE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

if(MSVC)
	set_target_properties(${TARGET_NAME} PROPERTIES WIN32_EXECUTABLE FALSE)
	add_compile_definitions(NOMINMAX)
endif()

set_target_properties(${TARGET_NAME} PROPERTIES FOLDER ${FOLDER_NAME})

target_sources(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc)

target_include_directories(${TARGET_NAME} PRIVATE include res)

target_link_libraries(${TARGET_NAME} PRIVATE Qt6::Quick Qt6::Svg Qt6::Network Qt6::Widgets Qt6::Concurrent)
target_link_libraries(${TARGET_NAME} PRIVATE gArtnet gCore gFtp gLog gMamba gMqtt gMulticastSocket gPromotion gSerializer)
target_link_libraries(${TARGET_NAME} PRIVATE xMaker xDesignKit)

target_include_directories(${TARGET_NAME} PRIVATE ${TINYXML2_DIR}/include)
target_include_directories(${TARGET_NAME} PRIVATE ${LIBSSH2_DIR}/include)

target_link_libraries(${TARGET_NAME} PRIVATE optimized ${TINYXML2_DIR}/lib/tinyxml2.lib)
target_link_libraries(${TARGET_NAME} PRIVATE debug ${TINYXML2_DIR}/lib/tinyxml2d.lib)	
target_link_libraries(${TARGET_NAME} PRIVATE optimized ${LIBSSH2_DIR}/lib/libssh2.lib)
target_link_libraries(${TARGET_NAME} PRIVATE debug ${LIBSSH2_DIR}/lib/libssh2d.lib)


install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION bin)

FILE(GLOB release_dll_files "${LIBSSH2_DIR}/bin/*.dll")
FILE(GLOB debug_dll_files "${LIBSSH2_DIR}/bin/*d.dll")
LIST(REMOVE_ITEM release_dll_files ${debug_dll_files})

get_target_property(${TARGET_NAME} Qt6::qmake IMPORTED_LOCATION)
get_filename_component(Qt6_WINDEPLOYQT_EXECUTABLE ${${TARGET_NAME}} PATH)
set(Qt6_WINDEPLOYQT_EXECUTABLE "${Qt6_WINDEPLOYQT_EXECUTABLE}/windeployqt.exe")
message(STATUS "Qt6_WINDEPLOYQT_EXECUTABLE: '${Qt6_WINDEPLOYQT_EXECUTABLE}'")

add_custom_command(	TARGET ${TARGET_NAME} POST_BUILD 
					COMMAND ${Qt6_WINDEPLOYQT_EXECUTABLE} 
					--qmldir ${CMAKE_CURRENT_SOURCE_DIR}/qml
					$<TARGET_FILE_DIR:${TARGET_NAME}>)

add_custom_command(	TARGET ${TARGET_NAME} POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_if_different 
					${qm_files} "$<TARGET_FILE_DIR:${TARGET_NAME}>/translations")

install(FILES ${qm_files} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/translations)
install(CODE "execute_process(COMMAND ${Qt6_WINDEPLOYQT_EXECUTABLE} --qmldir ${CMAKE_CURRENT_SOURCE_DIR}/qml ${CMAKE_INSTALL_PREFIX}/bin/${TARGET_NAME}.exe)")
