#ifndef OPENTODOLISTWIDGETSQMLPLUGIN_H
#define OPENTODOLISTWIDGETSQMLPLUGIN_H

#include <QQmlExtensionPlugin>

class OpenTodoListWidgetsQmlPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
public:
    explicit OpenTodoListWidgetsQmlPlugin(QObject *parent = 0);

    virtual void registerTypes(const char *uri);

signals:

public slots:

};

#endif // OPENTODOLISTWIDGETSQMLPLUGIN_H
