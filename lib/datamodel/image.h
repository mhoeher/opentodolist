#ifndef IMAGE_H
#define IMAGE_H

#include "toplevelitem.h"

#include <QObject>

class Image : public TopLevelItem
{
  Q_OBJECT
  Q_PROPERTY(QString image READ image WRITE setImage NOTIFY imageChanged)
  Q_PROPERTY(bool busy READ busy WRITE setBusy NOTIFY busyChanged)
  Q_PROPERTY(bool validImage READ validImage NOTIFY imageChanged)
public:
  static const QString ItemType;
  static const QStringList PersistentProperties;
  
  explicit Image(const QString &directory = QString(), QObject *parent = 0);
  
  /**
     @brief The path to the image relative to the item's directory.
   */
  QString image() const { return m_image; }
  void setImage(const QString &image);
  
  bool validImage() const;
  
  /**
     @brief Returns whether the image is performing some background task.
   */
  bool busy() const { return m_busy; }
  
signals:
  
  /**
     @brief The image URL has changed.
   */
  void imageChanged();
  
  /**
     @brief The busy status of the image changed.
   */
  void busyChanged();
  
public slots:
  
protected:
  // Item interface
  bool deleteItemData() override;

private:
  
  QString m_image;
  bool    m_busy;
  
  void setBusy(bool busy);
  
  QString findLocalFileName(const QString &image) const;
  
  static void copyImage(const QString &imgFrom, const QString &imgTo);
  static void removeImage(const QString &image);
  
  
};

#endif // IMAGE_H
