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
    Q_PROPERTY(QtQuick2ApplicationViewer* viewer READ viewer CONSTANT)
    Q_PROPERTY(OpenTodoList::SystemIntegration::StatusNotifierIcon* notifierIcon READ notifierIcon CONSTANT)
    Q_PROPERTY(OpenTodoList::DataBase::Database* database READ database CONSTANT)

public:
    explicit Application(int &argc, char *argv[]);
    virtual ~Application();

    ApplicationInstance* instance() const;
    CommandHandler* handler() const;
    QtQuick2ApplicationViewer* viewer() const;
    StatusNotifierIcon* notifierIcon() const;
    DataBase::Database* database() const;

    void showWindow();

signals:

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
    void setupPaths();
    void showNotifierIcon();

};

} // SystemIntegration
} // OpenTodoList

#endif // APPLICATION_H
