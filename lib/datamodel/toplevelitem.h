#ifndef TOPLEVELITEM_H
#define TOPLEVELITEM_H

#include "complexitem.h"

#include <QObject>

/**
 * @brief Base class for all top level items.
 *
 * The TopLevelItem class is used as a common base class for all top level items, i.e. items
 * directly contained in a Library.
 */
class TopLevelItem : public ComplexItem
{
    Q_OBJECT

    Q_PROPERTY(Color color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QStringList tags READ tags WRITE setTags NOTIFY tagsChanged)
    Q_PROPERTY(QUuid libraryId READ libraryId WRITE setLibraryId
               NOTIFY libraryIdChanged)
public:

    enum Color {
        White,
        Red,
        Green,
        Blue,
        Orange,
        Yellow,
        Lilac
    };

    Q_ENUM(Color)

    explicit TopLevelItem(const QString &filename, QObject* parent = nullptr);
    explicit TopLevelItem(QObject* parent = nullptr);
    explicit TopLevelItem(const QDir &dir, QObject *parent = nullptr);
    ~TopLevelItem() override;

    QUuid parentId() const override;

    Color color() const;
    void setColor(const Color &color);
    void setColor(const QString &color);

    QStringList tags() const;
    void setTags(const QStringList& tags);
    Q_INVOKABLE void addTag(const QString &tag);
    Q_INVOKABLE void removeTagAt(int index);
    Q_INVOKABLE void removeTag(const QString &tag);
    Q_INVOKABLE bool hasTag(const QString &tag) const;

    QUuid libraryId() const;
    void setLibraryId(const QUuid &libraryId);

signals:

    void colorChanged();
    void tagsChanged();
    void libraryIdChanged();

public slots:

protected:

private:

    QUuid m_libraryId;
    Color m_color;
    QStringList m_tags;

    // Item interface
protected:
    QVariantMap toMap() const override;
    void fromMap(QVariantMap map) override;
};

typedef QSharedPointer<TopLevelItem> TopLevelItemPtr;

#endif // TOPLEVELITEM_H
