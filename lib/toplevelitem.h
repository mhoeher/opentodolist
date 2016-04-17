#ifndef TOPLEVELITEM_H
#define TOPLEVELITEM_H

#include "complexitem.h"

#include <QObject>

/**
   @brief Base class for all top level items.
   
   The TopLevelItem class is used as a common base class for all top level items, i.e. items
   directly contained in a Library.
 */
class TopLevelItem : public ComplexItem
{
    Q_OBJECT
    Q_PROPERTY(Color color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QString colorName READ colorName WRITE setColorName NOTIFY colorChanged)
    Q_PROPERTY(QStringList tags READ tags WRITE setTags NOTIFY tagsChanged)
public:
    
    static const QStringList PersistentProperties;
    static const QString ItemType;
    
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
    
    explicit TopLevelItem(const QString &directory = QString(),
                          const QString &itemType = ItemType,
                          const QStringList &persistentProperties = QStringList(),
                          QObject *parent = 0);
    
    
    
    /**
     @brief The color of the item.
   */
    Color color() const { return m_color; }
    void setColor(const Color &color);
    
    QString colorName() const;
    void setColorName(const QString &color);
    
    QStringList tags() const;
    void setTags(const QStringList& tags);
    Q_INVOKABLE void addTag(const QString &tag);
    Q_INVOKABLE void removeTagAt(int index);
    Q_INVOKABLE bool hasTag(const QString &tag) const;
    
signals:
    
    void colorChanged();
    void tagsChanged();
    
public slots:
    
protected:
    
    explicit TopLevelItem(bool loadItem,
                          const QString &directory = QString(),
                          const QString &itemType = ItemType,
                          const QStringList &persistentProperties = QStringList(),
                          QObject *parent = 0);
    
private:
    
    Color m_color;
    QStringList m_tags;
};

#endif // TOPLEVELITEM_H
