#ifndef OPENTODOLISTQMLEXTENSIONSPLUGIN_H
#define OPENTODOLISTQMLEXTENSIONSPLUGIN_H

#include <QObject>
#include <QQmlExtensionPlugin>

// Forward declaration:
class QQmlEngine;
class QJSEngine;

class OpenTodoListQmlExtensionsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
public:
    explicit OpenTodoListQmlExtensionsPlugin(QObject *parent = 0);

    // QQmlTypesExtensionInterface interface
    void registerTypes(const char *uri) override;

private:
    static QObject *createApplication(QQmlEngine *engine, QJSEngine *jsEngine);
};

#endif // OPENTODOLISTQMLEXTENSIONSPLUGIN_H
