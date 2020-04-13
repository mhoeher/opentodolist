/*
 * Copyright 2020 Martin Hoeher <martin@rpdev.net>
 +
 * This file is part of OpenTodoList.
 *
 * OpenTodoList is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * OpenTodoList is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenTodoList.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "androidfiledialog.h"

#include <QFileInfo>
#include <QThread>
#include <QUrl>
#include <QVariant>

AndroidFileDialog::AndroidFileDialog(QObject *parent)
    : QObject(parent),
      m_receiver(new ResultReceiver(this)),
      m_resultReceiver(nullptr),
      m_type(SelectFile)
{
}

AndroidFileDialog::~AndroidFileDialog()
{
    delete m_receiver;
}

bool AndroidFileDialog::open()
{
    switch (m_type) {
    case SelectFile:
        return openFile();
    case SelectImage:
        return openImage();
    }
}

QObject *AndroidFileDialog::receiver() const
{
    return m_resultReceiver;
}

void AndroidFileDialog::setReceiver(QObject *receiver)
{
    m_resultReceiver = receiver;
    if (m_resultReceiver != receiver) {
        m_resultReceiver = receiver;
        emit receiverChanged();
    }
}

bool AndroidFileDialog::openFile()
{
    QAndroidJniObject ACTION_GET_CONTENT =
            QAndroidJniObject::fromString("android.intent.action.GET_CONTENT");
    QAndroidJniObject intent("android/content/Intent");
    if (ACTION_GET_CONTENT.isValid() && intent.isValid()) {
        intent.callObjectMethod("setAction", "(Ljava/lang/String;)Landroid/content/Intent;",
                                ACTION_GET_CONTENT.object<jstring>());
        intent.callObjectMethod("setType", "(Ljava/lang/String;)Landroid/content/Intent;",
                                QAndroidJniObject::fromString("file/*").object<jstring>());
        QtAndroid::startActivity(intent.object<jobject>(), EXISTING_FILE_NAME_REQUEST, m_receiver);
        return true;
    } else {
        return false;
    }
}

bool AndroidFileDialog::openImage()
{
    if (!ensureCanAccessImages()) {
        return false;
    }

    QAndroidJniObject ACTION_PICK = QAndroidJniObject::getStaticObjectField(
            "android/content/Intent", "ACTION_PICK", "Ljava/lang/String;");
    QAndroidJniObject EXTERNAL_CONTENT_URI =
            QAndroidJniObject::getStaticObjectField("android/provider/MediaStore$Images$Media",
                                                    "EXTERNAL_CONTENT_URI", "Landroid/net/Uri;");

    QAndroidJniObject intent = QAndroidJniObject(
            "android/content/Intent", "(Ljava/lang/String;Landroid/net/Uri;)V",
            ACTION_PICK.object<jstring>(), EXTERNAL_CONTENT_URI.object<jobject>());

    if (ACTION_PICK.isValid() && intent.isValid()) {
        intent.callObjectMethod("setType", "(Ljava/lang/String;)Landroid/content/Intent;",
                                QAndroidJniObject::fromString("image/*").object<jstring>());
        QtAndroid::startActivity(intent.object<jobject>(), EXISTING_IMAGE_NAME_REQUEST, m_receiver);
        return true;
    } else {
        return false;
    }
}

bool AndroidFileDialog::ensureCanAccessImages()
{
    const auto ReadExtStorage = "android.permission.READ_EXTERNAL_STORAGE";
    if (QtAndroid::checkPermission(ReadExtStorage) == QtAndroid::PermissionResult::Denied) {
        auto resultHash = QtAndroid::requestPermissionsSync(QStringList({ ReadExtStorage }));
        if (resultHash[ReadExtStorage] == QtAndroid::PermissionResult::Denied)
            return false;
    }
    return true;
}

AndroidFileDialog::Type AndroidFileDialog::type() const
{
    return m_type;
}

void AndroidFileDialog::setType(const Type &type)
{
    if (m_type != type) {
        m_type = type;
        emit typeChanged();
    }
}

AndroidFileDialog::ResultReceiver::ResultReceiver(AndroidFileDialog *dialog)
    : QAndroidActivityResultReceiver(), m_dialog(dialog)
{
}

AndroidFileDialog::ResultReceiver::~ResultReceiver() {}

void AndroidFileDialog::ResultReceiver::handleActivityResult(int receiverRequestCode,
                                                             int resultCode,
                                                             const QAndroidJniObject &data)
{
    QString path;
    jint RESULT_OK = QAndroidJniObject::getStaticField<jint>("android/app/Activity", "RESULT_OK");
    if (resultCode == RESULT_OK) {
        switch (receiverRequestCode) {
        case EXISTING_FILE_NAME_REQUEST: {
            QAndroidJniObject uri = data.callObjectMethod("getData", "()Landroid/net/Uri;");
            path = uriToPath(uri);
            break;
        }
        case EXISTING_IMAGE_NAME_REQUEST: {
            QAndroidJniObject uri = data.callObjectMethod("getData", "()Landroid/net/Uri;");
            path = uriToPath(uri);
            break;
        }
        }
    }
    if (m_dialog->m_resultReceiver != nullptr) {
        if (!path.isNull()) {
            if (!QFileInfo(path).isFile()) {
                path = QString();
            }
        }
        if (!path.isNull()) {
            auto url = QUrl::fromLocalFile(path);
            QMetaObject::invokeMethod(m_dialog->m_resultReceiver, "openFinished",
                                      Qt::QueuedConnection, Q_ARG(QVariant, url));
        } else {
            QMetaObject::invokeMethod(m_dialog->m_resultReceiver, "openFinished",
                                      Qt::QueuedConnection, Q_ARG(QVariant, QVariant()));
        }
    }
}

QString AndroidFileDialog::ResultReceiver::uriToPath(QAndroidJniObject uri)
{
    if (uri.toString().startsWith("file:", Qt::CaseInsensitive)) {
        return uri.callObjectMethod("getPath", "()Ljava/lang/String;").toString();
    } else {
        QAndroidJniObject contentResolver = QtAndroid::androidActivity().callObjectMethod(
                "getContentResolver", "()Landroid/content/ContentResolver;");
        QAndroidJniObject cursor = contentResolver.callObjectMethod(
                "query",
                "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;"
                "[Ljava/lang/String;"
                "Ljava/lang/String;)Landroid/database/Cursor;",
                uri.object<jobject>(), 0, 0, 0, 0);
        QAndroidJniObject DATA = QAndroidJniObject::fromString("_data");
        jint columnIndex = cursor.callMethod<jint>("getColumnIndexOrThrow", "(Ljava/lang/String;)I",
                                                   DATA.object<jstring>());
        cursor.callMethod<jboolean>("moveToFirst", "()Z");
        QAndroidJniObject result =
                cursor.callObjectMethod("getString", "(I)Ljava/lang/String;", columnIndex);
        return result.isValid() ? result.toString() : QString();
    }
}
