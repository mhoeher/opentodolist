################################################################################
#
# Add App Executable
#
# This is the actual app - linking against the core library.
#
################################################################################

qt_add_executable(OpenTodoList WIN32 MACOSX_BUNDLE)

target_sources(
    OpenTodoList
    PRIVATE
        abstractitemmodel.h
        application.cpp
        application.h
        datamodel/complexitem.cpp
        datamodel/complexitem.h
        datamodel/image.cpp
        datamodel/image.h
        datamodel/item.cpp
        datamodel/item.h
        datamodel/library.cpp
        datamodel/library.h
        datamodel/note.cpp
        datamodel/note.h
        datamodel/notepage.cpp
        datamodel/notepage.h
        datamodel/recipe.h
        datamodel/recipe.cpp
        datamodel/task.cpp
        datamodel/task.h
        datamodel/todo.cpp
        datamodel/todo.h
        datamodel/todolist.cpp
        datamodel/todolist.h
        datamodel/toplevelitem.cpp
        datamodel/toplevelitem.h
        datastorage/applicationsettings.cpp
        datastorage/applicationsettings.h
        datastorage/cache.cpp
        datastorage/cache.h
        datastorage/copyitemquery.cpp
        datastorage/copyitemquery.h
        datastorage/deleteitemsquery.cpp
        datastorage/deleteitemsquery.h
        datastorage/getitemquery.cpp
        datastorage/getitemquery.h
        datastorage/getitemsquery.cpp
        datastorage/getitemsquery.h
        datastorage/getlibraryitemsuidsitemsquery.cpp
        datastorage/getlibraryitemsuidsitemsquery.h
        datastorage/getlibraryquery.cpp
        datastorage/getlibraryquery.h
        datastorage/insertorupdateitemsquery.cpp
        datastorage/insertorupdateitemsquery.h
        datastorage/itemsquery.cpp
        datastorage/itemsquery.h
        datastorage/librariesitemsquery.cpp
        datastorage/librariesitemsquery.h
        datastorage/libraryloader.cpp
        datastorage/libraryloader.h
        datastorage/loadlibraryquery.cpp
        datastorage/loadlibraryquery.h
        datastorage/movetodoquery.cpp
        datastorage/movetodoquery.h
        datastorage/movetaskquery.h
        datastorage/movetaskquery.cpp
        datastorage/promotetaskquery.cpp
        datastorage/promotetaskquery.h
        datastorage/updateitemquery.cpp
        datastorage/updateitemquery.h
        fileutils.cpp
        fileutils.h
        models/itemsmodel.cpp
        models/itemsmodel.h
        models/itemssortfiltermodel.cpp
        models/itemssortfiltermodel.h
        models/librariesmodel.cpp
        models/librariesmodel.h
        service/backgroundservice.cpp
        service/backgroundservice.h
        sync/account.cpp
        sync/account.h
        sync/dropboxaccount.cpp
        sync/dropboxaccount.h
        sync/dropboxsynchronizer.cpp
        sync/dropboxsynchronizer.h
        sync/nextcloudaccount.cpp
        sync/nextcloudaccount.h
        sync/owncloudaccount.cpp
        sync/owncloudaccount.h
        sync/remotelibraryinfo.cpp
        sync/remotelibraryinfo.h
        sync/synchronizer.cpp
        sync/synchronizer.h
        sync/syncjob.cpp
        sync/syncjob.h
        sync/syncrunner.cpp
        sync/syncrunner.h
        sync/synqclientsynchronizer.cpp
        sync/synqclientsynchronizer.h
        sync/webdavaccount.cpp
        sync/webdavaccount.h
        sync/webdavsynchronizer.cpp
        sync/webdavsynchronizer.h
        utilities/colors.cpp
        utilities/colors.h
        utilities/directorywatcher.cpp
        utilities/directorywatcher.h
        utilities/fileeditobserver.cpp
        utilities/fileeditobserver.h
        utilities/fontutils.cpp
        utilities/fontutils.h
        utilities/htmlsyntaxhighlighter.cpp
        utilities/htmlsyntaxhighlighter.h
        utilities/jsonutils.cpp
        utilities/jsonutils.h
        utilities/keystore.cpp
        utilities/keystore.h
        utilities/platformshareutils.cpp
        utilities/platformshareutils.h
        utilities/problem.cpp
        utilities/problem.h
        utilities/problemmanager.cpp
        utilities/problemmanager.h
        utilities/shareutils.cpp
        utilities/shareutils.h
        utilities/syntaxhighlighting.cpp
        utilities/syntaxhighlighting.h
        utilities/textutils.cpp
        utilities/textutils.h
        utilities/translations.cpp
        utilities/translations.h
        utilities/updateservice.cpp
        utilities/updateservice.h
        utilities/urlhandler.h
        utilities/urlhandler.cpp
        utilities/networkutils.h
        utilities/networkutils.cpp
        appstartup.cpp
        appstartup.h
        main.cpp


        # Include SimpleCrypt source:
        ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/simplecrypt/simplecrypt.cpp

        $<$<BOOL:${WIN32}>:OpenTodoList.rc>

        $<$<BOOL:${APPLE}>:res/OpenTodoList.icns>

        $<$<BOOL:${IOS}>:utilities/iosplatformshareutils.h>
        $<$<BOOL:${IOS}>:utilities/iosplatformshareutils.cpp>
        $<$<BOOL:${IOS}>:utilities/iosplatformshareutils.mm>
        $<$<BOOL:${IOS}>:utilities/ios/shareutilsdocviewcontroller.h>
        $<$<BOOL:${IOS}>:utilities/ios/shareutilsdocviewcontroller.mm>
        $<$<BOOL:${IOS}>:utilities/ios/application.mm>
        $<$<BOOL:${IOS}>:ios/Images.xcassets>
        $<$<BOOL:${IOS}>:ios/LaunchScreenOTL.storyboard>

        $<$<BOOL:${ANDROID}>:utilities/androidplatformshareutils.h>
        $<$<BOOL:${ANDROID}>:utilities/androidplatformshareutils.cpp>
        $<$<BOOL:${ANDROID}>:utilities/android/androidfiledialog.h>
        $<$<BOOL:${ANDROID}>:utilities/android/androidfiledialog.cpp>

        $<$<BOOL:${USE_GENERIC_SHARE_UTILS}>:utilities/genericplatformshareutils.h>
        $<$<BOOL:${USE_GENERIC_SHARE_UTILS}>:utilities/genericplatformshareutils.cpp>
)

