include(CMakeParseArguments)

set(QT_ANDROID_MK_APK_DIR ${CMAKE_CURRENT_LIST_DIR})

function(qt_android_build_apk)
    set(options)
    set(oneValueArgs
        TARGET PACKAGE_NAME ANDROID_EXTRA_FILES QML_ROOT_PATH
        SDK_BUILD_TOOLS_VERSION EXTRA_LIBS)
    set(multiValueArgs)
    cmake_parse_arguments(
        APK "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Gather required variables to create the configuration file.

    find_package(Qt5 COMPONENTS Core REQUIRED)
    # Qt5Core_DIR now points to $Qt5InstallPrefix/lib/cmake/Qt5Core, so
    # we get the parent directory three times:
    get_filename_component(QT5_INSTALL_PREFIX "${Qt5Core_DIR}/../../.." ABSOLUTE)
    message("Qt5 installed in ${QT5_INSTALL_PREFIX}")

    # Adjust QML root path if not set:
    if(NOT APK_QML_ROOT_PATH)
        set(APK_QML_ROOT_PATH $<TARGET_FILE_DIR:${APK_TARGET}>)
    endif()

    # Get he toolchain prefix, i.e. the folder name within the
    # toolchains/ folder without the compiler version
    # APK_NDK_TOOLCHAIN_PREFIX
    file(RELATIVE_PATH APK_NDK_TOOLCHAIN_PREFIX ${CMAKE_ANDROID_NDK} ${CMAKE_CXX_COMPILER})
    string(REPLACE "/" ";" APK_NDK_TOOLCHAIN_PREFIX ${APK_NDK_TOOLCHAIN_PREFIX})
    list(GET APK_NDK_TOOLCHAIN_PREFIX 1 APK_NDK_TOOLCHAIN_PREFIX)
    string(LENGTH "-${CMAKE_ANDROID_NDK_TOOLCHAIN_VERSION}" VERSION_LENGTH)
    string(LENGTH "${APK_NDK_TOOLCHAIN_PREFIX}" FOLDER_LENGTH)
    math(EXPR PREFIX_LENGTH ${FOLDER_LENGTH}-${VERSION_LENGTH})
    string(SUBSTRING "${APK_NDK_TOOLCHAIN_PREFIX}" 0 ${PREFIX_LENGTH} APK_NDK_TOOLCHAIN_PREFIX)

    # Get path to the target:
    set(APK_TARGET_OUTPUT_FILENAME $<TARGET_FILE:${APK_TARGET}>)

    # Get Android SDK build tools version:
    if(NOT APK_SDK_BUILD_TOOLS_VERSION)
        file(GLOB sdk_versions RELATIVE ${ANDROID_SDK_ROOT}/build-tools
            ${ANDROID_SDK_ROOT}/build-tools/*)
        list(GET sdk_versions -1 APK_SDK_BUILD_TOOLS_VERSION)
    endif()

    # Step 1: Create an intermediate config file. At this point,
    # the generator expressions will we use are not yet resolved.
    configure_file(
        ${QT_ANDROID_MK_APK_DIR}/qt-android-deployment.json.in
        ${CMAKE_CURRENT_BINARY_DIR}/${APK_TARGET}-config.json.pre)

    # Step 2: Run file(CONFIGURE ...) to create the final config JSON
    # with generator expressions resolved:
    file(
        GENERATE
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${APK_TARGET}-config.json
        INPUT ${CMAKE_CURRENT_BINARY_DIR}/${APK_TARGET}-config.json.pre)

    # Step 3: Create a custom target which will build our APK:
    set(APK_DIR ${CMAKE_CURRENT_BINARY_DIR}/${APK_TARGET}-apk-build)
    if(NOT APK_ANDROID_EXTRA_FILES)
        set(
            APK_ANDROID_EXTRA_FILES
            ${QT5_INSTALL_PREFIX}/src/android/templates/)
    endif()
    if(JAVA_HOME)
        set(ANDROIDDEPLOYQT_EXTRA_ARGS
            ${ANDROIDDEPLOYQT_EXTRA_ARGS} --jdk '${JAVA_HOME}')
    endif()
    if(${CMAKE_BUILD_TYPE} STREQUAL Release)
        set(ANDROIDDEPLOYQT_EXTRA_ARGS
            ${ANDROIDDEPLOYQT_EXTRA_ARGS} --release)
        set(APK_FILENAME ${APK_TARGET}-apk-build-release-unsigned.apk)
    else()
        set(APK_FILENAME ${APK_TARGET}-apk-build-debug.apk)
    endif()
    add_custom_target(
        ${APK_TARGET}-apk
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${APK_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${QT5_INSTALL_PREFIX}/src/android/templates/
            ${APK_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${APK_ANDROID_EXTRA_FILES}/
            ${APK_DIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory
            ${APK_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}
        COMMAND ${CMAKE_COMMAND} -E copy
            ${APK_TARGET_OUTPUT_FILENAME}
            ${APK_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}
        COMMAND ${QT5_INSTALL_PREFIX}/bin/androiddeployqt
            --verbose
            --output ${APK_DIR}
            --input ${CMAKE_CURRENT_BINARY_DIR}/${APK_TARGET}-config.json
            --deployment bundled
            --gradle
            ${ANDROIDDEPLOYQT_EXTRA_ARGS}
    )

    # Step 4: Create a custom target which pushes the created APK onto
    # the device.
    add_custom_target(
        ${APK_TARGET}-apk-install
        COMMAND ${ANDROID_SDK_ROOT}/platform-tools/adb install -r
            ${APK_DIR}/build/outputs/apk/${APK_FILENAME}
        DEPENDS
            ${APK_TARGET}-apk
    )

endfunction()
