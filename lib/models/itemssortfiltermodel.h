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

#ifndef MODELS_ITEMSSORTFILTERMODEL_H_
#define MODELS_ITEMSSORTFILTERMODEL_H_

#include <QDateTime>
#include <QJSValue>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QUuid>

class ItemsSortFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool groupDone READ groupDone WRITE setGroupDone NOTIFY groupDoneChanged)
public:
    explicit ItemsSortFilterModel(QObject* parent = nullptr);

    int count() const;

    Q_INVOKABLE int roleFromName(const QString& roleName) const;

    bool groupDone() const;
    void setGroupDone(bool newGroupDone);

signals:

    void countChanged();

    // QSortFilterProxyModel interface
    void groupDoneChanged();

protected:
    bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const override;

private:
    bool m_groupDone;
};

#endif // MODELS_ITEMSSORTFILTERMODEL_H_
