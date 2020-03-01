#include "notepage.h"

NotePage::NotePage(QObject *parent) : ComplexItem(QString(), parent) {}

NotePage::NotePage(const QString &filename, QObject *parent)
    : ComplexItem(filename, parent), m_noteUid()
{
    connect(this, &NotePage::noteUidChanged, this, &NotePage::changed);
}

NotePage::NotePage(const QDir &dir, QObject *parent) : ComplexItem(dir, parent), m_noteUid()
{
    connect(this, &NotePage::noteUidChanged, this, &NotePage::changed);
}

NotePage::~NotePage() {}

QUuid NotePage::noteUid() const
{
    return m_noteUid;
}

void NotePage::setNoteUid(const QUuid &noteUid)
{
    if (m_noteUid != noteUid) {
        m_noteUid = noteUid;
        emit noteUidChanged();
    }
}

QUuid NotePage::parentId() const
{
    return m_noteUid;
}

QVariantMap NotePage::toMap() const
{
    auto result = ComplexItem::toMap();
    result["noteUid"] = m_noteUid;
    return result;
}

void NotePage::fromMap(QVariantMap map)
{
    ComplexItem::fromMap(map);
    setNoteUid(map.value("noteUid", m_noteUid).toUuid());
}
