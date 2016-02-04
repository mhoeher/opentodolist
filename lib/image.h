#ifndef IMAGE_H
#define IMAGE_H

#include "toplevelitem.h"

#include <QObject>

class Image : public TopLevelItem
{
  Q_OBJECT
  Q_PROPERTY(QString image READ image WRITE setImage NOTIFY imageChanged)
public:
  static const QString ItemType;
  static const QStringList PersistentProperties;
  
  explicit Image(const QString &directory = QString(), QObject *parent = 0);
  
  /**
     @brief The path to the image relative to the item's directory.
   */
  QString image() const { return m_image; }
  void setImage(const QString &image);
  
signals:
  
  /**
     @brief The image URL has changed.
   */
  void imageChanged();
  
public slots:
  
private:
  
  QString m_image;
  
};

#endif // IMAGE_H
