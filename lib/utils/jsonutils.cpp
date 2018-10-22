#include "jsonutils.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QSaveFile>


namespace JsonUtils {

static Q_LOGGING_CATEGORY(log, "OpenTodoList.JsonUtils", QtWarningMsg)

/**
 * @brief Write a JSON file, keeping existing properties in the file.
 *
 * This function can be used to write the properties contained in @p data to the
 * filename. If filename points to an existing file, any properties that exist in the
 * file but not in data will be preserved.
 *
 * The intention of this function is to keep a set of files backwards compatible (e.g. if
 * users switch between versions of the application).
 *
 * The function returns true on success or false otherwise.
 *
 * Note that this function will not touch the target file if there are no
 * changed. If you need to know if the file was actually written, pass in a
 * pointer to a boolean via the @p changed parameter.
 */
bool patchJsonFile(const QString& filename, const QVariantMap& data,
                   bool* changed)
{
    bool result = false;
    QFile file(filename);
    QVariantMap properties;
    QByteArray existingFileContent;
    bool hasChanged = false;
    if (file.exists()) {
        if (file.open(QIODevice::ReadOnly)) {
            QJsonParseError error;
            existingFileContent = file.readAll();
            auto doc = QJsonDocument::fromJson(existingFileContent, &error);
            if (error.error == QJsonParseError::NoError) {
                properties = doc.toVariant().toMap();
            } else {
                qCWarning(log) << "Failed to parse" << filename << ":"
                                     << error.errorString();
            }
            file.close();
        } else {
            qCWarning(log) << "Failed to open" << filename << "for reading:"
                                 << file.errorString();
        }
    }
    for (auto key : data.keys()) {
        properties[key] = data[key];
    }
    auto doc = QJsonDocument::fromVariant(properties);
    auto newFileContent = doc.toJson(QJsonDocument::Indented);
    if (newFileContent != existingFileContent) {
        QFileInfo fi(filename);
        fi.dir().mkpath(".");
        QSaveFile saveFile(filename);
        if (saveFile.open(QIODevice::WriteOnly)) {
            saveFile.write(newFileContent);
            result = saveFile.commit();
            hasChanged = result;
        } else {
            qCWarning(log) << "Failed to open" << filename << "for writing:"
                                 << file.errorString();
        }
    } else {
        qCDebug(log) << "File" << filename << "was not changed - "
                           << "skipping rewrite";
        result = true;
    }
    if (changed != nullptr) {
        *changed = hasChanged;
    }
    return result;
}


/**
 * @brief Load a variant map from a JSON file.
 */
QVariantMap loadMap(const QString& filename, bool* ok)
{
    bool success = false;
    QVariantMap result;
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonParseError error;
        auto doc = QJsonDocument::fromJson(file.readAll(), &error);
        if (error.error == QJsonParseError::NoError) {
            result = doc.toVariant().toMap();
            success = true;
        } else {
            qCWarning(log) << "Failed to parse" << filename << ":"
                                 << error.errorString();
        }
        file.close();
    } else {
        qCDebug(log) << "Failed to open" << filename << "for reading:"
                             << file.errorString();
    }
    if (ok != nullptr) {
        *ok = success;
    }
    return result;
}

}

