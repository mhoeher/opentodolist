#ifndef SHORTCUT_H
#define SHORTCUT_H

#include <QKeySequence>
#include <QObject>
#include <QVariant>

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

#endif // SHORTCUT_H
