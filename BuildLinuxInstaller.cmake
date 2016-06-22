set(REPOSITORY_URL ${INSTALLER_ONLINE_REPOSITORY_ROOT}/OpenTodoList-linux-x86_64)
set(PACKAGE_DIR ${INSTALLER_DIR}/packages/net.rpdev.OpenTodoList)
set(INSTALLER_CONFIG_DIR ${CMAKE_CURRENT_BINARY_DIR}/installer)

configure_file(templates/installer/config/config.xml.in
    ${CMAKE_CURRENT_BINARY_DIR}/installer/config.xml)
configure_file(
    templates/installer/packages/net.rpdev.OpenTodoList/meta/package.xml.in
    ${CMAKE_CURRENT_BINARY_DIR}/installer/package.xml
)

find_package(Qt5Core)

add_custom_target(LinuxInstaller)
add_custom_command(
    TARGET
        LinuxInstaller
    POST_BUILD
    COMMAND
        which patchelf
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
    COMMAND
        perl ${CMAKE_CURRENT_SOURCE_DIR}/bin/linux-deploy-qt.pl 
            --qt ${QT_INSTALL_PREFIX} --exe ${INSTALLER_DIR}/dist/bin/OpenTodoList
    COMMAND
        ${CMAKE_COMMAND} -E make_directory ${INSTALLER_DIR}/dist/share/OpenTodoList/icons
    COMMAND
        ${CMAKE_COMMAND} -E copy 
            ${CMAKE_CURRENT_SOURCE_DIR}/app/res/OpenTodoList80.png 
            ${INSTALLER_DIR}/dist/share/OpenTodoList/icons/
    COMMAND
        archivegen 
            ${PACKAGE_DIR}/data/OpenTodoList.7z 
            ${INSTALLER_DIR}/dist/bin 
            ${INSTALLER_DIR}/dist/lib 
            ${INSTALLER_DIR}/dist/plugins 
            ${INSTALLER_DIR}/dist/qml 
            ${INSTALLER_DIR}/dist/share 
            ${INSTALLER_DIR}/dist/translations
    COMMAND
        repogen -r -p ${INSTALLER_DIR}/packages ${INSTALLER_DIR}/OpenTodoList-linux-x86_64/
    COMMAND
        binarycreator -c ${INSTALLER_DIR}/config/config.xml 
            -p ${INSTALLER_DIR}/packages/ --online-only 
            ${INSTALLER_DIR}/OpenTodoList-online-linux-x86_64-${OPENTODOLIST_VERSION}
)
