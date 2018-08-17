# Instruct CMake to run moc and rrc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(APP_SOURCES
    main.cpp
)

if(WIN32)
    set(APP_SOURCES ${APP_SOURCES} OpenTodoList.rc)
endif()

set(APP_RESOURCES
    ./qml.qrc
    ./res.qrc
    ./translations.qrc
)

# Find required Qt libraries
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5Widgets REQUIRED)

if(WIN32)
    set(APP_FLAGS WIN32)
endif()

if(APPLE)
    set(APP_FLAGS MACOSX_BUNDLE)
    set(APP_SOURCES ${APP_SOURCES} res/OpenTodoList.icns)
    set(MACOSX_BUNDLE_ICON_FILE OpenTodoList.icns)
    set_source_files_properties(res/OpenTodoList.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()

if(ANDROID)
    add_library(OpenTodoList SHARED ${APP_FLAGS} ${APP_SOURCES} ${APP_RESOURCES})
    set(EXTRA_LIBS $<TARGET_FILE:qt5keychain>)
    if(NOT "${OPENTODOLIST_ANDROID_EXTRA_LIBS}" STREQUAL "")
        set(EXTRA_LIBS "${EXTRA_LIBS},${OPENTODOLIST_ANDROID_EXTRA_LIBS}")
    endif()
    qt_android_build_apk(
        TARGET OpenTodoList
        PACKAGE_NAME net.rpdev.opentodolist
        QML_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}
        ANDROID_EXTRA_FILES ${CMAKE_CURRENT_SOURCE_DIR}/android
        EXTRA_LIBS "${EXTRA_LIBS}"
    )
else()
    add_executable(OpenTodoList ${APP_FLAGS} ${APP_SOURCES} ${APP_RESOURCES})
endif()

configure_file(Info.plist.in ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)
set_target_properties(OpenTodoList PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)

target_link_libraries(
    OpenTodoList

    # Internal:
    opentodolist-common

    # Qt libraries:
    Qt5::Qml
    Qt5::Quick
    Qt5::Xml
    Qt5::Concurrent
    Qt5::Widgets
)
