TARGET = OpenTodoListDesktopQml
TEMPLATE = app

DESTDIR = ../bin

INCLUDEPATH += ../OpenTodoListCore

LIBS += -L$$DESTDIR -lOpenTodoListCore

QT += quick

HEADERS += \
    applicationviewer.h \
    imageprovider.h

SOURCES += \
    main.cpp \
    applicationviewer.cpp \
    imageprovider.cpp

OTHER_FILES += \
    qml/OpenTodoListDesktopQml/action_delete.png \
    qml/OpenTodoListDesktopQml/action_delete.svg \
    qml/OpenTodoListDesktopQml/action_restore.png \
    qml/OpenTodoListDesktopQml/action_restore.svg \
    qml/OpenTodoListDesktopQml/view.svg \
    qml/OpenTodoListDesktopQml/view.sci \
    qml/OpenTodoListDesktopQml/view.png \
    qml/OpenTodoListDesktopQml/Utils.js \
    qml/OpenTodoListDesktopQml/toolbutton_4.svg \
    qml/OpenTodoListDesktopQml/toolbutton_4.png \
    qml/OpenTodoListDesktopQml/toolbutton_3.svg \
    qml/OpenTodoListDesktopQml/toolbutton_3.png \
    qml/OpenTodoListDesktopQml/toolbutton_2.svg \
    qml/OpenTodoListDesktopQml/toolbutton_2.png \
    qml/OpenTodoListDesktopQml/toolbutton_1.svg \
    qml/OpenTodoListDesktopQml/toolbutton_1.png \
    qml/OpenTodoListDesktopQml/description.svg \
    qml/OpenTodoListDesktopQml/description.sci \
    qml/OpenTodoListDesktopQml/description.png \
    qml/OpenTodoListDesktopQml/View.qml \
    qml/OpenTodoListDesktopQml/ToolButton.qml \
    qml/OpenTodoListDesktopQml/TodoListView.qml \
    qml/OpenTodoListDesktopQml/TodoListEntry.qml \
    qml/OpenTodoListDesktopQml/TodoDetailsView.qml \
    qml/OpenTodoListDesktopQml/SimpleTextInput.qml \
    qml/OpenTodoListDesktopQml/SearchView.qml \
    qml/OpenTodoListDesktopQml/ProgressBar.qml \
    qml/OpenTodoListDesktopQml/NewTodoListView.qml \
    qml/OpenTodoListDesktopQml/main.qml \
    qml/OpenTodoListDesktopQml/Button.qml \
    qml/OpenTodoListDesktopQml/IconButton.qml \
    qml/OpenTodoListDesktopQml/ViewContainer.qml \
    qml/OpenTodoListDesktopQml/DeletedTodosView.qml \
    qml/OpenTodoListDesktopQml/DatePicker.qml \
    qml/OpenTodoListDesktopQml/Calendar.qml \
    qml/OpenTodoListDesktopQml/fontawesome-webfont.license.txt \
    qml/OpenTodoListDesktopQml/fontawesome-webfont.ttf \
    qml/OpenTodoListDesktopQml/SymbolButton.qml \
    qml/OpenTodoListDesktopQml/ColorScheme.qml \
    qml/OpenTodoListDesktopQml/rpdevlogo_webheader.png \
    qml/OpenTodoListDesktopQml/FontLayout.qml \
    qml/OpenTodoListDesktopQml/ProgressIndicator.qml \
    qml/OpenTodoListDesktopQml/PriorityIndicator.qml \
    qml/OpenTodoListDesktopQml/TodoView.qml \
    qml/OpenTodoListDesktopQml/Popup.qml

# Add more folders to ship with the application, here
qml.source = qml/OpenTodoListDesktopQml
qml.target = qml
DEPLOYMENTFOLDERS = qml

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

include(../utils.pri)
copyToDestDir($$OTHER_FILES,$$PWD)

# Support "make install":
target.path = $$INSTALL_PREFIX/bin
OpenTodoListDesktopQmlData.files = $$OTHER_FILES
OpenTodoListDesktopQmlData.path = $$INSTALL_PREFIX/share/OpenTodoList
INSTALLS += target OpenTodoListDesktopQmlData
