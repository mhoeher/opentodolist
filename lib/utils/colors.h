#ifndef OPENTODOLIST_COLORS_H
#define OPENTODOLIST_COLORS_H

#include <QObject>
#include <QColor>

class Color
{
    Q_GADGET

    Q_PROPERTY(QColor color READ color CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(bool valid READ isValid CONSTANT)

public:
    Color();
    Color(const QColor& color, const QString& name);

    bool isValid() const;
    const QColor& color() const;
    const QString& name() const;

private:
    QColor m_color;
    QString m_name;
};

class Colors : public QObject
{
    Q_OBJECT
public:
    explicit Colors(QObject* parent = nullptr);

    Q_INVOKABLE QList<Color> loadRALColors() const;
    Q_INVOKABLE QList<Color> loadQtColors() const;
    Q_INVOKABLE QList<Color> loadColors() const;
};

#endif // OPENTODOLIST_COLORS_H
