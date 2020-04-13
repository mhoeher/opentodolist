#ifndef OPENTODOLIST_TRANSLATIONS_H
#define OPENTODOLIST_TRANSLATIONS_H

#include <QObject>
#include <QString>
#include <QStringList>

class QTranslator;
class QQmlEngine;

namespace OpenTodoList {

class Translations : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
public:
    explicit Translations(QQmlEngine *engine, QObject *parent = nullptr);

    QString language() const;
    void setLanguage(const QString &language);

    Q_INVOKABLE QStringList allLanguages() const;

signals:

    void languageChanged();

public slots:

private:
    QQmlEngine *m_engine;
    QTranslator *m_translator;
    QString m_language;

    void load();
    void save();
    void apply();
};

} // namespace OpenTodoList

#endif // OPENTODOLIST_TRANSLATIONS_H
