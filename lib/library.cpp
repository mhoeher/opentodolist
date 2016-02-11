#include "library.h"

#include "image.h"
#include "note.h"
#include "todolist.h"

#include <QDir>
#include <QSet>
#include <QQueue>

/**
   @brief Set the name of the library.
 */
void Library::setName(const QString &name)
{
  if ( m_name != name ) {
    m_name = name;
    emit nameChanged();
  }
}

/**
   @brief Set the directory for which the library returns objects.
 */
void Library::setDirectory(const QString &directory)
{
  if ( m_directory != directory ) {
    // TODO: Trigger search for new/changed items
    m_directory = directory;
    emit directoryChanged();
  }
}

/**
   @brief Return the arguments which are required to restore the library.
   
   This method returns the additional arguments which are required in order to restore the
   library. The arguments returned will be passed to the LibraryFactory::createLibrary()
   method to restore the library.
   
   Sub-classes shall extend this method to include their specific arguments.
 */
QVariantMap Library::saveArgs() const
{
  return QVariantMap();
}

Note *Library::addNote(const QString &title)
{
  auto result = new Note(itemPathFromTitle(title, Note::ItemType), this);
  Q_CHECK_PTR(result);
  result->setTitle(title);
  addItem(result);
  return result;
}

Image *Library::addImage(const QString &title, const QString &image)
{
  auto result = new Image(itemPathFromTitle(title, Image::ItemType), this);
  Q_CHECK_PTR(result);
  result->setTitle(title);
  result->setImage(image);
  addItem(result);
  return result;
}

TodoList *Library::addTodoList(const QString &title)
{
  auto result = new TodoList(itemPathFromTitle(title, TodoList::ItemType), this);
  Q_CHECK_PTR(result);
  result->setTitle(title);
  addItem(result);
  return result;
}

TopLevelItemList Library::items()
{
  loadItems();
  return m_items;
}

/**
   @brief Constructor.
   
   Creates a new library object with the given @p name and the @p directory. The library
   is created by the @p factory. The object is a child of the given @p parent.
 */
Library::Library(const QString &name, 
                 const QString &directory,
                 const LibraryFactory *factory,
                 QObject *parent) : QObject(parent),
  m_name( name ),
  m_directory( directory ),
  m_factory( factory ),
  m_items(),
  m_itemsLoaded(false)
{
}

QString Library::itemPathFromTitle(const QString &title, const QString &itemType) const
{
  QString baseDir = dirForItemType(itemType);
  QString tpl = baseDir + "/" + Item::titleToDirectoryName(title);
  int i = 0;
  QString result = tpl;
  while (QDir(tpl).exists()) {
    result = tpl + " - " + QString::number(i++);
  }
  return result;
}

void Library::addItem(TopLevelItem *item)
{
  item->commitItem();
  connect(item, &Item::itemDeleted, this, &Library::onTopLevelItemDeleted);
  m_items.append(item);
}

void Library::loadItems()
{
  if (!m_itemsLoaded) {
    QQueue<QString> dirs;
    QSet<QString> visitedDirs;
    dirs.enqueue(QFileInfo(m_directory).canonicalFilePath());
    while (!dirs.isEmpty()) {
      QString dir = dirs.dequeue();
      visitedDirs.insert(dir);
      if (Item::isItemDirectory<Note>(dir)) {
        auto note = new Note(dir, this);
        Q_CHECK_PTR(note);
        if (containsItem(note->uid())) {
          delete note;
          continue;
        }
        addItem(note);
      } else if (Item::isItemDirectory<Image>(dir)) {
        auto image = new Image(dir, this);
        Q_CHECK_PTR(image);
        if (containsItem(image->uid())) {
          delete image;
          continue;
        }
        addItem(image);
      } else if(Item::isItemDirectory<TodoList>(dir)) {
        auto todoList = new TodoList(dir, this);
        Q_CHECK_PTR(todoList);
        if (containsItem(todoList->uid())) {
          delete todoList;
          continue;
        }
        addItem(todoList);
      } else {
        QDir d(dir);
        for (auto entry : d.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
          QString newPath = QFileInfo(d.absoluteFilePath(entry)).canonicalFilePath();
          if (!visitedDirs.contains(newPath)) {
            dirs.enqueue(newPath);
          }
        }
      }
    }
    m_itemsLoaded = true;
  }
}

bool Library::containsItem(const QUuid &uid) const
{
  for (auto item : m_items) {
    if (item->uid() == uid) {
      return true;
    }
  }
  return false;
}

QString Library::dirForItemType(const QString &itemType) const
{
  QString baseDir = m_directory;
  QString appDir = baseDir + "/OpenTodoList";
  int i = 0;
  while (QFile::exists(appDir)) {
    appDir = baseDir + "/OpenTodoList - " + QString::number(i++);
  }
  QString typeDir = appDir + "/" + itemType;
  i = 0;
  while (QFile::exists(typeDir)) {
    typeDir = appDir + "/" + itemType + " - " + QString::number(i);
  }
  return typeDir;
}

void Library::onTopLevelItemDeleted(Item *item)
{
  Q_CHECK_PTR(item);
  for (int i = 0; i < m_items.size(); ++i) {
    if (m_items.at(i) == item) {
      m_items.removeAt(i);
      item->deleteLater();
      return;
    }
  }
}
