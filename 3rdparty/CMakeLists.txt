include(ExternalProject)

# The path where to install the sub-modules within the build folder
set(OPENTODOLIST_MODULES_INSTALL_DIR "${CMAKE_BINARY_DIR}/modules")

# Common cache arguments to pass down:
set(MODULES_COMMON_CACHE_ARGS)
set(MODULES_COMMON_CACHE_VARIABLES_TO_PASS_THROUGH
    CMAKE_PREFIX_PATH
    ANDROID_NATIVE_API_LEVEL
    ANDROID_NDK
    CMAKE_TOOLCHAIN_FILE
    ANDROID_STL
    CMAKE_FIND_ROOT_PATH
    ANDROID_SDK
    ANDROID_ABI
    CMAKE_CXX_COMPILER_LAUNCHER
    CMAKE_C_COMPILER_LAUNCHER
    CMAKE_OSX_ARCHITECTURES
    CMAKE_OSX_DEPLOYMENT_TARGET
    CMAKE_C_COMPILER
    CMAKE_CXX_COMPILER
    CMAKE_BUILD_TYPE
    QT_QMAKE_EXECUTABLE
    KF5_HOST_TOOLING
)
foreach(variable ${MODULES_COMMON_CACHE_VARIABLES_TO_PASS_THROUGH})
    if(NOT ${variable} STREQUAL "")
        set(
            MODULES_COMMON_CACHE_ARGS
            ${MODULES_COMMON_CACHE_ARGS}
            -D${variable}:STRING=${${variable}}
        )
    endif()
endforeach()


# ECM (needed for KF Syntax Highlighting and optional for other sub-modules)
if(NOT OPENTODOLIST_USE_SYSTEM_ECM)
    ExternalProject_Add(
        build_ecm
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/KDE/extra-cmake-modules
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${OPENTODOLIST_MODULES_INSTALL_DIR}
        CMAKE_CACHE_ARGS
            ${MODULES_COMMON_CACHE_ARGS}
    )
else()
    add_custom_target(build_ecm)
endif()


