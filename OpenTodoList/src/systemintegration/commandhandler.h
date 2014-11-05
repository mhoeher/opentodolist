#ifndef PRIMARYINSTANCECOMMANDHANDLER_H
#define PRIMARYINSTANCECOMMANDHANDLER_H

#include <QObject>
#include "QWindow"

namespace OpenTodoList {

namespace SystemIntegration {

/**
   @brief Helper class used to handle incoming messages in primary application instance
 */
class CommandHandler : public QObject
{
    Q_OBJECT
public:
    explicit CommandHandler(QObject *parent = 0);

    static QString show();
    static QString hide();
    static QString toggle();
    static QString terminate();

    QWindow *applicationWindow() const;
    void setApplicationWindow(QWindow *applicationWindow);

signals:

    void customCommandReceived( const QString command );

public slots:

    void handleMessage( const QString &message );

    void showWindow();
    void hideWindow();
    void toggleWindow();
    void terminateApplication();

private:

    static const QString ShowWindowCommand;
    static const QString HideWindowCommand;
    static const QString ToggleWindowCommand;
    static const QString TerminateApplicationCommand;

    QWindow *m_applicationWindow;
};

} /* SystemIntegration */

} /* OpenTodoList */

#endif // PRIMARYINSTANCECOMMANDHANDLER_H
