#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QString>

class FileUtils
{
public:
  FileUtils() = delete;
  
  static bool isSubDirOrFile(const QString &dir, const QString &subDir);
};

#endif // FILEUTILS_H
