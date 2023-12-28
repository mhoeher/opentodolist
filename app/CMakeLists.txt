# Instruct CMake to run moc and rrc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(PROJECT_SOURCES
    main.cpp
    appstartup.cpp
    appstartup.h
)

if(WIN32)
    set(PROJECT_SOURCES ${PROJECT_SOURCES} OpenTodoList.rc)
endif()

set(APP_RESOURCES
    ./qml.qrc
    ./res.qrc
)

if(WIN32)
    set(APP_FLAGS WIN32)
endif()

set(CI_PIPELINE_IID $ENV{CI_PIPELINE_IID})
if("${CI_PIPELINE_IID}" STREQUAL "")
    set(CI_PIPELINE_IID "0")
endif()

if(APPLE)
    set(APP_FLAGS MACOSX_BUNDLE)
    set(PROJECT_SOURCES ${PROJECT_SOURCES} res/OpenTodoList.icns)
    set(MACOSX_BUNDLE_ICON_FILE OpenTodoList.icns)
    set_source_files_properties(res/OpenTodoList.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(OpenTodoList ${APP_FLAGS} ${PROJECT_SOURCES} ${APP_RESOURCES})

    # Include Assets and special config for iOS
    if(IOS)
        target_sources(OpenTodoList PRIVATE ios/Images.xcassets)
        set_source_files_properties(
            ios/Images.xcassets
            PROPERTIES
                MACOSX_PACKAGE_LOCATION Resources
        )
        qt_import_plugins(
            OpenTodoList
            INCLUDE Qt::QIosOptionalPlugin_NSPhotoLibraryPlugin
        )
    endif()
else()
    if(ANDROID)
        add_library(OpenTodoList SHARED ${PROJECT_SOURCES} ${APP_RESOURCES})

        set(
            ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
            CACHE INTERNAL "Extra files for Android deployment"
        )
    elseif(IOS)
        include(../3rdparty/QtIosCMake/AddQtIosApp.cmake)
        include(../3rdparty/QtStaticCMake/QtStaticCMake.cmake)
        add_executable(OpenTodoList)
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
            LAUNCHSCREEN_STORYBOARD "ios/LaunchScreenOTL.storyboard"
            CUSTOM_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/ios/Info.plist.in
        )
        target_sources(OpenTodoList PRIVATE  ${PROJECT_SOURCES} ${APP_RESOURCES})
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
        add_executable(OpenTodoList ${APP_FLAGS} ${PROJECT_SOURCES} ${APP_RESOURCES})
    endif()
endif()

