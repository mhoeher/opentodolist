#include "fileutils.h"

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
