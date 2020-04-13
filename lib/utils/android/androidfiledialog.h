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

#ifndef UTILS_ANDROID_ANDROIDFILEDIALOG_H_
#define UTILS_ANDROID_ANDROIDFILEDIALOG_H_

#include <QAndroidActivityResultReceiver>
#include <QAndroidJniObject>
#include <QObject>
#include <QUrl>
#include <QtAndroid>

/**
 * @brief Select files on Android via Intents.
 *
 * Inspired by https://stackoverflow.com/a/30487842/6367098.
 */
class AndroidFileDialog : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QObject *receiver READ receiver WRITE setReceiver NOTIFY receiverChanged)

public:
    enum Type { SelectFile, SelectImage };

    Q_ENUM(Type)

    explicit AndroidFileDialog(QObject *parent = nullptr);
    virtual ~AndroidFileDialog();

    Type type() const;
    void setType(const Type &type);

    QObject *receiver() const;
    void setReceiver(QObject *receiver);

signals:

    void typeChanged();
    void fileUrlChanged();
    void receiverChanged();

public slots:

    bool open();

private:
    class ResultReceiver : public QAndroidActivityResultReceiver
    {

    public:
        explicit ResultReceiver(AndroidFileDialog *dialog);
        virtual ~ResultReceiver();
        void handleActivityResult(int receiverRequestCode, int resultCode,
                                  const QAndroidJniObject &data);
        QString uriToPath(QAndroidJniObject uri);

    private:
        AndroidFileDialog *m_dialog;
    };

    static const int EXISTING_FILE_NAME_REQUEST = 1;
    static const int EXISTING_IMAGE_NAME_REQUEST = 2;

    ResultReceiver *m_receiver;
    QObject *m_resultReceiver;
    Type m_type;

    bool openFile();
    bool openImage();

    bool ensureCanAccessImages();
};

#endif // UTILS_ANDROID_ANDROIDFILEDIALOG_H_
