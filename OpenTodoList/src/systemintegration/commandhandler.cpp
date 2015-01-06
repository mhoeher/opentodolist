/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014 - 2015 Martin Höher <martin@rpdev.net>
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

#include "systemintegration/commandhandler.h"

#include <QCoreApplication>

namespace OpenTodoList {

namespace SystemIntegration {


// Static strings for the various commands:
const QString CommandHandler::ShowWindowCommand = "window.show";
const QString CommandHandler::HideWindowCommand = "window.hide";
const QString CommandHandler::ToggleWindowCommand = "window.toggle";
const QString CommandHandler::TerminateApplicationCommand = "application.terminate";

/**
   @brief Constructor
 */
CommandHandler::CommandHandler(QObject *parent) :
    QObject(parent),
    m_applicationWindow( 0 )
{
}

/**
   @brief Generates the command that indicates to the primary instance to show its window
 */
QString CommandHandler::show()
{
    return ShowWindowCommand;
}

/**
   @brief Generates the command that indicates to the primary instance to hide its window
 */
QString CommandHandler::hide()
{
    return HideWindowCommand;
}

/**
   @brief Generates the command for toggling the visibility of the application's window
 */
QString CommandHandler::toggle()
{
    return ToggleWindowCommand;
}

/**
   @brief Generates the command for terminating the application
 */
QString CommandHandler::terminate()
{
    return TerminateApplicationCommand;
}

/**
   @brief Handles incoming messages

   This method handles each incoming @p message to the primary application instance. It is supposed
   to receive input from ApplicationInstance::receivedMessage(). It will scan the received
   command for known input commands and process them as far as possible.

   If custom commands are received, the customCommandReceived() signal is used.
 */
void CommandHandler::handleMessage(const QString &message)
{
    QStringList list = message.split( "\n" );
    if ( list.count() > 0 ) {
        QString commandName = list.at( 0 );
        if ( commandName == ShowWindowCommand ) {
            showWindow();
        } else if (commandName == HideWindowCommand ) {
            hideWindow();
        } else if ( commandName == ToggleWindowCommand ) {
            toggleWindow();
        } else if ( commandName == TerminateApplicationCommand ) {
            terminate();
        } else {
            emit customCommandReceived( message );
        }
    }
}

/**
   @brief Shows the application's window and brings it to the front
 */
void CommandHandler::showWindow()
{
    if ( m_applicationWindow ) {
        m_applicationWindow->hide();
        m_applicationWindow->show();
        m_applicationWindow->raise();
    } else {
      emit requestCreateWindow();
    }
}

/**
   @brief Hides the application's window
 */
void CommandHandler::hideWindow()
{
    if ( m_applicationWindow ) {
        m_applicationWindow->hide();
    }
}

/**
   @brief Toggles the visibility of the application's window
 */
void CommandHandler::toggleWindow()
{
    if ( m_applicationWindow ) {
        if ( m_applicationWindow->visibility() == QWindow::Hidden ||
             m_applicationWindow->visibility() == QWindow::Minimized ) {
            showWindow();
        } else {
            hideWindow();
        }
    } else {
      showWindow();
    }
}

void CommandHandler::terminateApplication()
{
    QCoreApplication::quit();
}

/**
   @brief The application's main window.
   @sa setApplicationWindow()
 */
QWindow *CommandHandler::applicationWindow() const
{
    return m_applicationWindow;
}

/**
   @brief Sets the application's main window
   @sa applicationWindow()
 */
void CommandHandler::setApplicationWindow(QWindow *applicationWindow)
{
    m_applicationWindow = applicationWindow;
}

} /* SystemIntegration */

} /* OpenTodoList */
