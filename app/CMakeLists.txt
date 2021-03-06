# Instruct CMake to run moc and rrc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(APP_SOURCES
    main.cpp
    appstartup.cpp
    appstartup.h
)

if(WIN32)
    set(APP_SOURCES ${APP_SOURCES} OpenTodoList.rc)
endif()

set(APP_RESOURCES
    ./qml.qrc
    ./res.qrc
)

if(WIN32)
    set(APP_FLAGS WIN32)
endif()

if(APPLE)
    set(APP_FLAGS MACOSX_BUNDLE)
    set(APP_SOURCES ${APP_SOURCES} res/OpenTodoList.icns)
    set(MACOSX_BUNDLE_ICON_FILE OpenTodoList.icns)
    set_source_files_properties(res/OpenTodoList.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(OpenTodoList ${APP_FLAGS} ${APP_SOURCES} ${APP_RESOURCES})
else()
    if(ANDROID)
        add_library(OpenTodoList SHARED ${APP_SOURCES} ${APP_RESOURCES})

        set(
            ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
            CACHE INTERNAL "Extra files for Android deployment"
        )
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
        include(../3rdparty/QtIosCMake/AddQtIosApp.cmake)
        include(../3rdparty/QtStaticCMake/QtStaticCMake.cmake)
        add_executable(OpenTodoList)
        set(CI_PIPELINE_IID $ENV{CI_PIPELINE_IID})
        if("${CI_PIPELINE_IID}" STREQUAL "")
            set(CI_PIPELINE_IID "0")
        endif()
        add_qt_ios_app(
            OpenTodoList
            NAME "OpenTodoList"
            BUNDLE_IDENTIFIER "net.rpdev.OpenTodoList"
            VERSION "${OPENTODOLIST_VERSION_MAJOR}.${OPENTODOLIST_VERSION_MINOR}.${CI_PIPELINE_IID}"
            ORIENTATION_PORTRAIT
            ORIENTATION_PORTRAIT_UPDOWN
            ORIENTATION_PORTRAIT_UPSIDEDOWN
            ORIENTATION_LANDSCAPE_LEFT
            ORIENTATION_LANDSCAPE_RIGHT
            PHOTO_LIBRARY_USAGE_DESCRIPTION "Allow adding photos from the library as images."
            SUPPORT_IPHONE
            SUPPORT_IPAD
            IPA
            UPLOAD_SYMBOL
            ASSET_DIR ios/Images.xcassets
            LAUNCHSCREEN_STORYBOARD "ios/LaunchScreen.storyboard"
        )
        target_sources(OpenTodoList PRIVATE  ${APP_SOURCES} ${APP_RESOURCES})
        qt_generate_plugin_import(
            OpenTodoList
            OUTPUT "OpenTodoList_plugin_import.cpp"
            OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
            VERBOSE
        )

        qt_generate_qml_plugin_import(
            OpenTodoList
            QML_SRC ${CMAKE_CURRENT_SOURCE_DIR}
            OUTPUT "OpenTodoList_qml_plugin_import.cpp"
            OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
            VERBOSE
        )

    else()
        add_executable(OpenTodoList ${APP_FLAGS} ${APP_SOURCES} ${APP_RESOURCES})
    endif()
endif()

target_compile_definitions(
    OpenTodoList
    PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>
)

set_target_properties(
    OpenTodoList
    PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER net.rpdev.OpenTodoList
        MACOSX_BUNDLE_ICON_FILE OpenTodoList.icns
)

if(NOT ANDROID AND NOT ${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
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

    PRIVATE

        # Internal:
        opentodolist-common

        # Qt libraries:
        Qt${QT_VERSION_MAJOR}::Qml
        Qt${QT_VERSION_MAJOR}::Quick
        Qt${QT_VERSION_MAJOR}::Xml
        Qt${QT_VERSION_MAJOR}::Concurrent
        Qt${QT_VERSION_MAJOR}::Widgets
)

if(OPENTODOLIST_FLATPAK)
    target_compile_definitions(OpenTodoList PRIVATE OPENTODOLIST_FLATPAK)
endif()

if(OPENTODOLIST_WITH_KNOTIFICATIONS)
    target_link_libraries(
        OpenTodoList

        PRIVATE

            KF5::Notifications
    )
    target_compile_definitions(
        OpenTodoList

        PRIVATE

            OPENTODOLIST_WITH_KNOTIFICATIONS
    )
endif()

file(GLOB_RECURSE TS_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/*.js
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/*.h
)
file(GLOB TS_FILES translations/*.ts)
if(OPENTODOLIST_UPDATE_TRANSLATIONS)
    if (NOT ${PROJECT_NAME}-MultiAbiBuild)
        # Only run this once per multi-abi build on Android. Otherwise, we might
        # get conflicting writes to the *.ts files from parallel sub-builds.
        qt5_create_translation(
            MESSAGES
            ${TS_SOURCE_FILES}
            ${TS_FILES}
        )
    endif()
endif()
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
