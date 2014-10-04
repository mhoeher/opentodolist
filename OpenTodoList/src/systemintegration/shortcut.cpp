#include "systemintegration/shortcut.h"

#include <QCoreApplication>
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>

namespace OpenTodoList {

namespace SystemIntegration {

Shortcut::Shortcut(QObject *parent) :
    QObject(parent),
    m_keySequence( QKeySequence() ),
    m_enabled( true )
{
    if ( qApp ) {
        qApp->installEventFilter( this );
    } else {
        qWarning() << "Constructing a Shortcut without a QCoreApplication instantiated! This is "
                      "not supported!";
    }
}

/**
   @brief Sets the key sequence the shortcut is bound to

   This allows to bind the shortcut to a key sequence. The @p keySequence can be set via different
   ways. One way is to use QKeySequence::StandardKey:

   @code
   Shortcut {
       keySequence: fromStandardKey( StandardKey.Quit )
       onTriggered: Qt.quit()
   }
   @endcode

   Another one is to specify a string describing the sequence:

   @code
   Shortcut {
       keySequence: qsTr( "Ctrl+Q" )
       onTriggered: Qt.quit()
   }
   @endcode

   @sa keySequence
   @sa fromStandardKey
 */
void Shortcut::setKeySequence(const QVariant &keySequence)
{
    QKeySequence ke = keySequence.value<QKeySequence>();
    if ( ke != m_keySequence ) {
        m_keySequence = ke;
        emit keySequenceChanged();
    }
}

/**
   @brief Enable or disable the shortcut

   This sets the shortcut to @p enabled.
 */
void Shortcut::setEnabled(bool enabled)
{
    if ( m_enabled != enabled ) {
        m_enabled = enabled;
        emit enabledChanged();
    }
}

/**
   @brief Creates a key sequence from a standard key

   This is a helper method. It takes as input a @p standardKey from the set defined in
   QKeySequence::StandardKey and returns a QKeySequence (wrapped in a QVariant):

   @code
   Shortcut {
       keySequence: fromStandardKey( StandardKey.Quit )
   }
   @endcode
 */
QVariant Shortcut::fromStandardKey(QKeySequence::StandardKey standardKey)
{
    return QKeySequence( standardKey );
}

bool Shortcut::eventFilter(QObject *o, QEvent *ev )
{
    if ( m_enabled && !m_keySequence.isEmpty() && ev->type() == QEvent::KeyPress ) {
        QKeyEvent *keyEv = static_cast<QKeyEvent*>( ev );

        // Discard key modifiers
        if ( keyEv->key() >= Qt::Key_Shift && keyEv->key() <= Qt::Key_Alt ) {
            return QObject::eventFilter( o, ev );
        }

        int key = keyEv->modifiers() + keyEv->key();
        if ( QKeySequence(key) == m_keySequence ) {
            emit triggered();
            return true;
        }
    }
    return QObject::eventFilter( o, ev );
}

} /* SystemIntegration */

} /* OpenTodoList */
