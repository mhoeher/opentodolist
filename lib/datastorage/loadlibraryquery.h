#ifndef LOADLIBRARYQUERY_H
#define LOADLIBRARYQUERY_H

#include "itemsquery.h"

#include <QString>
#include <QUuid>

class LoadLibraryQuery : public ItemsQuery
{
    Q_PROPERTY(QString directory READ directory WRITE setDirectory)
    Q_PROPERTY(QUuid libraryId READ libraryId WRITE setLibraryId)
public:
    explicit LoadLibraryQuery(QObject* parent = nullptr);

    // ItemsQuery interface
    QString directory() const;
    void setDirectory(const QString& newDirectory);

    QUuid libraryId() const;
    void setLibraryId(const QUuid& newLibraryId);

signals:
    void directoryChanged();
    void libraryIdChanged();

protected:
    void run() override;

private:
    QString m_directory;
    QUuid m_libraryId;
};

#endif // LOADLIBRARYQUERY_H
