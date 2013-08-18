TARGET = OpenTodoList
TEMPLATE = app

INCLUDEPATH += ../OpenTodoListCore

# Note: Plugins need to come BEFORE the core libraries in order for the
#       linker to find all symbols correctly.
android:LIBS += -L../plugins/opentodobackends/LocalXmlBackend -lLocalXmlBackend
android:QT += xml

LIBS += -L../OpenTodoListCore -lOpenTodoListCore


QT += quick

HEADERS += \
    applicationviewer.h \
    imageprovider.h

SOURCES += \
    main.cpp \
    applicationviewer.cpp \
    imageprovider.cpp

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    qml/OpenTodoList/action_delete.png \
    qml/OpenTodoList/action_delete.svg \
    qml/OpenTodoList/action_restore.png \
    qml/OpenTodoList/action_restore.svg \
    qml/OpenTodoList/view.svg \
    qml/OpenTodoList/view.sci \
    qml/OpenTodoList/view.png \
    qml/OpenTodoList/Utils.js \
    qml/OpenTodoList/toolbutton_4.svg \
    qml/OpenTodoList/toolbutton_4.png \
    qml/OpenTodoList/toolbutton_3.svg \
    qml/OpenTodoList/toolbutton_3.png \
    qml/OpenTodoList/toolbutton_2.svg \
    qml/OpenTodoList/toolbutton_2.png \
    qml/OpenTodoList/toolbutton_1.svg \
    qml/OpenTodoList/toolbutton_1.png \
    qml/OpenTodoList/description.svg \
    qml/OpenTodoList/description.sci \
    qml/OpenTodoList/description.png \
    qml/OpenTodoList/View.qml \
    qml/OpenTodoList/ToolButton.qml \
    qml/OpenTodoList/TodoListView.qml \
    qml/OpenTodoList/TodoListEntry.qml \
    qml/OpenTodoList/TodoDetailsView.qml \
    qml/OpenTodoList/SimpleTextInput.qml \
    qml/OpenTodoList/SearchView.qml \
    qml/OpenTodoList/ProgressBar.qml \
    qml/OpenTodoList/NewTodoListView.qml \
    qml/OpenTodoList/main.qml \
    qml/OpenTodoList/Button.qml \
    qml/OpenTodoList/IconButton.qml \
    qml/OpenTodoList/ViewContainer.qml \
    qml/OpenTodoList/DeletedTodosView.qml \
    qml/OpenTodoList/DatePicker.qml \
    qml/OpenTodoList/Calendar.qml \
    qml/OpenTodoList/fontawesome-webfont.license.txt \
    qml/OpenTodoList/fontawesome-webfont.ttf \
    qml/OpenTodoList/SymbolButton.qml \
    qml/OpenTodoList/ColorScheme.qml \
    qml/OpenTodoList/rpdevlogo_webheader.png \
    qml/OpenTodoList/FontLayout.qml \
    qml/OpenTodoList/ProgressIndicator.qml \
    qml/OpenTodoList/PriorityIndicator.qml \
    qml/OpenTodoList/TodoView.qml \
    qml/OpenTodoList/Popup.qml

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
#qtcAddDeployment()

#include(../utils.pri)
#copyToDestDir($$OTHER_FILES,$$PWD)

# Support "make install":
#target.path = $$INSTALL_PREFIX/bin
#OpenTodoListData.files = $$OTHER_FILES
#OpenTodoListData.path = $$INSTALL_PREFIX/share/OpenTodoList
#INSTALLS += target OpenTodoListData

