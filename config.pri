# The application version
VERSION=0.1.0

# Pass in values via defines
DEFINES += VERSION=\\\"$$VERSION\\\"

OTHER_FILES += \
    ../templates/installer/packages/net.rpdev.OpenTodoList/meta/script.js \
    ../bin/linux-deploy-qt.pl
