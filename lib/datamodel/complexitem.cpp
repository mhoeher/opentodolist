#include "complexitem.h"

#include <QtConcurrent>
#include <QTextDocument>

static Q_LOGGING_CATEGORY(log, "OpenTodoList.ComplexItem", QtWarningMsg)

        /**
         * @brief Constructor.
         */
        ComplexItem::ComplexItem(QObject *parent)
    : ComplexItem(QString(), parent)
{
}

/**
 * @brief Constructor.
 */
ComplexItem::ComplexItem(const QString &filename, QObject *parent)
    : Item(filename, parent), m_dueTo(), m_notes(), m_attachments()
{
    setupConnections();
}

/**
 * @brief Constructor.
 */
ComplexItem::ComplexItem(const QDir &dir, QObject *parent)
    : Item(dir, parent), m_dueTo(), m_notes(), m_attachments()
{
    setupConnections();
}

/**
 * @brief Destructor.
 */
ComplexItem::~ComplexItem() {}

QUuid ComplexItem::parentId() const
{
    return QUuid();
}

/**
 * @brief The due date and time of the item.
 */
QDateTime ComplexItem::dueTo() const
{
    return m_dueTo;
}

/**
 * @brief Set the due date and time of the item.
 */
void ComplexItem::setDueTo(const QDateTime &dueTo)
{
    if (m_dueTo != dueTo) {
        m_dueTo = dueTo;
        // Note: Save only on a second basis.
        QTime t = dueTo.time();
        t.setHMS(t.hour(), t.minute(), t.second(), 0);
        m_dueTo.setTime(t);
        emit dueToChanged();
    }
}

/**
 * @brief Return the notes (as HTML) attached to the item.
 */
QString ComplexItem::notes()
{
    return m_notes;
}

/**
 * @brief Set the notes (as HTML) of the item.
 */
void ComplexItem::setNotes(const QString &notes)
{
    auto copy = notes;
    // Detect old-style HTML notes and convert to plain text:
    if (copy.startsWith("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" "
                        "\"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
                        "<html><head><meta name=\"qrichtext\" content=\"1\""
                        " />")) {
        QTextDocument doc;
        doc.setHtml(copy);
        copy = doc.toPlainText();
    }
    if (m_notes != copy) {
        m_notes = copy;
        emit notesChanged();
    }
}

QStringList ComplexItem::attachments() const
{
    return m_attachments;
}

QString ComplexItem::attachmentFileName(const QString &filename)
{
    QString result;
    if (isValid()) {
        QDir dir(directory());
        result = dir.absoluteFilePath(filename);
    }
    return result;
}

/**
 * @brief Attach a file to the item.
 *
 * This attaches the file (itentified by its @p filename) to the item.
 * Attaching means that the file is copied next to the item's main file.
 */
void ComplexItem::attachFile(const QString &filename)
{
    if (isValid()) {
        QFileInfo fi(filename);
        if (fi.exists() && fi.isFile() && fi.isReadable()) {
            QDir dir(directory());
            QString targetFileName = fi.fileName();
            int i = 0;
            while (dir.exists(targetFileName)) {
                ++i;
                targetFileName =
                        fi.baseName() + "-" + QString::number(i) + "." + fi.completeSuffix();
            }
            QFile::copy(filename, dir.absoluteFilePath(targetFileName));
            m_attachments.append(targetFileName);
            std::stable_sort(m_attachments.begin(), m_attachments.end());
            emit attachmentsChanged();
        }
    }
}

/**
 * @brief Remove an attachment.
 *
 * Remove the attachment identified by the attachment @p filename.
 */
void ComplexItem::detachFile(const QString &filename)
{
    if (isValid()) {
        auto absoluteFilename = attachmentFileName(filename);
        QFileInfo fi(absoluteFilename);
        if (fi.exists() && fi.isWritable()) {
            auto directory = this->directory();
            QDir dir(directory);
            if (!dir.remove(filename)) {
                qCWarning(log) << "Failed to remove" << filename << "from" << dir.path();
            }
            m_attachments.removeAll(filename);
            emit attachmentsChanged();
        }
    }
}

void ComplexItem::setupConnections()
{
    connect(this, &ComplexItem::dueToChanged, this, &ComplexItem::changed);
    connect(this, &ComplexItem::notesChanged, this, &ComplexItem::changed);
    connect(this, &ComplexItem::attachmentsChanged, this, &ComplexItem::changed);
}

void ComplexItem::setAttachments(const QStringList &attachments)
{
    if (m_attachments != attachments) {
        m_attachments = attachments;
        emit attachmentsChanged();
    }
}

QVariantMap ComplexItem::toMap() const
{
    auto result = Item::toMap();
    result["dueTo"] = m_dueTo;
    result["notes"] = m_notes;
    result["attachments"] = m_attachments;
    return result;
}

void ComplexItem::fromMap(QVariantMap map)
{
    Item::fromMap(map);
    setDueTo(map.value("dueTo", m_dueTo).toDateTime());
    setNotes(map.value("notes", m_notes).toString());
    setAttachments(map.value("attachments", m_attachments).toStringList());
}

bool ComplexItem::deleteItem()
{
    if (isValid()) {
        for (auto attachment : m_attachments) {
            auto path = attachmentFileName(attachment);
            QFile file(path);
            if (file.exists()) {
                if (!file.remove()) {
                    qCWarning(log)
                            << "Failed to remove attachment" << path << ":" << file.errorString();
                }
            }
        }
    }
    return Item::deleteItem();
}
