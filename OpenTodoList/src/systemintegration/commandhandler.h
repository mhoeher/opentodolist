/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014 - 2015  Martin Höher <martin@rpdev.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    void requestCreateWindow();

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
