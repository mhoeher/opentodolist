#ifndef IMAGE_H
#define IMAGE_H

#include "toplevelitem.h"

#include <QObject>
#include <QUrl>

class Image : public TopLevelItem
{
    Q_OBJECT
    Q_PROPERTY(QString image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(QUrl imageUrl READ imageUrl NOTIFY imageChanged)
    Q_PROPERTY(bool validImage READ validImage NOTIFY imageChanged)
public:

    explicit Image(const QString& filename, QObject *parent = nullptr);
    explicit Image(QObject *parent = nullptr);
    explicit Image(const QDir &dir, QObject *parent = nullptr);
    virtual ~Image();

    /**
     * @brief The path to the image relative to the item's directory.
     */
    QString image() const { return m_image; }
    void setImage(const QString &image);

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

#endif // IMAGE_H
