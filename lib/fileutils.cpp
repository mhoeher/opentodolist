#include "fileutils.h"

#include "../datamodel/library.h"

#include <QDir>
#include <QFileInfo>

bool FileUtils::isSubDirOrFile(const QString &dir, const QString &subDir)
{
  QString parDir = QDir::fromNativeSeparators(dir);
  QString childDir = QDir::fromNativeSeparators(subDir);
  if (!parDir.endsWith("/")) {
    parDir = parDir + "/";
  }
  return childDir.startsWith(parDir) || (dir == subDir);
}


/**
 * @brief Convert a path to make it suitable for persisting.
 *
 * This converts the given @p path and returns a representation which
 * is suitable for being saved to disk. In particular, it:
 *
 * - Strips the default library location from the beginning and replaces it
 *   with a placeholder. This is required on some platforms like iOS, where
 *   the default app locations change between installs/updates.
 */
QString FileUtils::toPersistedPath(const QString &path)
{
    auto result = path;
    auto libsLocation = Library::defaultLibrariesLocation();
    if (result.startsWith(libsLocation)) {
        result = "<APPROOT>" + result.mid(libsLocation.length());
    }
    return result;
}


/**
 * @brief Get a path from its persisted form.
 *
 * This is basically the reverse operation of @sa toPersistedPath(). It takes
 * a @p path in its persisted form and returns a valid local path.
 */
QString FileUtils::fromPersistedPath(const QString &path)
{
    auto result = path;
    QString placeholder = "<APPROOT>";
    if (result.startsWith(placeholder)) {
        result = Library::defaultLibrariesLocation() +
                result.mid(placeholder.length());
    }
    return result;
}
