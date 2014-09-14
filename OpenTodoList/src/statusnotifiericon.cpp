#include "statusnotifiericon.h"

#include <QDebug>

StatusNotifierIcon::StatusNotifierIcon(CommandHandler *handler, QObject *parent) :
    QObject(parent)
{
    m_commandHandler = handler;

#ifdef QT_WIDGETS_LIB
#ifdef HAS_KNOTIFICATIONS
    m_statusNotifier = new KStatusNotifierItem( this );
    m_contextMenu = m_statusNotifier->contextMenu();
#else
    m_contextMenu = new QMenu();
#endif
    m_quitApplicationAction = m_contextMenu->addAction( tr( "Quit" ) );
    connect( m_quitApplicationAction, SIGNAL(triggered()), qApp, SLOT(quit()) );
#endif

#ifdef HAS_KNOTIFICATIONS
    m_statusNotifier->setCategory( KStatusNotifierItem::ApplicationStatus );
    m_statusNotifier->setContextMenu( m_contextMenu );
    connect( m_statusNotifier, SIGNAL(activateRequested(bool,QPoint)),
             this, SLOT(applicationActivationRequested()) );
#elif HAS_QSYSTEM_TRAY_ICON
    m_trayIcon = new QSystemTrayIcon( this );
    if ( !QSystemTrayIcon::isSystemTrayAvailable() ) {
        qWarning() << "Using System Tray Icon API but desktop does not have system tray!";
    }
#endif
}

StatusNotifierIcon::~StatusNotifierIcon()
{
}

QString StatusNotifierIcon::applicationTitle() const
{
    return m_applicationTitle;
}

void StatusNotifierIcon::setApplicationTitle(const QString &applicationTitle)
{
    if ( m_applicationTitle != applicationTitle ) {
        m_applicationTitle = applicationTitle;
#ifdef HAS_KNOTIFICATIONS
        m_statusNotifier->setTitle( applicationTitle );
        m_statusNotifier->setToolTipTitle( applicationTitle );
#elif HAS_QSYSTEM_TRAY_ICON
        m_trayIcon->setToolTip( applicationTitle );
#endif
        emit applicationTitleChanged();
    }
}

QIcon StatusNotifierIcon::applicationIcon() const
{
    return m_applicationIcon;
}

void StatusNotifierIcon::setApplicationIcon(const QIcon &icon)
{
    m_applicationIcon = icon;
#ifdef HAS_KNOTIFICATIONS
    m_statusNotifier->setIconByPixmap( icon );
#elif HAS_QSYSTEM_TRAY_ICON
    m_trayIcon->setIcon( icon );
#endif
    emit applicationIconChanged();
}

void StatusNotifierIcon::show()
{
#ifdef HAS_QSYSTEM_TRAY_ICON
    m_trayIcon->show();
#endif
}

void StatusNotifierIcon::applicationActivationRequested()
{
    m_commandHandler->toggleWindow();
}
