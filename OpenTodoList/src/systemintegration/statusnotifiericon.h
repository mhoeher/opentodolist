#ifndef STATUSNOTIFIERICON_H
#define STATUSNOTIFIERICON_H

#include "commandhandler.h"

#include <QIcon>
#include <QObject>
#include <QString>

#ifdef HAS_KNOTIFICATIONS
#include <KStatusNotifierItem>
#define NEED_ACTIONS
#else
#ifdef HAS_QSYSTEM_TRAY_ICON
#include <QSystemTrayIcon>
#define NEED_ACTIONS
#endif
#endif

#ifdef QT_WIDGETS_LIB
#include <QApplication>
#include <QAction>
#include <QMenu>
#endif

namespace OpenTodoList {

namespace SystemIntegration {

class StatusNotifierIcon : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString applicationTitle READ applicationTitle WRITE setApplicationTitle NOTIFY applicationTitleChanged)
    Q_PROPERTY(QIcon applicationIcon READ applicationIcon WRITE setApplicationIcon NOTIFY applicationIconChanged)
public:
    explicit StatusNotifierIcon( CommandHandler *handler, QObject *parent = 0);
    virtual ~StatusNotifierIcon();

    QString applicationTitle() const;
    void setApplicationTitle( const QString &applicationTitle );

    QIcon applicationIcon() const;
    void setApplicationIcon( const QIcon &icon );


signals:

    void applicationTitleChanged();
    void applicationIconChanged();

public slots:

    void show();

private:

    CommandHandler *m_commandHandler;
    QString m_applicationTitle;
    QIcon m_applicationIcon;

#ifdef HAS_KNOTIFICATIONS
    KStatusNotifierItem *m_statusNotifier;
#elif HAS_QSYSTEM_TRAY_ICON
    QSystemTrayIcon *m_trayIcon;
#endif
#ifdef QT_WIDGETS_LIB
    QAction *m_toggleApplicationWindowAction;
    QAction *m_quitApplicationAction;
    QMenu *m_contextMenu;
#endif

private slots:

#ifdef QT_WIDGETS_LIB
    void applicationActivationRequested();
#endif

};

} /* SystemIntegration */

} /* OpenTodoList */

#endif // STATUSNOTIFIERICON_H
