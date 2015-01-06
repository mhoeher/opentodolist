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

#ifndef APPLICATION_H
#define APPLICATION_H

#ifdef HAS_QT_WIDGETS
#include <QApplication>
#else
#include <QGuiApplication>
#endif

#include "database/database.h"

#include "systemintegration/applicationinstance.h"
#include "systemintegration/commandhandler.h"
#include "systemintegration/statusnotifiericon.h"

#include "qtquick2applicationviewer.h"

namespace OpenTodoList {
namespace SystemIntegration {

#ifdef HAS_QT_WIDGETS
typedef QApplication ApplicationBase;
#else
typedef QGuiApplication ApplicationBase;
#endif

class Application : public ApplicationBase
{
    Q_OBJECT
    Q_PROPERTY(OpenTodoList::SystemIntegration::ApplicationInstance* instance READ instance CONSTANT)
    Q_PROPERTY(OpenTodoList::SystemIntegration::CommandHandler* handler READ handler CONSTANT)
    Q_PROPERTY(QtQuick2ApplicationViewer* viewer READ viewer NOTIFY viewerChanged )
    Q_PROPERTY(OpenTodoList::SystemIntegration::StatusNotifierIcon* notifierIcon READ notifierIcon CONSTANT)
    Q_PROPERTY(OpenTodoList::DataBase::Database* database READ database CONSTANT)

public:
    explicit Application(int &argc, char *argv[]);
    virtual ~Application();

  void prepare();

    ApplicationInstance* instance() const;
    CommandHandler* handler() const;
    QtQuick2ApplicationViewer* viewer() const;
    StatusNotifierIcon* notifierIcon() const;
    DataBase::Database* database() const;

    void showWindow();
    void hideWindow();

signals:

    void viewerChanged();

public slots:

private:

    ApplicationInstance         *m_instance;
    CommandHandler              *m_handler;
    QtQuick2ApplicationViewer   *m_viewer;
    StatusNotifierIcon          *m_notifier;
    QString                      m_basePath;

    DataBase::Database          *m_database;

    bool                         m_pluginsRegistered;

    void registerPlugins();
    void setupPaths(QtQuick2ApplicationViewer *viewer = nullptr);
    void showNotifierIcon();

private slots:

    void showWindowImplementation();
    void hideWindowImplementation();

};

} // SystemIntegration
} // OpenTodoList

#endif // APPLICATION_H
