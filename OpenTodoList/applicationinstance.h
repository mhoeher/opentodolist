#ifndef APPLICATIONINSTANCE_H
#define APPLICATIONINSTANCE_H

#include <QObject>
#include <QLocalServer>
#include <QLocalSocket>


/**
   @brief A class used to ensure the application runs at most once

   This class is used to ensure that the application runs at most once. It can be used to first
   check whether the application is already running. If it is the case, it allows signalling
   to the running application instance (so that it e.g. can bring its main window to the
   front).
 */
class ApplicationInstance : public QObject
{
    Q_OBJECT
public:

    /**
       @brief Describes the various states the application can be in
     */
    enum State {
        InstanceIsPrimary, //!< This instance is the first instance
        InstanceIsSecondary, //!< This instance is a one started later and connected to a primary
        UnconnectedState //!< Failed to start local server and connect to an existing instance
    };

    /**
       @brief The maximum size of a single message.
     */
    static const int MaxLineSize = 10240;

    explicit ApplicationInstance(const QString &applicationName, QObject *parent = 0);

    State state() const;

signals:

    void receivedMessage( const QString &message );

public slots:

    void sendMessage( const QString &message = QString() );

private:

    QString       m_applicationName;
    QLocalServer *m_server;
    QLocalSocket *m_socket;
    State         m_state;

private slots:

    void onClientConnected();
    void onClientDataReceived();

};

#endif // APPLICATIONINSTANCE_H