qt_add_repc_sources(OpenTodoList service/backgroundservice.rep)
qt_add_repc_replicas(OpenTodoList service/backgroundservice.rep)

target_compile_features(
    OpenTodoList
    PRIVATE
        cxx_std_17
)

target_compile_options(
    OpenTodoList
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Werror -Wno-error=deprecated-declarations>
        $<$<BOOL:${IOS}>:-Wno-error=shorten-64-to-32>
)

# Set include paths
target_include_directories(
    OpenTodoList
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/simplecrypt

        # Include paths to "sub"-modules - needed for QML type registration:
        datamodel
        datastorage
        models
        service
        sync
        utilities
        utilities/android/
)

# Add compile definitions
target_compile_definitions(
    OpenTodoList
    PRIVATE
        $<$<BOOL:${OPENTODOLIST_WITH_UPDATE_SERVICE}>:OPENTODOLIST_WITH_UPDATE_SERVICE>
        $<$<BOOL:${OPENTODOLIST_WITH_APPIMAGE_EXTRAS}>:OPENTODOLIST_IS_APPIMAGE>
)

# Link against Qt libraries
set(
    OPENTODOLIST_LIBRARIES
        # Third party libraries.
        #
        # Note: When building these as part of the app build, we usually do
        # so as static libraries. With static libraries, the order of linking is
        # important, so make sure that we list them before the Qt libraries
        # further down!
        QLMDB::qlmdb-qt${QT_VERSION_MAJOR}
        SynqClient::synqclient-qt${QT_VERSION_MAJOR}
        qt${QT_VERSION_MAJOR}keychain
        KF5::SyntaxHighlighting

        # Qt libraries:
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::GuiPrivate
        Qt${QT_VERSION_MAJOR}::Qml
        Qt${QT_VERSION_MAJOR}::Quick
        Qt${QT_VERSION_MAJOR}::Xml
        Qt${QT_VERSION_MAJOR}::Concurrent
        Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::RemoteObjects
        Qt${QT_VERSION_MAJOR}::NetworkAuth
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::CorePrivate

        $<$<BOOL:${OPENTODOLIST_WITH_KNOTIFICATIONS}>:KF5::Notifications>
)
target_link_libraries(
    OpenTodoList
    PRIVATE
        ${OPENTODOLIST_LIBRARIES}
)

