#ifndef LIBRARYFACTORY_H
#define LIBRARYFACTORY_H

#include <QObject>
#include <QPointer>
#include <QString>
#include <QVariantMap>
#include <QVector>

// Forward declaration:
class Library;


/**
   @brief A factory class to create Library objects.
   
   The LibraryFactory class is an abstract base class which is used to create Library objects.
 */
class LibraryFactory : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString name READ name CONSTANT)
  Q_PROPERTY(QString id READ id CONSTANT)
  Q_PROPERTY(QString description READ description CONSTANT)
  Q_PROPERTY(QString createComponent READ createComponent CONSTANT)
  Q_PROPERTY(QString editComponent READ editComponent CONSTANT)

public:
  
  /**
     @brief The type name of libraries created by the factory, as shown in the user interface.
   */
  QString name() const            { return m_name; }
  
  /**
     @brief The unique ID of the factory.
   */
  QString id() const              { return m_id; }
  
  /**
     @brief The description of the type of libraries created by the factory.
   */
  QString description() const     { return m_description; }
  
  /**
     @brief The name of a QML file which provides a component for creating new libraries.
   */
  QString createComponent() const { return m_createComponent; }
  
  /**
     @brief The name of a QML file which provides a component for editing existing libraries.
   */
  QString editComponent() const   { return m_editComponent; }
  
  /**
     @brief Create a new library.
     
     This method can be used to create a new Library object with the given @p name and
     @p directory. If specified, the library will be a child of the @p parent.
     
     @note Sub-classes must implement this method in order to be instantiated.
   */
  virtual Library* createLibrary(const QString &name,
                                 const QString &directory,
                                 const QVariantMap &args = QVariantMap(),
                                 QObject *parent = 0) const = 0;
  
signals:
  
public slots:
  
protected:
  
  explicit LibraryFactory(const QString &name = QString(),
                          const QString &id = QString(),
                          const QString &description = QString(),
                          const QString &createComponent = QString(),
                          const QString &editComponent = QString(),
                          QObject *parent = 0);
  
private:
  
  QString m_name;
  QString m_id;
  QString m_description;
  QString m_createComponent;
  QString m_editComponent;
};


/**
   @brief A list of factories.
 */
typedef QVector<LibraryFactory*> LibraryFactories;

#endif // LIBRARYFACTORY_H
