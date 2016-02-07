#ifndef LOCALLIBRARYFACTORY_H
#define LOCALLIBRARYFACTORY_H

#include "libraryfactory.h"

#include <QObject>


// Forward declaration:
class Application;


/**
   @brief A class which creates Library objects
   
   This LibraryFactory implementation is used to create Library objects, which just encapsulate
   a local directory.
 */
class LocalLibraryFactory : public LibraryFactory
{
  Q_OBJECT
  
  friend class Application;

public:
  
  static const QString ID;
  
  // LibraryFactory interface
  Library *createLibrary(const QString &name,
                         const QString &directory,
                         const QVariantMap &args = QVariantMap(),
                         QObject *parent = 0) const override;
  
signals:
  
public slots:
  
private:
  
  explicit LocalLibraryFactory(QObject *parent = 0);
  
};

#endif // LOCALLIBRARYFACTORY_H
