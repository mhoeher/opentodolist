set(OTLDQ_SRCS 
    applicationviewer.cpp
    main.cpp
    qtquick2applicationviewer/qtquick2applicationviewer.cpp
)

set(OTLDQ_HDRS
    applicationviewer.h
    qtquick2applicationviewer/qtquick2applicationviewer.h
)

set(OTLDQ_FILES
    qml/OpenTodoListDesktopQml/main.qml
    OpenTodoListDesktopQml_harmattan.desktop.in
    OpenTodoListDesktopQml.desktop
    OpenTodoListDesktopQml80.png
    OpenTodoListDesktopQml64.png
)

set(OTL_LAUNCH_APP ${CMAKE_INSTALL_PREFIX}/bin/OpenTodoListDesktop )
add_executable(OpenTodoListDesktop ${OTLDQ_SRCS})

include_directories(qtquick2applicationviewer)
include_directories(../OpenTodoListCore)

qt5_use_modules( OpenTodoListDesktop Core)
qt5_use_modules( OpenTodoListDesktop Quick)

target_link_libraries(OpenTodoListDesktop OpenTodoListCore)

CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/OpenTodoListDesktopQml.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/OpenTodoListDesktopQml.desktop )

install( TARGETS OpenTodoListDesktop DESTINATION bin )
install( DIRECTORY qml DESTINATION share/OpenTodoList )
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/OpenTodoListDesktopQml.desktop DESTINATION share/applications )