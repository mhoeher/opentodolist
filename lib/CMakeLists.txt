# Add the library
add_library(opentodolist-common)

target_sources(
    opentodolist-common
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
        opentodolistqmlextensionsplugin.cpp
        opentodolistqmlextensionsplugin.h
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
        utils/colors.cpp
        utils/colors.h
        utils/directorywatcher.cpp
        utils/directorywatcher.h
        utils/fileeditobserver.cpp
        utils/fileeditobserver.h
        utils/fontutils.cpp
        utils/fontutils.h
        utils/htmlsyntaxhighlighter.cpp
        utils/htmlsyntaxhighlighter.h
        utils/jsonutils.cpp
        utils/jsonutils.h
        utils/keystore.cpp
        utils/keystore.h
        utils/platformshareutils.cpp
        utils/platformshareutils.h
        utils/problem.cpp
        utils/problem.h
        utils/problemmanager.cpp
        utils/problemmanager.h
        utils/shareutils.cpp
        utils/shareutils.h
        utils/syntaxhighlighting.cpp
        utils/syntaxhighlighting.h
        utils/textutils.cpp
        utils/textutils.h
        utils/translations.cpp
        utils/translations.h
        utils/updateservice.cpp
        utils/updateservice.h
        utils/urlhandler.h
        utils/urlhandler.cpp

        # Include SimpleCrypt source:
        ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/simplecrypt/simplecrypt.cpp
)

if(IOS)
    target_sources(
        opentodolist-common
        PRIVATE
            utils/iosplatformshareutils.h
            utils/iosplatformshareutils.cpp
            utils/iosplatformshareutils.mm
            utils/ios/shareutilsdocviewcontroller.h
            utils/ios/shareutilsdocviewcontroller.mm
            utils/ios/application.mm
    )
elseif(ANDROID)
    target_sources(
        opentodolist-common
        PRIVATE
            utils/androidplatformshareutils.h
            utils/androidplatformshareutils.cpp
    )
else()
    target_sources(
        opentodolist-common
        PRIVATE
            utils/genericplatformshareutils.h
            utils/genericplatformshareutils.cpp
    )
endif()


target_compile_features(
    opentodolist-common PRIVATE cxx_std_14
)

target_compile_options(
    opentodolist-common
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Werror>
)

if(ANDROID)
    target_sources(
        opentodolist-common
        PRIVATE
            utils/android/androidfiledialog.h
            utils/android/androidfiledialog.cpp
    )
endif()

# Some versions of the syntax highlighting library introduced a "bug" which causes the
# path to the KF5 folder not to be included in the header search path. While some systems
# already rolled out a fix, some don't (of the time of writing this, the 28th of May 2022, this
# affects at least Fedora). As a workaround, we manually add the parent folder of the
# "KSyntaxHighlighting" to the search paths:
get_target_property(KF5_SYNTAXHIGHLIGHTING_INCLUDE_DIRS KF5::SyntaxHighlighting INTERFACE_INCLUDE_DIRECTORIES)
foreach(path ${KF5_SYNTAXHIGHLIGHTING_INCLUDE_DIRS})
    if(path MATCHES ".*KSyntaxHighlighting")
        get_filename_component(path ${path} DIRECTORY)
        target_include_directories(opentodolist-common PRIVATE ${path})
    endif()
endforeach()

if("${QT_VERSION_MAJOR}" STREQUAL "5")
    unset(REPC_SOURCES)
    qt5_generate_repc(REPC_SOURCES service/backgroundservice.rep SOURCE)
    qt5_generate_repc(REPC_SOURCES service/backgroundservice.rep REPLICA)
    target_sources(
        opentodolist-common
        PRIVATE
        ${REPC_SOURCES}
    )
    unset(REPC_SOURCES)
else()
    qt_add_repc_sources(opentodolist-common service/backgroundservice.rep)
    qt_add_repc_replicas(opentodolist-common service/backgroundservice.rep)
endif()

# Set include paths
target_include_directories(
    opentodolist-common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty
        ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/simplecrypt
        ${CMAKE_CURRENT_BINARY_DIR}/../3rdparty/qtkeychain
        ${CMAKE_CURRENT_BINARY_DIR}
)

# Add compile definitions
target_compile_definitions(
    opentodolist-common
    PRIVATE
        $<$<BOOL:${OPENTODOLIST_WITH_UPDATE_SERVICE}>:OPENTODOLIST_WITH_UPDATE_SERVICE>
        $<$<BOOL:${OPENTODOLIST_WITH_APPIMAGE_EXTRAS}>:OPENTODOLIST_IS_APPIMAGE>
)

# Link against Qt libraries
target_link_libraries(opentodolist-common
    PUBLIC
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
        Qt${QT_VERSION_MAJOR}::Qml
        Qt${QT_VERSION_MAJOR}::Quick
        Qt${QT_VERSION_MAJOR}::Xml
        Qt${QT_VERSION_MAJOR}::Concurrent
        Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::RemoteObjects
        Qt${QT_VERSION_MAJOR}::NetworkAuth
        $<$<BOOL:${ANDROID}>:Qt${QT_VERSION_MAJOR}::CorePrivate>
)

install(
    TARGETS opentodolist-common
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
