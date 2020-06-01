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

#ifndef DATAMODEL_IMAGE_H_
#define DATAMODEL_IMAGE_H_

#include <QObject>
#include <QUrl>

#include "toplevelitem.h"

class Image : public TopLevelItem
{
    Q_OBJECT
    Q_PROPERTY(QString image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(QUrl imageUrl READ imageUrl NOTIFY imageChanged)
    Q_PROPERTY(bool validImage READ validImage NOTIFY imageChanged)
public:
    explicit Image(const QString& filename, QObject* parent = nullptr);
    explicit Image(QObject* parent = nullptr);
    explicit Image(const QDir& dir, QObject* parent = nullptr);
    ~Image() override;

    /**
     * @brief The path to the image relative to the item's directory.
     */
    QString image() const { return m_image; }
    void setImage(const QString& image);

    QUrl imageUrl() const;

    bool validImage() const;

signals:

    /**
     * @brief The image URL has changed.
     */
    void imageChanged();

public slots:

protected:
    // Item interface
    QVariantMap toMap() const override;
    void fromMap(QVariantMap map) override;
    bool deleteItem() override;

private:
    QString m_image;
};

typedef QSharedPointer<Image> ImagePtr;

#endif // DATAMODEL_IMAGE_H_
