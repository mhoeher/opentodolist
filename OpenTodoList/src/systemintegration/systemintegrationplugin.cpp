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

#include "systemintegration/systemintegrationplugin.h"

#include "application.h"
#include "shortcut.h"

#include <qqml.h>
#include <QJSEngine>

namespace OpenTodoList {

/**
 * @brief Classes integrating the application into the host system
 *
 * This namespace contains various classes that are used to integrate the application into
 * the host operating system and desktop environment.
 */
namespace SystemIntegration {

Plugin::Plugin(QObject *parent) :
    QQmlExtensionPlugin(parent)
{
}

void Plugin::registerTypes(const char *uri)
{
    //@uri net.rpdev.OpenTodoList.SystemIntegration
    qmlRegisterType< Shortcut >( uri, 1, 0,  "Shortcut" );
    qmlRegisterUncreatableType< CommandHandler >( uri, 1, 0, "CommandHandler", "Use Application.handler instead!" );
    qmlRegisterUncreatableType< ApplicationInstance >( uri, 1, 0, "ApplicationInstance", "Use Application.instance instead!" );
    qmlRegisterUncreatableType< StatusNotifierIcon >( uri, 1, 0, "StatusNotifierIcon", "Use Application.notifierIcon instead!" );
    qmlRegisterUncreatableType< Application >( uri, 1, 0, "Application", "Use application global variable instead!" );
}

} /* SystemIntegration */

} /* OpenTodoList */
