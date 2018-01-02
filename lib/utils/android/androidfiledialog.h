#ifndef ANDROIDFILEDIALOG_H
#define ANDROIDFILEDIALOG_H

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
    Q_PROPERTY(QObject* receiver READ receiver WRITE setReceiver NOTIFY receiverChanged)

public:

    enum Type {
        SelectFile,
        SelectImage
    };

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

    class ResultReceiver : public QAndroidActivityResultReceiver {

    public:

        ResultReceiver(AndroidFileDialog *dialog);
        virtual ~ResultReceiver();
        void handleActivityResult(int receiverRequestCode,
                                  int resultCode,
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

};

#endif // ANDROIDFILEDIALOG_H