# Some versions of the syntax highlighting library introduced a "bug" which causes the
# path to the KF5 folder not to be included in the header search path. While some systems
# already rolled out a fix, some don't (of the time of writing this, the 28th of May 2022, this
# affects at least Fedora). As a workaround, we manually add the parent folder of the
# "KSyntaxHighlighting" to the search paths:
get_target_property(KF5_SYNTAXHIGHLIGHTING_INCLUDE_DIRS KF5::SyntaxHighlighting INTERFACE_INCLUDE_DIRECTORIES)
foreach(path ${KF5_SYNTAXHIGHLIGHTING_INCLUDE_DIRS})
    if(path MATCHES ".*KSyntaxHighlighting")
        get_filename_component(path ${path} DIRECTORY)
        target_include_directories(OpenTodoList PRIVATE ${path})
    endif()
endforeach()


set_source_files_properties(
    res/OpenTodoList.icns
    ios/Images.xcassets
    ios/LaunchScreenOTL.storyboard
    PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources
)

if(IOS)
    set(INFO_PLIST_FILE ${CMAKE_CURRENT_SOURCE_DIR}/ios/Info.plist.in)
else()
    set(INFO_PLIST_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in)
endif()

# Include Assets and special config for iOS
if(IOS)
    qt_import_plugins(
        OpenTodoList
        INCLUDE Qt::QIosOptionalPlugin_NSPhotoLibraryPlugin
    )
endif()

target_compile_definitions(
    OpenTodoList
    PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>
        $<$<BOOL:${OPENTODOLIST_WITH_KNOTIFICATIONS}>:OPENTODOLIST_WITH_KNOTIFICATIONS>
)

