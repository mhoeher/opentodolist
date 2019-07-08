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
)

# Find required Qt libraries
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5LinguistTools REQUIRED)

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

    set(
        ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
        CACHE INTERNAL "Extra files for Android deployment"
    )
    set(
        ANDROID_EXTRA_LIBS
        ${ANDROID_EXTRA_LIBS} ${EXTRA_LIBS}
    )
    list(REMOVE_DUPLICATES ANDROID_EXTRA_LIBS)
    set(ANDROID_EXTRA_LIBS ${ANDROID_EXTRA_LIBS}
        CACHE INTERNAL "Extra libraries to include in APK" FORCE
    )
    qt_android_build_apk(
        TARGET OpenTodoList
        PACKAGE_NAME net.rpdev.opentodolist
        QML_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}
    )
else()
    add_executable(OpenTodoList ${APP_FLAGS} ${APP_SOURCES} ${APP_RESOURCES})
endif()

configure_file(Info.plist.in ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)
set_target_properties(OpenTodoList PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)

if(NOT ANDROID)
    target_compile_definitions(OpenTodoList PRIVATE
        OTL_USE_SINGLE_APPLICATION
        QAPPLICATION_CLASS=QApplication)
    target_include_directories(OpenTodoList PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/SingleApplication)

    target_sources(OpenTodoList PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/SingleApplication/singleapplication.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/SingleApplication/singleapplication_p.cpp
    )
endif()

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

if(OPENTODOLIST_FLATPAK)
    target_compile_definitions(OpenTodoList PRIVATE OPENTODOLIST_FLATPAK)
endif()

file(GLOB_RECURSE TS_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/*.js
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/*.h
)
set(TS_FILES
    translations/opentodolist_en.ts
    translations/opentodolist_de.ts
    translations/opentodolist_fr.ts
)
qt5_create_translation(
    MESSAGES
    ${TS_SOURCE_FILES}
    ${TS_FILES}
)
qt5_add_translation(QM_FILES ${TS_FILES})
target_sources(OpenTodoList PRIVATE
    ${QM_FILES}
    ${CMAKE_CURRENT_BINARY_DIR}/translations-copy.qrc
)
file(WRITE  ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
    "<RCC>")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
    "<qresource prefix='/translations'>")
foreach(QM_FILE ${QM_FILES})
    get_filename_component(QM_FILE_NAME ${QM_FILE} NAME)
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
        "<file>${QM_FILE_NAME}</file>")
endforeach()
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
    "</qresource>")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
    "</RCC>")

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/translations-copy.qrc
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
        ${QM_FILES}
    COMMAND
        ${CMAKE_COMMAND} -E
            copy
            ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
            ${CMAKE_CURRENT_BINARY_DIR}/translations-copy.qrc
)

add_custom_target(opentodolist-translations
    DEPENDS
        ${QM_FILES}
        translations-copy.qrc
)


# Helper targets to convert between *.ts and *.po files (for
# external editing)
get_target_property(LCONVERT_EXECUTABLE Qt5::lconvert IMPORTED_LOCATION)
add_custom_target(opentodolist-po-to-ts)
foreach(ts_file ${TS_FILES})
    string(REGEX REPLACE "ts$" "po" po_file ${ts_file})
    set(PO_FILES ${PO_FILES} ${po_file})
    add_custom_command(
        OUTPUT ${po_file}
        COMMAND
            ${LCONVERT_EXECUTABLE}
                -i ${CMAKE_CURRENT_SOURCE_DIR}/${ts_file}
                -o ${CMAKE_CURRENT_SOURCE_DIR}/${po_file}
    )
    add_custom_command(
        TARGET opentodolist-po-to-ts
        COMMAND
            ${LCONVERT_EXECUTABLE}
                -i ${CMAKE_CURRENT_SOURCE_DIR}/${po_file}
                -o ${CMAKE_CURRENT_SOURCE_DIR}/${ts_file}
    )
endforeach()
add_custom_target(opentodolist-ts-to-po DEPENDS ${PO_FILES})

# Install the app:
install(
    TARGETS OpenTodoList
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)
