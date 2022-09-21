include(ExternalProject)

# The path where to install the sub-modules within the build folder
set(OPENTODOLIST_MODULES_INSTALL_DIR "${CMAKE_BINARY_DIR}/modules")

# ECM (needed for KF Syntax Highlighting and optional for other sub-modules)
if(NOT OPENTODOLIST_USE_SYSTEM_ECM)
    ExternalProject_Add(
        build_ecm
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/KDE/extra-cmake-modules
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${OPENTODOLIST_MODULES_INSTALL_DIR}
    )
else()
    add_custom_target(build_ecm)
endif()


# KF Syntax Highlighting
if(NOT OPENTODOLIST_USE_SYSTEM_KF_SYNTAX_HIGHLIGHTING)
    ExternalProject_Add(
        build_kf_syntax_highlighting
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/KDE/syntax-highlighting
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${OPENTODOLIST_MODULES_INSTALL_DIR}
            -DCMAKE_PREFIX_PATH=${OPENTODOLIST_MODULES_INSTALL_DIR}
            -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
            -DQT_MAJOR_VERSION=${QT_VERSION_MAJOR}
            -DQRC_SYNTAX=ON
            -DNO_STANDARD_PATHS=ON
            -DBUILD_SHARED_LIBS=OFF
    )
    ExternalProject_Add_StepDependencies(build_kf_syntax_highlighting build build_ecm)

    add_library(SyntaxHighlighting_imported INTERFACE)
    add_dependencies(SyntaxHighlighting_imported build_kf_syntax_highlighting)
    target_link_directories(
        SyntaxHighlighting_imported
        INTERFACE
            ${OPENTODOLIST_MODULES_INSTALL_DIR}/lib
    )
    target_link_libraries(SyntaxHighlighting_imported INTERFACE -lKF5SyntaxHighlighting)
    target_include_directories(
        SyntaxHighlighting_imported
        INTERFACE
            ${OPENTODOLIST_MODULES_INSTALL_DIR}/include/KF${QT_VERSION_MAJOR}
    )
    add_library(KF5::SyntaxHighlighting ALIAS SyntaxHighlighting_imported)
else()
    add_custom_target(build_kf_syntax_highlighting)
    find_package(KF5SyntaxHighlighting)
endif()


# QtKeychain
if(NOT ANDROID AND NOT OPENTODOLIST_USE_SYSTEM_QTKEYCHAIN)
    ExternalProject_Add(
        build_qtkeychain
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qtkeychain
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${OPENTODOLIST_MODULES_INSTALL_DIR}
            -DCMAKE_PREFIX_PATH=${OPENTODOLIST_MODULES_INSTALL_DIR}
            -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
            $<$<STREQUAL:${QT_VERSION_MAJOR},6>:-DBUILD_WITH_QT6=ON>
            -DBUILD_TRANSLATIONS=ON
            -DBUILD_SHARED_LIBS=OFF
    )
    ExternalProject_Add_StepDependencies(build_qtkeychain build build_ecm)

    add_library(qtkeychain_imported INTERFACE)
    add_dependencies(qtkeychain_imported build_synqclient)
    target_link_directories(
        qtkeychain_imported
        INTERFACE
            ${OPENTODOLIST_MODULES_INSTALL_DIR}/lib
    )
    target_link_libraries(qtkeychain_imported INTERFACE -lqt${QT_VERSION_MAJOR}keychain)
    target_include_directories(
        qtkeychain_imported
        INTERFACE
            ${OPENTODOLIST_MODULES_INSTALL_DIR}/include/
    )
    add_library(qt${QT_VERSION_MAJOR}keychain ALIAS qtkeychain_imported)
else()
    add_custom_target(build_qtkeychain)
    if(NOT ANDROID)
        find_package(Qt${QT_VERSION_MAJOR}Keychain REQUIRED)
    else()
        # For Android, define a "fake" library that does nothing:
        add_library(qt${QT_VERSION_MAJOR}keychain INTERFACE)
    endif()
endif()


# QLMDB
if(NOT OPENTODOLIST_USE_SYSTEM_QLMDB)
    ExternalProject_Add(
        build_qlmdb
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qlmdb
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${OPENTODOLIST_MODULES_INSTALL_DIR}
            -DCMAKE_PREFIX_PATH=${OPENTODOLIST_MODULES_INSTALL_DIR}
            -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
            -DQLMDB_WITH_STATIC_LIBS=ON
            -DQLMDB_WITHOUT_TESTS=ON
            -DQLMDB_QT_VERSIONS=Qt${QT_VERSION_MAJOR}
    )
    ExternalProject_Add_StepDependencies(build_qlmdb build build_ecm)

    add_library(qlmdb_imported INTERFACE)
    add_dependencies(qlmdb_imported build_qlmdb)
    target_link_directories(
        qlmdb_imported
        INTERFACE
            ${OPENTODOLIST_MODULES_INSTALL_DIR}/lib
    )
    target_link_libraries(qlmdb_imported INTERFACE -lqlmdb-qt${QT_VERSION_MAJOR})
    target_include_directories(
        qlmdb_imported
        INTERFACE
            ${OPENTODOLIST_MODULES_INSTALL_DIR}/include/qlmdb-qt${QT_VERSION_MAJOR}
    )
    add_library(QLMDB::qlmdb-qt${QT_VERSION_MAJOR} ALIAS qlmdb_imported)
else()
    add_custom_target(build_qlmdb)
    find_package(QLMDB-qt${QT_VERSION_MAJOR} REQUIRED)
endif()


# SynqClient
if(NOT OPENTODOLIST_USE_SYSTEM_SYNQCLIENT)
    ExternalProject_Add(
        build_synqclient
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/synqclient
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${OPENTODOLIST_MODULES_INSTALL_DIR}
            -DCMAKE_PREFIX_PATH=${OPENTODOLIST_MODULES_INSTALL_DIR}
            -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
            -DBUILD_SHARED_LIBS=OFF
            -DSYNQCLIENT_WITHOUT_TESTS=ON
            -DSYNQCLIENT_QT_VERSIONS=Qt${QT_VERSION_MAJOR}
    )
    ExternalProject_Add_StepDependencies(build_synqclient build build_ecm)

    add_library(synqclient_imported INTERFACE)
    add_dependencies(synqclient_imported build_synqclient)
    target_link_directories(
        synqclient_imported
        INTERFACE
            ${OPENTODOLIST_MODULES_INSTALL_DIR}/lib
    )
    target_link_libraries(synqclient_imported INTERFACE -lsynqclient-qt${QT_VERSION_MAJOR})
    target_include_directories(
        synqclient_imported
        INTERFACE
            ${OPENTODOLIST_MODULES_INSTALL_DIR}/include/synqclient-qt${QT_VERSION_MAJOR}
    )
    add_library(SynqClient::synqclient-qt${QT_VERSION_MAJOR} ALIAS synqclient_imported)
else()
    add_custom_target(build_synqclient)
    find_package(SynqClient-qt${QT_VERSION_MAJOR} REQUIRED)
endif()
