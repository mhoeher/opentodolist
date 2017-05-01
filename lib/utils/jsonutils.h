#ifndef JSONUTILS_H
#define JSONUTILS_H


#include <QString>
#include <QVariantMap>
#include <QLoggingCategory>


/**
 * @brief Various utils to work with JSON.
 *
 * This namespace holds utility functions to work with JSON data.
 */
namespace JsonUtils {

bool patchJsonFile(const QString &filename, const QVariantMap &data);
QVariantMap loadMap(const QString &filename, bool* ok = nullptr);


Q_DECLARE_LOGGING_CATEGORY(jsonUtils)

}


#endif // JSONUTILS_H