set_target_properties(
    OpenTodoList
    PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${INFO_PLIST_FILE}
        MACOSX_BUNDLE_GUI_IDENTIFIER net.rpdev.OpenTodoList
        MACOSX_BUNDLE_BUNDLE_NAME OpenTodoList
        MACOSX_BUNDLE_BUNDLE_VERSION "${OPENTODOLIST_VERSION_MAJOR}.${OPENTODOLIST_VERSION_MINOR}.${CI_PIPELINE_IID}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${OPENTODOLIST_VERSION_MAJOR}.${OPENTODOLIST_VERSION_MINOR}.${CI_PIPELINE_IID}"
        MACOSX_BUNDLE_LONG_VERSION_STRING "${OPENTODOLIST_VERSION_MAJOR}.${OPENTODOLIST_VERSION_MINOR}.${OPENTODOLIST_VERSION_PATCH}"
        MACOSX_BUNDLE_LAUNCHSCREEN_STORYBOARD LaunchScreen
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "${OPENTODOLIST_APPLE_CODE_SIGN_IDENTITY}"
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${OPENTODOLIST_APPLE_TEAM_ID}"
        XCODE_ATTRIBUTE_CODE_SIGN_STYLE "${OPENTODOLIST_APPLE_CODE_SIGN_STYLE}"
        XCODE_ATTRIBUTE_PROVISIONING_PROFILE_SPECIFIER "${OPENTODOLIST_IOS_PROVISIONING_PROFILE_SPECIFIER}"
        XCODE_PROPERTY_PRODUCT_BUNDLE_IDENTIFIER "net.rpdev.OpenTodoList"


        QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
        QT_ANDROID_TARGET_SDK_VERSION 35
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
                XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES
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

if(OPENTODOLIST_FLATPAK)
    target_compile_definitions(OpenTodoList PRIVATE OPENTODOLIST_FLATPAK)
endif()

# Install the app:
install(
    TARGETS OpenTodoList
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set(QML_IMPORT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/qml
    CACHE PATH "QML search path for project local files")

if (ANDROID)
    set(CMAKE_ANDROID_ARCH_ABI "${ANDROID_ABI}")
    add_android_openssl_libraries(OpenTodoList)
endif()

################################################################################
#
# Define "pseudo" library
#
# This is used to make linking against the objects of the app easier,
# apply other flags and filter out the unwanted "main" entry point.
#
################################################################################

add_library(opentodolist-common INTERFACE)
target_sources(
    opentodolist-common
    INTERFACE
        $<FILTER:$<TARGET_OBJECTS:OpenTodoList>,EXCLUDE,main.cpp>
)
target_include_directories(
    opentodolist-common
    INTERFACE
        $<TARGET_PROPERTY:OpenTodoList,INCLUDE_DIRECTORIES>
)
target_link_libraries(
    opentodolist-common
    INTERFACE
        ${OPENTODOLIST_LIBRARIES}
)


################################################################################
#
# Add Gallery Example
#
# This is just an example app used to develop the custom theme.
#
################################################################################

if(DESKTOP_MODE)
    qt_add_executable(gallery gallery.cpp)
    target_link_libraries(
        gallery
        PRIVATE
            Qt::Gui
            Qt::Qml
            Qt::Quick
            opentodolist-common
            opentodolist_actions_moduleplugin
            opentodolist_components_moduleplugin
            opentodolist_dialogs_moduleplugin
            opentodolist_menues_moduleplugin
            opentodolist_style_moduleplugin
            opentodolist_utils_moduleplugin
            opentodolist_widgets_moduleplugin
            opentodolist_windows_moduleplugin
            opentodolist_pages_moduleplugin
    )
    target_compile_definitions(
        gallery
        PRIVATE
            SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()


################################################################################
#
# Setup QML Module
#
# This adds some stuff to integrate the QML sources properly, allowing the
# project to be used both conveniently in QtCreator and also other editors
# like VSCode with the Qt plugins.
#
################################################################################

# Add aliases for some resources:
set_source_files_properties(
    ../3rdparty/ral-json/ral.json
    PROPERTIES
           QT_RESOURCE_ALIAS ral.json
)

# Add the module:
qt_add_qml_module(
    OpenTodoList
    URI
        net.rpdev.OpenTodoList
    IMPORTS
        QtQuick.Controls.Basic
    # OUTPUT_DIRECTORY
    #     # Store QML related files in a dedicated sub-folder of the build folder.
    #     # Otherwise, we have file name conflicts on some platforms (such as
    #     # Linux), where the executable name would be equal to the OpenTodoList
    #     # qml folder name.
    #     ${CMAKE_CURRENT_BINARY_DIR}/qml/OpenTodoList
    QML_FILES
        main.qml
    RESOURCES
        ../3rdparty/ral-json/ral.json
        3rdpartyinfo/comfortaa/3rdpartyinfo.json
        3rdpartyinfo/material-design-icons/3rdpartyinfo.json
        3rdpartyinfo/noto-color-emoji/3rdpartyinfo.json
        3rdpartyinfo/oxygen/3rdpartyinfo.json
        3rdpartyinfo/qt/3rdpartyinfo.json
        3rdpartyinfo/qtkeychain/3rdpartyinfo.json
        3rdpartyinfo/ral-json/3rdpartyinfo.json
        3rdpartyinfo/roboto/3rdpartyinfo.json
        3rdpartyinfo/simplecrypt/3rdpartyinfo.json
        3rdpartyinfo/SingleApplication/3rdpartyinfo.json
        3rdpartyinfo/syntax-highlighting/3rdpartyinfo.json
        icons/hicolor/128x128/apps/net.rpdev.OpenTodoList-Monochrome.png
        icons/hicolor/128x128/apps/net.rpdev.OpenTodoList.png
        icons/hicolor/16x16/apps/net.rpdev.OpenTodoList-Monochrome.png
        icons/hicolor/16x16/apps/net.rpdev.OpenTodoList.png
        icons/hicolor/256x256/apps/net.rpdev.OpenTodoList-Monochrome.png
        icons/hicolor/256x256/apps/net.rpdev.OpenTodoList.png
        icons/hicolor/32x32/apps/net.rpdev.OpenTodoList-Monochrome.png
        icons/hicolor/32x32/apps/net.rpdev.OpenTodoList.png
        icons/hicolor/64x64/apps/net.rpdev.OpenTodoList-Monochrome.png
        icons/hicolor/64x64/apps/net.rpdev.OpenTodoList.png
        OpenTodoList.notifyrc
        qtquickcontrols2.conf
        res/COPYING
        res/OpenTodoList.icns
        res/OpenTodoList.ico
        sample.png
        translations/languages.json
        translations/languages.json
        translations/languages.json
        translations/update-info.json
        translations/update-info.json
)

# Sub-modules are split into their own plugins - link against them here
# (see https://www.basyskom.de/en/how-to-use-modern-qml-tooling-in-practice/)
# for background):

target_link_libraries(
    OpenTodoList
    PRIVATE
        opentodolist_actions_moduleplugin
        opentodolist_components_moduleplugin
        opentodolist_dialogs_moduleplugin
        opentodolist_menues_moduleplugin
        opentodolist_style_moduleplugin
        opentodolist_utils_moduleplugin
        opentodolist_widgets_moduleplugin
        opentodolist_windows_moduleplugin
        opentodolist_pages_moduleplugin
)

add_subdirectory(net/rpdev/OpenTodoList/Actions)
add_subdirectory(net/rpdev/OpenTodoList/Components)
add_subdirectory(net/rpdev/OpenTodoList/Dialogs)
add_subdirectory(net/rpdev/OpenTodoList/Menues)
add_subdirectory(net/rpdev/OpenTodoList/Style)
add_subdirectory(net/rpdev/OpenTodoList/Utils)
add_subdirectory(net/rpdev/OpenTodoList/Widgets)
add_subdirectory(net/rpdev/OpenTodoList/Windows)
add_subdirectory(net/rpdev/OpenTodoList/Pages)

################################################################################
#
# Setup Translations
#
# The following sets up the translations integration, so we include translations
# in the app as well as adding additional targets to convert to and from
# *.po files (needed for POEditor).
#
################################################################################

file(GLOB TS_FILES CONFIGURE_DEPENDS translations/*.ts)
# Remove the English translation, its used as a "special" one to handle
# plurals properly:
list(FILTER TS_FILES EXCLUDE REGEX OpenTodoList-en\.ts)
qt_add_translations(
    TARGETS
        OpenTodoList
    SOURCE_TARGETS
        OpenTodoList OpenTodoList
    TS_FILES
        ${TS_FILES}
    RESOURCE_PREFIX
        "/qt/qml/net/rpdev/OpenTodoList/translations"
    PLURALS_TS_FILE
        translations/OpenTodoList-en.ts
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

################################################################################
#
# QML Language Server (qmlls) Hack
#
# This is required as of now to get qmlls working in VSCode. The qmlls process
# is started by VSCode with a default search path pointing to the root binary
# directory - there, it obviously won't find the needed QML module files.
# To aid helping it to find the needed module, add a symlink to the module in
# the root binary directory.
#
################################################################################

add_custom_command(
    TARGET
        OpenTodoList
    POST_BUILD
    COMMAND
        ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/OpenTodoList
    COMMAND
        ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_CURRENT_BINARY_DIR}/qml/OpenTodoList
            ${CMAKE_BINARY_DIR}/OpenTodoList
)