# KF Syntax Highlighting
if(NOT OPENTODOLIST_USE_SYSTEM_KF_SYNTAX_HIGHLIGHTING)
    set(
        KF_SYNTAX_HIGHLIGHTING_SOURCE_ARGS
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/KDE/syntax-highlighting
    )
    if(CMAKE_GENERATOR STREQUAL "Xcode")
        # There is a bug in KDE Syntax Highlighting which prevents it from building with the
        # "new" Xcode build system. Hence, build this dependency with ninja instead
        set(KDE_SH_GENERATOR Ninja)
    else()
        set(KDE_SH_GENERATOR ${CMAKE_GENERATOR})
    endif()
    if(IOS)
        # On iOS, KDE Syntax Highlighting does not build out of the box. This is because the
        # build does not properly detect that we are cross-compiling. For this reason,
        # we manually build the module once for the host and then pass the indexer executable
        # into the iOS build of the module.
        #
        # In addition, we have to patch the source tree. We do this by doing a git clone of the
        # submodule and patching this one in place.
        # Note that this breaks if someone tries to build the iOS build from a release
        # tarball. This is unfortunate but everything else will get complicated.

        if(NOT QT_HOST_PATH)
            message(FATAL_ERROR "QT_HOST_PATH is not set, but we need a host Qt to continue!")
        endif()

        ExternalProject_Add(
            build_kf_syntax_highlighting_host
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/KDE/syntax-highlighting
            CMAKE_GENERATOR
                ${KDE_SH_GENERATOR}
            CMAKE_ARGS
                -DCMAKE_TOOLCHAIN_FILE=${QT_HOST_PATH}/lib/cmake/Qt6/qt.toolchain.cmake
                -DCMAKE_INSTALL_PREFIX=${OPENTODOLIST_MODULES_INSTALL_DIR}-host
                -DQT_MAJOR_VERSION=${QT_VERSION_MAJOR}
                -DQRC_SYNTAX=ON
                -DNO_STANDARD_PATHS=ON
                -DBUILD_SHARED_LIBS=OFF
            CMAKE_CACHE_ARGS
                "-DCMAKE_PREFIX_PATH:STRING=${OPENTODOLIST_MODULES_INSTALL_DIR};${QT_HOST_PATH}" # ECM + Host Qt
                -DCMAKE_OSX_SYSROOT:STRING=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk
        )
        ExternalProject_Add_StepDependencies(build_kf_syntax_highlighting_host build build_ecm)

        set(KDE_SH_CROSSCOMPILING_EXTRA_ARGS
            -DKATEHIGHLIGHTINGINDEXER_EXECUTABLE:STRING=${CMAKE_CURRENT_BINARY_DIR}/build_kf_syntax_highlighting_host-prefix/src/build_kf_syntax_highlighting_host-build/bin/katehighlightingindexer
            -DCMAKE_CROSSCOMPILING:BOOL=ON
            -DIOS:BOOL=ON
        )
        set(KDE_SH_CROSSCOMPILING_EXTRA_TARGETS build_kf_syntax_highlighting_host)

        execute_process(
            COMMAND
                git describe --tags
            WORKING_DIRECTORY
                ${CMAKE_CURRENT_SOURCE_DIR}/KDE/syntax-highlighting
            OUTPUT_VARIABLE
                KF5_SYNTAX_HIGHLIGHTING_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        set(KDE_SH_CROSSCOMPILING_IOS_EXTRA_PARAMS
            GIT_REPOSITORY
                ${CMAKE_CURRENT_SOURCE_DIR}/KDE/syntax-highlighting
            GIT_TAG
                ${KF5_SYNTAX_HIGHLIGHTING_VERSION}
            PATCH_COMMAND
                ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/KDE/syntax-highlighting-ios.patch <SOURCE_DIR> && git checkout <SOURCE_DIR> && git apply <SOURCE_DIR>/syntax-highlighting-ios.patch
        )
    unset(KF_SYNTAX_HIGHLIGHTING_SOURCE_ARGS)

    endif()
    set(
        KF5_PREFIX_PATHS
        ${OPENTODOLIST_MODULES_INSTALL_DIR}
        ${CMAKE_PREFIX_PATH}
    )
    ExternalProject_Add(
        build_kf_syntax_highlighting
        ${KF_SYNTAX_HIGHLIGHTING_SOURCE_ARGS}
        CMAKE_GENERATOR
            ${KDE_SH_GENERATOR}
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${OPENTODOLIST_MODULES_INSTALL_DIR}
            -DQT_MAJOR_VERSION=${QT_VERSION_MAJOR}
            -DQRC_SYNTAX=ON
            -DNO_STANDARD_PATHS=ON
            -DBUILD_SHARED_LIBS=OFF
            -DQt${QT_VERSION_MAJOR}_DIR=${QT_DIR}
            -DKF_IGNORE_PLATFORM_CHECK=ON
        CMAKE_CACHE_ARGS
            ${MODULES_COMMON_CACHE_ARGS}
            -DCMAKE_PREFIX_PATH:STRING=${KF5_PREFIX_PATHS}
            -DECM_DIR:STRING=${OPENTODOLIST_MODULES_INSTALL_DIR}/share/ECM/cmake
            ${KDE_SH_CROSSCOMPILING_EXTRA_ARGS}
        ${KDE_SH_CROSSCOMPILING_IOS_EXTRA_PARAMS}
    )
    ExternalProject_Add_StepDependencies(build_kf_syntax_highlighting configure build_ecm ${KDE_SH_CROSSCOMPILING_EXTRA_TARGETS})

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
            -DBUILD_TRANSLATIONS=OFF
            -DBUILD_SHARED_LIBS=OFF
        CMAKE_CACHE_ARGS
            ${MODULES_COMMON_CACHE_ARGS}
    )
    ExternalProject_Add_StepDependencies(build_qtkeychain build build_ecm)

    add_library(qtkeychain_imported INTERFACE)
    add_dependencies(qtkeychain_imported build_qtkeychain)
    target_link_directories(
        qtkeychain_imported
        INTERFACE
            ${OPENTODOLIST_MODULES_INSTALL_DIR}/lib
    )
    target_link_libraries(qtkeychain_imported INTERFACE -lqt${QT_VERSION_MAJOR}keychain)
    if(APPLE)
        target_link_libraries(
            qtkeychain_imported 
            INTERFACE
                "-framework Foundation"
                "-framework Security"
        )
    endif()
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
        CMAKE_CACHE_ARGS
            ${MODULES_COMMON_CACHE_ARGS}
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
        CMAKE_CACHE_ARGS
            ${MODULES_COMMON_CACHE_ARGS}
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
