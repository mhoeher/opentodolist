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
        datastorage/movetodoquery.cpp
        datastorage/movetodoquery.h
        datastorage/promotetaskquery.cpp
        datastorage/promotetaskquery.h
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
        sync/dropboxsynchronizer.cpp
        sync/dropboxsynchronizer.h
        sync/synchronizer.cpp
        sync/synchronizer.h
        sync/syncjob.cpp
        sync/syncjob.h
        sync/syncrunner.cpp
        sync/syncrunner.h
        sync/webdavsynchronizer.cpp
        sync/webdavsynchronizer.h
        utils/colors.h
        utils/colors.cpp
        utils/directorywatcher.cpp
        utils/directorywatcher.h
        utils/htmlsyntaxhighlighter.cpp
        utils/htmlsyntaxhighlighter.h
        utils/jsonutils.cpp
        utils/jsonutils.h
        utils/keystore.cpp
        utils/keystore.h
        utils/problem.cpp
        utils/problem.h
        utils/problemmanager.cpp
        utils/problemmanager.h
        utils/syntaxhighlighting.cpp
        utils/syntaxhighlighting.h
        utils/translations.cpp
        utils/translations.h
        utils/updateservice.cpp
        utils/updateservice.h
        sync/webdavaccount.h
        sync/webdavaccount.cpp
        sync/nextcloudaccount.h
        sync/nextcloudaccount.cpp
        sync/owncloudaccount.h
        sync/owncloudaccount.cpp
        sync/dropboxaccount.h
        sync/dropboxaccount.cpp
        sync/remotelibraryinfo.h
        sync/remotelibraryinfo.cpp

        # Include SimpleCrypt source:
        ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/simplecrypt/simplecrypt.cpp
)

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
        $<$<BOOL:OPENTODOLIST_WITH_UPDATE_SERVICE>:OPENTODOLIST_WITH_UPDATE_SERVICE>
        $<$<BOOL:OPENTODOLIST_WITH_APPIMAGE_EXTRAS>:OPENTODOLIST_IS_APPIMAGE>
)

# Link against Qt libraries
target_link_libraries(opentodolist-common
    PUBLIC
        # Qt libraries:
        Qt${QT_VERSION_MAJOR}::Qml
        Qt${QT_VERSION_MAJOR}::Quick
        Qt${QT_VERSION_MAJOR}::Xml
        Qt${QT_VERSION_MAJOR}::Concurrent
        Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::RemoteObjects
        Qt${QT_VERSION_MAJOR}::NetworkAuth

        $<$<BOOL:${ANDROID}>:Qt${QT_VERSION_MAJOR}::AndroidExtras>

        # TODO: If possible, upgrade and remove this
        # At least with Qt 5.15, we get linking errors on ARM64 using cmake. Hence, explicitly
        # link against the missing libraries:
        $<$<STREQUAL:${ANDROID_ABI},"arm64-v8a">:${ANDROID_TOOLCHAIN_ROOT}/sysroot/usr/lib/aarch64-linux-android/${ANDROID_NATIVE_API_LEVEL}/libGLESv2.so>
        $<$<STREQUAL:${ANDROID_ABI},"arm64-v8a">:${ANDROID_TOOLCHAIN_ROOT}/sysroot/usr/lib/aarch64-linux-android/${ANDROID_NATIVE_API_LEVEL}/liblog.so>
        $<$<STREQUAL:${ANDROID_ABI},"arm64-v8a">:${ANDROID_TOOLCHAIN_ROOT}/sysroot/usr/lib/aarch64-linux-android/${ANDROID_NATIVE_API_LEVEL}/libz.so>
        $<$<STREQUAL:${ANDROID_ABI},"arm64-v8a">:${ANDROID_TOOLCHAIN_ROOT}/sysroot/usr/lib/aarch64-linux-android/libc++_shared.so>

        # Third party libraries:
        QLMDB::qlmdb-qt${QT_VERSION_MAJOR}
        SynqClient::synqclient-qt${QT_VERSION_MAJOR}
        qt${QT_VERSION_MAJOR}keychain
        KF5::SyntaxHighlighting

        # Optional libraries
        ${LIB_LIBRARIES}
)

install(
    TARGETS opentodolist-common
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
