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

#ifndef DATAMODEL_NOTEPAGE_H_
#define DATAMODEL_NOTEPAGE_H_

#include <QObject>

#include "datamodel/complexitem.h"

class NotePage : public ComplexItem
{
    Q_OBJECT
    Q_PROPERTY(QUuid noteUid READ noteUid WRITE setNoteUid NOTIFY noteUidChanged)
public:
    explicit NotePage(QObject *parent = nullptr);
    explicit NotePage(const QString &filename, QObject *parent = nullptr);
    explicit NotePage(const QDir &dir, QObject *parent = nullptr);
    ~NotePage() override;

    QUuid noteUid() const;
    void setNoteUid(const QUuid &noteUid);

    QUuid parentId() const override;

signals:

    void noteUidChanged();

protected:
    // Item interface
    QVariantMap toMap() const override;
    void fromMap(QVariantMap map) override;

private:
    QUuid m_noteUid;
};

#endif // DATAMODEL_NOTEPAGE_H_
