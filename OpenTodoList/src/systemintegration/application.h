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

#include <QQmlApplicationEngine>
#include <QFileSystemWatcher>

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
    Q_PROPERTY(QQmlApplicationEngine* viewer READ viewer NOTIFY viewerChanged )
    Q_PROPERTY(OpenTodoList::SystemIntegration::StatusNotifierIcon* notifierIcon READ notifierIcon CONSTANT)
    Q_PROPERTY(OpenTodoList::DataBase::Database* database READ database CONSTANT)

public:
    explicit Application(int &argc, char *argv[]);
    virtual ~Application();

  void prepare();

    ApplicationInstance* instance() const;
    CommandHandler* handler() const;
    QQmlApplicationEngine* viewer() const;
    StatusNotifierIcon* notifierIcon() const;
    DataBase::Database* database() const;

    void showWindow();
    Q_INVOKABLE void hideWindow();

    QString mainQmlFile() const;
    void setMainQmlFile(const QString &mainQmlFile);

    bool reloadQmlOnChange() const;
    void setReloadQmlOnChange(bool reloadQmlOnChange);

signals:

    void viewerChanged();

public slots:

private:

    ApplicationInstance         *m_instance;
    CommandHandler              *m_handler;
    QQmlApplicationEngine       *m_viewer;
    StatusNotifierIcon          *m_notifier;
    QString                      m_basePath;

    DataBase::Database          *m_database;

    bool                         m_pluginsRegistered;

    QString                      m_mainQmlFile;
    bool                         m_reloadQmlOnChange;

    QFileSystemWatcher          *m_fileSystemWatcher;

    void registerPlugins();
    void setupPaths(QQmlApplicationEngine *viewer = nullptr);
    void showNotifierIcon();
    void watchQmlFiles();
    void unwatchQmlFiles();

private slots:

    void showWindowImplementation();
    void hideWindowImplementation();
    void reloadQml();

};

} // SystemIntegration
} // OpenTodoList

#endif // APPLICATION_H
