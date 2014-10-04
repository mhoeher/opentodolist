#include "systemintegration/applicationinstance.h"

#include <QDebug>

namespace OpenTodoList {

namespace SystemIntegration {

/**
   @brief Constructor

   Creates a new instance of the class. The instance will bind to the @p applicationName and
   try to reserve it for itself. Otherwise, it prepares for communication with the already
   running instance of the application.
 */
ApplicationInstance::ApplicationInstance(const QString &applicationName, QObject *parent) :
    QObject(parent),
    m_applicationName( applicationName ),
    m_server( new QLocalServer(this) ),
    m_socket( new QLocalSocket() ),
    m_state( UnconnectedState )
{
    // Try to connect to the server to see if it is running
    m_socket->connectToServer( applicationName );
    if ( m_socket->waitForConnected( 1000 ) ) {
        qDebug() << "Established connection to already running application instance of"
                 << applicationName;
        m_state = InstanceIsSecondary;
    } else {
        // Try to start the server
        if ( m_server->listen( applicationName ) ) {
            qDebug() << "Started first instance of" << applicationName;
            m_state = InstanceIsPrimary;
        } else {
            // Duh! Could be we crashed the last time. Remove the previous
            // pipe and retry:
            QLocalServer::removeServer( applicationName );
            if ( m_server->listen( applicationName ) ) {
                qDebug() << "Started first instance of" << applicationName;
                m_state = InstanceIsPrimary;
            } else {
                // Give up...
                qWarning() << "Unable to ensure single application instance...";
            }
        }
    }

    if ( m_state == InstanceIsPrimary ) {
        connect( m_server, SIGNAL(newConnection()), this, SLOT(onClientConnected()) );
    }
}

/**
   @brief Returns the status of the application instance
   @sa Status
 */
ApplicationInstance::State ApplicationInstance::state() const
{
    return m_state;
}

/**
   @brief Sends a message to the primary application instance

   This sends the @p message to the primary application instance. This is intended
   to be used to to let the primary know a new client has been started and that
   it should e.g. bring up its GUI.

   @note This method has no effect if this is the primary instance!
 */
void ApplicationInstance::sendMessage(const QString &message)
{
    if ( m_socket->state() == QLocalSocket::ConnectedState ) {
        QString line = message;
        line = line.replace( "\\", "\\\\" );
        line = line.replace( "\n", "\\n" );
        m_socket->write( (line + "\n").toUtf8() );
        m_socket->flush();
        m_socket->waitForBytesWritten();
    }
}

// Called when a new client connects
void ApplicationInstance::onClientConnected()
{
    while ( m_server->hasPendingConnections() ) {
        // Get socket and connect to handler. Note: Socket is owned by m_server and hence
        // freed as soon as that one is closed...
        QLocalSocket *socket = m_server->nextPendingConnection();
        // ...or the connection is cut.
        connect( socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()) );

        // Connect to ready read signal
        connect( socket, SIGNAL(readyRead()), this, SLOT(onClientDataReceived()));

        qDebug() << "Client connection established";
    }
}

// Handle incoming data from clients
void ApplicationInstance::onClientDataReceived()
{
    QLocalSocket *socket = qobject_cast< QLocalSocket* >( sender() );
    if ( socket ) {
        while ( socket->canReadLine() ) {
            QString line = socket->readLine( MaxLineSize + 1 );
            line.chop(1);
            line.replace( "\\n", "\n" );
            line.replace( "\\\\", "\\" );
            qDebug() << "Received message from client:" << line;
            emit receivedMessage( line );
        }
    }
}

} /* SystemIntegration */

} /* OpenTodoList */
