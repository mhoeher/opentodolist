#include "library.h"

#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QtConcurrent>


const QString Library::LibraryFileName = "library.json";


/**
   @brief Set the name of the library.
 */
Library::Library(QObject* parent) : QObject(parent),
    m_name(),
    m_directory()
{

}

Library::Library(const QString& directory, QObject* parent) : Library(parent)
{
    m_directory = directory;
    load();
}

Library::~Library()
{
}

void Library::setName(const QString &name)
{
    if ( m_name != name ) {
        m_name = name;
        emit nameChanged();
        save();
    }
}

/**
 * @brief Remove the library from the application.
 *
 * This method removes the library from the application. Basically, it emits the libraryDeleted()
 * signal and then schedules the Library object for deletion.
 */
void Library::deleteLibrary()
{
    QString directory;
    QtConcurrent::run([=](){
        QDir dir(directory);
        for (auto entry : dir.entryList(QDir::Files)) {
            dir.remove(entry);
        }
        dir.remove(".");
    });
    emit libraryDeleted(this);
    deleteLater();
}

bool Library::load()
{
    bool result = false;
    if (isValid()) {
        QDir dir(m_directory);
        QString filename = dir.absoluteFilePath(LibraryFileName);
        QFile file(filename);
        if (file.exists()) {
            if (file.open(QIODevice::ReadWrite)) {
                QJsonParseError error;
                auto map = QJsonDocument::fromJson(file.readAll(), &error).toVariant().toMap();
                if (error.error == QJsonParseError::NoError) {
                    fromMap(map);
                    result = true;
                } else {
                    qWarning() << "Failed to parse file" << filename <<":" << error.errorString();
                }
                file.close();

            } else {
                qWarning() << "Failed to open file" << filename << "for reading:"
                           << file.errorString();
            }
        }
    }
    return result;
}

bool Library::save()
{
    bool result = false;
    if (isValid()) {
        QDir dir(m_directory);
        QString filename = dir.absoluteFilePath(LibraryFileName);
        QFile file(filename);
        QVariantMap map;
        if (file.exists()) {
            if (file.open(QIODevice::ReadOnly)) {
                QJsonParseError error;
                map = QJsonDocument::fromJson(file.readAll(), &error).toVariant().toMap();
                if (error.error != QJsonParseError::NoError) {
                    qWarning() << "Failed to parse" << filename << ":" << error.errorString();
                }
                file.close();
            } else {
                qWarning() << "Failed to open" << filename << "for reading:" << file.errorString();
            }
        }
        auto newMap = toMap();
        for (auto key : newMap.keys()) {
            map[key] = newMap[key];
        }
        if (file.open(QIODevice::WriteOnly)) {
            auto data = QJsonDocument::fromVariant(map).toJson(QJsonDocument::Indented);
            file.write(data);
            result = true;
            file.close();
        } else {
            qWarning() << "Failed to open" << filename << "for writing:" << file.errorString();
        }
    }
    return result;
}

QVariantMap Library::toMap() const
{
    QVariantMap result;
    result["name"] = m_name;
    return result;
}

void Library::fromMap(QVariantMap map)
{
    setName(map.value("name", m_name).toString());
}
