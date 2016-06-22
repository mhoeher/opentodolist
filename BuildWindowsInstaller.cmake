if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    set(ARCH x86_64)
else()
    set(ARCH i386)
endif()

set(REPOSITORY_URL ${INSTALLER_ONLINE_REPOSITORY_ROOT}/OpenTodoList-windows-${ARCH})
set(PACKAGE_DIR ${INSTALLER_DIR}/packages/net.rpdev.OpenTodoList)
set(INSTALLER_CONFIG_DIR ${CMAKE_CURRENT_BINARY_DIR}/installer)

# Needs to be relative for Wine to work:
file(RELATIVE_PATH INSTALLER_CONTROLLER_SCRIPT
    ${INSTALLER_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/templates/installer/config/controller.js
)

configure_file(templates/installer/config/config.xml.in
    ${CMAKE_CURRENT_BINARY_DIR}/installer/config.xml)
configure_file(
    templates/installer/packages/net.rpdev.OpenTodoList/meta/package.xml.in
    ${CMAKE_CURRENT_BINARY_DIR}/installer/package.xml
)

find_package(Qt5Core)

set(DIST_FILES
    ../bin/libgcc_s_seh-1.dll
    ../bin/libstdc++-6.dll
    ../bin/zlib1.dll
    ../bin/libpcre16-0.dll
    ../bin/libpcre-1.dll
    ../bin/libharfbuzz-0.dll
    ../bin/libpng16-16.dll
    ../bin/libglib-2.0-0.dll
    ../bin/libfreetype-6.dll
    ../bin/libintl-8.dll
    ../bin/libeay32.dll
    ../bin/ssleay32.dll
    ../bin/libiconv-2.dll
    ../bin/libbz2.dll
    ../bin/libjasper-1.dll
    ../bin/libjpeg-9.dll
    ../bin/libmng-2.dll
    ../bin/libtiff-5.dll
    ../bin/libwebp-5.dll
    ../bin/liblzma-5.dll
    ../bin/liblcms2-2.dll
    bin/Qt5Core.dll
    bin/Qt5Gui.dll
    bin/Qt5Network.dll
    bin/Qt5Svg.dll
    bin/Qt5Qml.dll
    bin/Qt5Quick.dll
    bin/Qt5Widgets.dll
    bin/Qt5Xml.dll
)

set(DIST_DIRS
    translations
    plugins/bearer:bearer
    plugins/iconengines:iconengines
    plugins/imageformats:imageformats
    plugins/platforms:platforms
    plugins/qmltooling:qmltooling
    qml/Qt:Qt
    qml/QtGraphicalEffects:QtGraphicalEffects
    qml/QtQuick:QtQuick
    qml/QtQuick.2:QtQuick.2
    qml/QtQml:QtQml
)


add_custom_target(WindowsInstaller)
add_custom_command(
    TARGET
        WindowsInstaller
    POST_BUILD
    COMMAND
        ${CMAKE_COMMAND} -E remove_directory ${INSTALLER_DIR}
    COMMAND
        ${CMAKE_COMMAND} -E make_directory ${PACKAGE_DIR}/meta
    COMMAND
        ${CMAKE_COMMAND} -E copy 
            ${CMAKE_CURRENT_BINARY_DIR}/installer/package.xml ${PACKAGE_DIR}/meta
    COMMAND
        ${CMAKE_COMMAND} -E copy 
            ${CMAKE_CURRENT_SOURCE_DIR}/templates/installer/packages/net.rpdev.OpenTodoList/meta/script.js
            ${PACKAGE_DIR}/meta
    COMMAND
        ${CMAKE_COMMAND} -E make_directory ${INSTALLER_DIR}/config
    COMMAND
        ${CMAKE_COMMAND} -E copy 
            ${CMAKE_CURRENT_SOURCE_DIR}/app/res/OpenTodoList80.png ${INSTALLER_DIR}/config
    COMMAND
        ${CMAKE_COMMAND} -E copy 
            ${CMAKE_CURRENT_BINARY_DIR}/installer/config.xml ${INSTALLER_DIR}/config
    COMMAND
        ${CMAKE_COMMAND} -E make_directory 
            ${PACKAGE_DIR}/data
    COMMAND
        ${CMAKE_COMMAND} -E make_directory ${INSTALLER_DIR}/dist/bin
    COMMAND
        ${CMAKE_COMMAND} -E copy $<TARGET_FILE:OpenTodoList> ${INSTALLER_DIR}/dist/bin
)

foreach(file ${DIST_FILES})
    add_custom_command(
        TARGET
            WindowsInstaller
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy ${QT_INSTALL_PREFIX}/${file} ${INSTALLER_DIR}/dist/bin
    )
endforeach()

foreach(dir ${DIST_DIRS})
    string(REPLACE ":" ";" dir ${dir})
    list(GET dir 0 from)
    list(GET dir -1 to)
    add_custom_command(
        TARGET
            WindowsInstaller
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_directory ${QT_INSTALL_PREFIX}/${from} ${INSTALLER_DIR}/dist/bin/${to}
    )
endforeach()

# Commands require relative paths to work from within Wine:
add_custom_command(
    TARGET
        WindowsInstaller
    WORKING_DIRECTORY
        ${INSTALLER_DIR}
    COMMAND
        ${CMAKE_COMMAND} -E make_directory ${INSTALLER_DIR}/dist/share/OpenTodoList/icons
    COMMAND
        ${CMAKE_COMMAND} -E copy 
            ${CMAKE_CURRENT_SOURCE_DIR}/app/res/OpenTodoList.ico 
            ${INSTALLER_DIR}/dist/share/OpenTodoList/icons/
)

add_custom_command(
    TARGET
        WindowsInstaller
    WORKING_DIRECTORY
        ${INSTALLER_DIR}/dist
    COMMAND
        archivegen.exe 
            ../packages/net.rpdev.OpenTodoList/data/OpenTodoList.7z 
            ./bin 
            ./share 
)

add_custom_command(
    TARGET
        WindowsInstaller
    WORKING_DIRECTORY
        ${INSTALLER_DIR}
    COMMAND
        repogen.exe -r -p ./packages ./OpenTodoList-windows-${ARCH}/
    COMMAND
        binarycreator.exe -c ./config/config.xml 
            -p ./packages/ --online-only 
            ./OpenTodoList-online-windows-${ARCH}-${OPENTODOLIST_VERSION}
)
