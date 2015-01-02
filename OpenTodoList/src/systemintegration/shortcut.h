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

#ifndef SHORTCUT_H
#define SHORTCUT_H

#include <QKeySequence>
#include <QObject>
#include <QVariant>

namespace OpenTodoList {

namespace SystemIntegration {

/**
   @brief Provides keyboard shortcuts

   This class can be used in QML to bind to keyboard shortcuts.

   @note This is vastly inspired by
     http://kdeblog.mageprojects.com/2012/12/07/application-wide-shortcuts-in-qml-this-time-without-qshortcut/
 */
class Shortcut : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant keySequence READ keySequence WRITE setKeySequence NOTIFY keySequenceChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
public:
    explicit Shortcut(QObject *parent = 0);

    /**
       @brief The sequence this shortcut is bound to

       @sa setKeySequence
     */
    QVariant keySequence() const { return m_keySequence; }
    void setKeySequence( const QVariant &keySequence );

    /**
       @brief Returns whether the shortcut is enabled

       This returns true of the shortcut is enabled or false otherwise.
       If the shortcut is disabled, the triggered() event is not emitted and
       keyboard input is not intercepted.

       @sa setEnabled()
     */
    bool enabled() const { return m_enabled; }
    void setEnabled( bool enabled );

    Q_INVOKABLE QVariant fromStandardKey( QKeySequence::StandardKey standardKey );

signals:

    /**
       @brief Indicates that the key sequence has been changed
     */
    void keySequenceChanged();

    /**
       @brief Indicates that the enabled property has been changed
     */
    void enabledChanged();

    /**
       @brief The shortcut has been triggered
     */
    void triggered();

public:
    // QObject interface
    virtual bool eventFilter(QObject *, QEvent *);

private:

    QKeySequence m_keySequence;
    bool         m_enabled;

};

} /* SystemIntegration */

} /* OpenTodoList */

#endif // SHORTCUT_H
