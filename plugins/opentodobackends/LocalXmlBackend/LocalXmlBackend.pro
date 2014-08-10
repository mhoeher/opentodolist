include(../../../config.pri)
setupPlugin(LocalXmlBackend,opentodobackends)

qtcAddDeployment()

QT += xml

SOURCES += \
    localxmlbackend.cpp

HEADERS += \
    localxmlbackend.h

OTHER_FILES += \
    LocalXmlBackend.json
