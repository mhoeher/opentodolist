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

#include "systemintegration/statusnotifiericon.h"

#include <QIcon>
#include <QDebug>

namespace OpenTodoList {

namespace SystemIntegration {

StatusNotifierIcon::StatusNotifierIcon(CommandHandler *handler, QObject *parent) :
  QObject(parent),
  m_commandHandler( handler ),
  m_applicationTitle(),
  m_applicationIcon()

{
#ifdef QT_WIDGETS_LIB

#ifdef HAS_KNOTIFICATIONS
  m_statusNotifier = new KStatusNotifierItem(this);
  connect( m_statusNotifier, &KStatusNotifierItem::activateRequested,
           [this](bool,QPoint) {
    m_commandHandler->toggleWindow();
  });
#endif

#ifdef HAS_QSYSTEM_TRAY_ICON
  m_trayIcon = new QSystemTrayIcon(this);
  connect( m_trayIcon, &QSystemTrayIcon::activated, [this] (QSystemTrayIcon::ActivationReason) {
    this->m_commandHandler->toggleWindow();
  });
#endif

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

QString StatusNotifierIcon::applicationIcon() const
{
  return m_applicationIcon;
}

void StatusNotifierIcon::setApplicationIcon(const QString &icon)
{
  if ( m_applicationIcon != icon ) {
    m_applicationIcon = icon;
    qDebug() << m_applicationIcon;

#ifdef HAS_KNOTIFICATIONS
    m_statusNotifier->setIconByName( m_applicationIcon );
    m_statusNotifier->setToolTipIconByName( m_applicationIcon );
#elif HAS_QSYSTEM_TRAY_ICON
    QPixmap bmp( m_applicationIcon );
    QIcon icon;
    icon.addPixmap( bmp );
    m_trayIcon->setIcon( icon );
#endif
    emit applicationIconChanged();
  }
}

void StatusNotifierIcon::show()
{
#ifdef HAS_QSYSTEM_TRAY_ICON
  m_trayIcon->show();
#endif
}

} /* SystemIntegration */

} /* OpenTodoList */
