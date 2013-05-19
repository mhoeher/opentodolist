set(OTLDQ_SRCS
    main.cpp
    qtquick2applicationviewer/qtquick2applicationviewer.cpp
)

set(OTLDQ_HDRS
    qtquick2applicationviewer/qtquick2applicationviewer.h
)

set(QTLDQ_FILES
    qml/OpenTodoListDesktopQml/main.qml
    OpenTodoListDesktopQml_harmattan.desktop
    OpenTodoListDesktopQml.desktop
    OpenTodoListDesktopQml80.png
    OpenTodoListDesktopQml64.png
)

add_executable(OpenTodoListDesktop ${OTLDQ_SRCS})

include_directories(qtquick2applicationviewer)

qt5_use_modules( OpenTodoListDesktop Core)
qt5_use_modules( OpenTodoListDesktop Quick)
