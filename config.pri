# The application version
OPENTODOLIST_VERSION=0.1.0

# Pass in values via defines
DEFINES += VERSION=\\\"$$OPENTODOLIST_VERSION\\\"

include(OpenTodoList/qtquick2applicationviewer/qtquick2applicationviewer.pri)

# Function to setup applications
defineTest(setupApplication) {

# Location where to store assets (Depending on target platform):
android { # Android: Automatically put into assets/ directry; leave blank
    assets_prefix =
} else { # All other: Save relative to application binary
    assets_prefix = ../share/
}

# Setup destination path
!macx {
DESTDIR = $$OUT_PWD/../bin
} else {
DESTDIR = $$OUT_PWD
}

# Setup path (relative to install root) where to install files via make install
target.path = /bin

export(DESTDIR)
export(target.path)
export(assets_prefix)
}

# Function to setup plugins
# Arg 1: Name of the plugin
# Arg 2: Type of the plugin (i.e. sub dir name below plugins/ directory)
defineTest(setupPlugin) {
name = $$1
type = $$2

# Location where to store assets (Depending on target platform):
android { # Android: Automatically put into assets/ directory; leave blank
    assets_prefix =
} else { # All other: Save relative to application binary
    assets_prefix = ../../../share/
}

android:!equals(type,"opentodolist") {
    TARGET = $$qtLibraryTarget(net_rpdev_opentodolist_plugins_$${type}_$$name)
} else {
    TARGET = $$qtLibraryTarget($$name)
}
TEMPLATE = lib
CONFIG += plugin
macx:CONFIG -= lib_bundle
INCLUDEPATH += ../../opentodolist/OpenTodoListCore

# Set destination path
macx {
DESTDIR = $$OUT_PWD/../OpenTodoList/OpenTodoList.app/Contents/Plugins
} else {
DESTDIR = $$OUT_PWD/../
}

# Path to where to "make install" relative to install root
target.path = /plugins/$$type
target.installBase = /plugins/$$type/$$name

export(TARGET)
export(TEMPLATE)
export(CONFIG)
export(INCLUDEPATH)
export(DESTDIR)
export(target.path)
export(target.installBase)
export(assets_prefix)

}
