/*
 * Copyright 2020 Martin Hoeher <martin@rpdev.net>
 +
 * This file is part of OpenTodoList.
 *
 * OpenTodoList is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * OpenTodoList is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenTodoList.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OPENTODOLISTQMLEXTENSIONSPLUGIN_H_
#define OPENTODOLISTQMLEXTENSIONSPLUGIN_H_

#include <QObject>
#include <QQmlExtensionPlugin>

// Forward declaration:
class QQmlEngine;
class QJSEngine;
class Application;

class OpenTodoListQmlExtensionsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
public:
    explicit OpenTodoListQmlExtensionsPlugin(QObject* parent = 0);

    // QQmlTypesExtensionInterface interface
    void registerTypes(const char* uri) override;

    Application* application() const;
    void setApplication(Application* application);

private:
    Application* m_application;
};

#endif // OPENTODOLISTQMLEXTENSIONSPLUGIN_H_
