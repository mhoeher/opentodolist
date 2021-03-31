# Add the library
add_library(opentodolist-common)


if(ANDROID)
    set(LIB_HEADERS_ANDROID
        ./utils/android/androidfiledialog.h)
    set(LIB_SOURCES_ANDROID
        ./utils/android/androidfiledialog.cpp)
endif()


set(LIB_HEADERS
    abstractitemmodel.h
    application.h
    datamodel/complexitem.h
    datamodel/image.h
    datamodel/item.h
    datamodel/library.h
    datamodel/note.h
    datamodel/notepage.h
    datamodel/task.h
    datamodel/todo.h
    datamodel/todolist.h
    datamodel/toplevelitem.h
    datastorage/applicationsettings.h
    datastorage/cache.h
    datastorage/deleteitemsquery.h
    datastorage/getitemquery.h
    datastorage/getitemsquery.h
    datastorage/getlibraryitemsuidsitemsquery.h
    datastorage/getlibraryquery.h
    datastorage/insertorupdateitemsquery.h
    datastorage/itemsquery.h
    datastorage/librariesitemsquery.h
    datastorage/libraryloader.h
    datastorage/movetodoquery.h
    datastorage/promotetaskquery.h
    fileutils.h
    models/itemsmodel.h
    models/itemssortfiltermodel.h
    models/librariesmodel.h
    opentodolistqmlextensionsplugin.h
    service/backgroundservice.h
    sync/account.h
    sync/synchronizer.h
    sync/syncjob.h
    sync/syncrunner.h
    sync/webdavsynchronizer.h
    utils/directorywatcher.h
    utils/jsonutils.h
    utils/keystore.h
    utils/problem.h
    utils/problemmanager.h
    utils/syntaxhighlighting.h
    utils/translations.h
    utils/updateservice.h
    utils/htmlsyntaxhighlighter.h

    ${LIB_HEADERS_ANDROID}
)

set(LIB_SOURCES
    application.cpp
    datamodel/complexitem.cpp
    datamodel/image.cpp
    datamodel/item.cpp
    datamodel/library.cpp
    datamodel/note.cpp
    datamodel/notepage.cpp
    datamodel/task.cpp
    datamodel/todo.cpp
    datamodel/todolist.cpp
    datamodel/toplevelitem.cpp
    datastorage/applicationsettings.cpp
    datastorage/cache.cpp
    datastorage/deleteitemsquery.cpp
    datastorage/getitemquery.cpp
    datastorage/getitemsquery.cpp
    datastorage/getlibraryitemsuidsitemsquery.cpp
    datastorage/getlibraryquery.cpp
    datastorage/insertorupdateitemsquery.cpp
    datastorage/itemsquery.cpp
    datastorage/librariesitemsquery.cpp
    datastorage/libraryloader.cpp
    datastorage/movetodoquery.cpp
    datastorage/promotetaskquery.cpp
    fileutils.cpp
    models/itemsmodel.cpp
    models/itemssortfiltermodel.cpp
    models/librariesmodel.cpp
    opentodolistqmlextensionsplugin.cpp
    service/backgroundservice.cpp
    sync/account.cpp
    sync/synchronizer.cpp
    sync/syncjob.cpp
    sync/syncrunner.cpp
    sync/webdavsynchronizer.cpp
    utils/directorywatcher.cpp
    utils/jsonutils.cpp
    utils/keystore.cpp
    utils/problem.cpp
    utils/problemmanager.cpp
    utils/syntaxhighlighting.cpp
    utils/translations.cpp
    utils/updateservice.cpp
    utils/htmlsyntaxhighlighter.cpp

    ${LIB_SOURCES_ANDROID}

    # Include SimpleCrypt source:
    ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/simplecrypt/simplecrypt.cpp
)

if(OPENTODOLIST_USE_SYSTEM_KF_SYNTAX_HIGHLIGHTING)
    target_link_libraries(opentodolist-common PRIVATE KF5::SyntaxHighlighting)
    target_compile_definitions(opentodolist-common PRIVATE OPENTODOLIST_USE_SYSTEM_KF_SYNTAX_HIGHLIGHTING)
else()
    include(qmake/kf5-syntax-highlighting.cmake)
    set(LIB_HEADERS ${LIB_HEADERS} ${KF5_SYNTAXHIGHLIGHTING_HEADERS})
    set(
        LIB_SOURCES ${LIB_SOURCES} 
        ${KF5_SYNTAXHIGHLIGHTING_SOURCES} ${KF5_SYNTAXHIGHLIGHTING_RESOURCES}
    )
endif()




qt5_generate_repc(LIB_SOURCES service/backgroundservice.rep SOURCE)
qt5_generate_repc(LIB_SOURCES service/backgroundservice.rep REPLICA)

target_sources(
    opentodolist-common
    PRIVATE
        ${LIB_HEADERS}
        ${LIB_SOURCES}
)

# Set include paths
target_include_directories(
    opentodolist-common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty
        ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/simplecrypt
        ${CMAKE_CURRENT_BINARY_DIR}/../3rdparty/qtkeychain
        ${CMAKE_CURRENT_BINARY_DIR}
    PRIVATE
        ${KF5_SYNTAXHIGHLIGHTING_INCLUDEPATHS}
)

# Add compile definitions
target_compile_definitions(
    opentodolist-common 
    PRIVATE 
        ${LIB_DEFINES}
        ${KF5_SYNTAXHIGHLIGHTING_DEFINES}
)
if(OPENTODOLIST_USE_SYSTEM_QTKEYCHAIN)
    target_compile_definitions(opentodolist-common PRIVATE OPENTODOLIST_USE_SYSTEM_QTKEYCHAIN)
endif()

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

        # Third party libraries:
        QLMDB::qlmdb-qt${QT_VERSION_MAJOR}
        SynqClient::synqclient-qt${QT_VERSION_MAJOR}

        # Optional libraries
        ${LIB_LIBRARIES}
)

if(ANDROID)
    target_link_libraries(
        opentodolist-common

        PUBLIC
            Qt${QT_VERSION_MAJOR}::AndroidExtras
    )

    if (ANDROID_ABI STREQUAL "arm64-v8a")
        # TODO: If possible, upgrade and remove this
        # At least with Qt 5.15, we get linking errors on ARM64 using cmake. Hence, explicitly
        # link against the missing libraries:
        target_link_libraries(
            opentodolist-common

            PUBLIC
                ${ANDROID_TOOLCHAIN_ROOT}/sysroot/usr/lib/aarch64-linux-android/${ANDROID_NATIVE_API_LEVEL}/libGLESv2.so
                ${ANDROID_TOOLCHAIN_ROOT}/sysroot/usr/lib/aarch64-linux-android/${ANDROID_NATIVE_API_LEVEL}/liblog.so
                ${ANDROID_TOOLCHAIN_ROOT}/sysroot/usr/lib/aarch64-linux-android/${ANDROID_NATIVE_API_LEVEL}/libz.so
                ${ANDROID_TOOLCHAIN_ROOT}/sysroot/usr/lib/aarch64-linux-android/libc++_shared.so
        )
    endif()
else()
    target_link_libraries(
        opentodolist-common

        PUBLIC
            qt${QT_VERSION_MAJOR}keychain
    )
endif()



install(
    TARGETS opentodolist-common
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
