#ifndef BACKENDRUNNER_H
#define BACKENDRUNNER_H

#include "backendwrapper.h"

#include <QDir>
#include <QObject>
#include <QStandardPaths>
#include <QThread>
#include <QVector>

// forward declarations:
class TodoListLibrary;
class BackendWrapper;

class BackendRunner : public QObject
{
    Q_OBJECT
public:
    explicit BackendRunner(TodoListLibrary *library, QObject *parent = 0);
    virtual ~BackendRunner();

signals:

    void startBackends();
    void stopBackends();

public slots:

    void start();
    void stop();

private:

    QThread m_thread;
    QVector< BackendWrapper* > m_backends;

#ifdef Q_OS_ANDROID
    static QString androidExtStorageLocation();
#endif
    static QString localStorageLocation( const QString &type );

};

#endif // BACKENDRUNNER_H