if(APPLE)
    if(IOS)
        set_target_properties(
            OpenTodoList
            PROPERTIES
                MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/ios/Info.plist.in
        )
        target_sources(
            OpenTodoList
            PRIVATE
                ios/LaunchScreenOTL.storyboard
        )
        set_source_files_properties(ios/LaunchScreenOTL.storyboard PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    else(IOS)
        set_target_properties(
            OpenTodoList
            PROPERTIES
                MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
        )
    endif()
endif()

target_compile_definitions(
    OpenTodoList
    PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>
)

target_compile_options(
    OpenTodoList
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Werror>
)

target_compile_features(
    OpenTodoList PRIVATE cxx_std_14
)

set_target_properties(
    OpenTodoList
    PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER net.rpdev.OpenTodoList
        MACOSX_BUNDLE_BUNDLE_NAME OpenTodoList
        MACOSX_BUNDLE_BUNDLE_VERSION "${OPENTODOLIST_VERSION_MAJOR}.${OPENTODOLIST_VERSION_MINOR}.${CI_PIPELINE_IID}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${OPENTODOLIST_VERSION_MAJOR}.${OPENTODOLIST_VERSION_MINOR}.${CI_PIPELINE_IID}"
        MACOSX_BUNDLE_LONG_VERSION_STRING "${OPENTODOLIST_VERSION_MAJOR}.${OPENTODOLIST_VERSION_MINOR}.${OPENTODOLIST_VERSION_PATCH}"
        MACOSX_BUNDLE_LAUNCHSCREEN_STORYBOARD LaunchScreen
        XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "${OPENTODOLIST_APPLE_CODE_SIGN_IDENTITY}"
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${OPENTODOLIST_APPLE_TEAM_ID}"
        XCODE_ATTRIBUTE_CODE_SIGN_STYLE "${OPENTODOLIST_APPLE_CODE_SIGN_STYLE}"
        XCODE_PROPERTY_PRODUCT_BUNDLE_IDENTIFIER "net.rpdev.OpenTodoList"

        QT_ANDROID_EXTRA_LIBS "${ANDROID_EXTRA_LIBS}"
        QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
        QT_ANDROID_TARGET_SDK_VERSION 34
        QT_ANDROID_MIN_SDK_VERSION 23
        QT_ANDROID_VERSION_NAME "${OPENTODOLIST_VERSION}"
        QT_ANDROID_VERSION_CODE "${OPENTODOLIST_ANDROID_VERSION_CODE}"
)

if(APPLE)
    if(IOS)
        set_target_properties(
            OpenTodoList
            PROPERTIES
                MACOSX_BUNDLE_ICON_FILE AppIcon
                XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME AppIcon
                XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
            )
    else(IOS)
        set_target_properties(
            OpenTodoList
            PROPERTIES
                MACOSX_BUNDLE_ICON_FILE OpenTodoList.icns
            )
            # Create empty translation files - needed to localize the menu bar on macOS
            # (https://bugs.kde.org/show_bug.cgi?id=432685#c3):
            file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/Empty.strings "\"Empty Translation\" = \"Empty Translation\";")
            file(GLOB _ts_files ${CMAKE_CURRENT_SOURCE_DIR}/translations/*.ts)
            foreach(_ts_file ${_ts_files})
                get_filename_component(_ts_name "${_ts_file}" NAME)
                string(REPLACE "-" ";" _ts_name "${_ts_name}")
                string(REPLACE "." ";" _ts_name "${_ts_name}")
                list(GET _ts_name 1 _ts_lang)
                add_custom_command(
                    TARGET OpenTodoList 
                    POST_BUILD
                    COMMAND
                        ${CMAKE_COMMAND} -E make_directory "$<TARGET_BUNDLE_DIR:OpenTodoList>/Contents/Resources/${_ts_lang}.lproj"
                    COMMAND
                        ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/Empty.strings "$<TARGET_BUNDLE_DIR:OpenTodoList>/Contents/Resources/${_ts_lang}.lproj/"
                    COMMENT "Create empty strings file for ${_ts_name}"
                )
            endforeach()

            # Include Qt's translations:
            get_target_property(_qmake_executable Qt::qmake IMPORTED_LOCATION)
            get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
            get_filename_component(_qt_dir "${_qt_bin_dir}" DIRECTORY)
            file(GLOB _qm_files ${_qt_dir}/translations/qt*.qm)
            add_custom_command(
                TARGET OpenTodoList 
                POST_BUILD
                COMMAND
                    ${CMAKE_COMMAND} -E make_directory "$<TARGET_BUNDLE_DIR:OpenTodoList>/Contents/translations"
                COMMAND
                    ${CMAKE_COMMAND} -E copy ${_qm_files} "$<TARGET_BUNDLE_DIR:OpenTodoList>/Contents/translations"
                COMMENT "Copy Qt translations"
            )
    endif()
endif()

if(NOT ANDROID AND NOT IOS)
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
        Qt${QT_VERSION_MAJOR}::Concurrent
        Qt${QT_VERSION_MAJOR}::Widgets
)

add_dependencies(OpenTodoList opentodolist-common)

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
        qt_create_translation(
            MESSAGES
            ${TS_SOURCE_FILES}
            ${TS_FILES}
        )
    endif()
endif()
qt_add_translation(QM_FILES ${TS_FILES})
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
get_target_property(LCONVERT_EXECUTABLE Qt::lconvert IMPORTED_LOCATION)
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

set(QML_IMPORT_PATH ${CMAKE_CURRENT_SOURCE_DIR}
    CACHE PATH "QML search path for project local files")

if (ANDROID)
    set(Qt6_VERSION "${QT_VERSION}")
    set(CMAKE_ANDROID_ARCH_ABI "${ANDROID_ABI}")
    add_android_openssl_libraries(OpenTodoList)
endif()
