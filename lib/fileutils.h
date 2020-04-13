#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QString>

class FileUtils
{
public:
    FileUtils() = delete;

    static bool isSubDirOrFile(const QString &dir, const QString &subDir);
    static QString toPersistedPath(const QString &path);
    static QString fromPersistedPath(const QString &path);
};

#endif // FILEUTILS_H
