#ifndef ABSTRACTTODO_H
#define ABSTRACTTODO_H

#include "opentodolistcore_global.h"

#include <QObject>

class OPENTODOLISTCORESHARED_EXPORT AbstractTodo : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString title READ title WRITE setTitle )
    Q_PROPERTY( QString description READ description WRITE setDescription )
    Q_PROPERTY( int progress READ progress WRITE setProgress )
    Q_PROPERTY( int priority READ priority WRITE setPriority )

public:
    explicit AbstractTodo(QObject *parent = 0);
    
    QString title() const;
    void setTitle(const QString &title);

    QString description() const;
    void setDescription(const QString &description);

    int progress() const;
    void setProgress(int progress);

    int priority() const;
    void setPriority(int priority);

signals:
    
public slots:

private:

    QString m_title;
    QString m_description;
    int     m_progress;
    int     m_priority;

};

#endif // ABSTRACTTODO_H
