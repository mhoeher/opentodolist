/*
 * Copyright 2020 Martin Hoeher <martin@rpdev.net>
 +
 * This file is part of OpenTodoList.
 *
 * OpenTodoList is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * OpenTodoList is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenTodoList.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DATAMODEL_COMPLEXITEM_H_
#define DATAMODEL_COMPLEXITEM_H_

#include <QDateTime>
#include <QLoggingCategory>
#include <QObject>
#include <QFutureWatcher>

#include "item.h"

/**
 * @brief Complex items.
 *
 * The ComplexItem class is used to model more complex items. This class introduces some additional
 * properties that are not required by the simpler Item class but that nevertheless are common to
 * most other item types.
 */
class ComplexItem : public Item
{
    Q_OBJECT

    Q_PROPERTY(QDateTime dueTo READ dueTo WRITE setDueTo NOTIFY dueToChanged)
    Q_PROPERTY(QString notes READ notes WRITE setNotes NOTIFY notesChanged)
    Q_PROPERTY(QStringList attachments READ attachments NOTIFY attachmentsChanged)
public:
    explicit ComplexItem(QObject *parent = nullptr);
    explicit ComplexItem(const QString &filename, QObject *parent = nullptr);
    explicit ComplexItem(const QDir &dir, QObject *parent = nullptr);
    ~ComplexItem() override;

    QUuid parentId() const override;

    QDateTime dueTo() const;
    void setDueTo(const QDateTime &dueTo);

    QString notes();
    void setNotes(const QString &notes);

    QStringList attachments() const;
    Q_INVOKABLE QString attachmentFileName(const QString &filename);

    // Item interface
    bool deleteItem() override;

signals:

    void dueToChanged();
    void notesChanged();
    void attachmentsChanged();

public slots:

    void attachFile(const QString &filename);
    void detachFile(const QString &filename);

protected:
private:
    QDateTime m_dueTo;
    QString m_notes;
    QStringList m_attachments;

    void setupConnections();
    void setAttachments(const QStringList &attachments);

protected:
    // Item interface
    QVariantMap toMap() const override;
    void fromMap(QVariantMap map) override;
};

typedef QSharedPointer<ComplexItem> ComplexItemPtr;

#endif // DATAMODEL_COMPLEXITEM_H_
