#ifndef MIGRATOR_2_X_TO_3_X_H
#define MIGRATOR_2_X_TO_3_X_H

#include <QObject>

class Application;

class Migrator_2_x_to_3_x : public QObject
{
    Q_OBJECT
public:
    explicit Migrator_2_x_to_3_x(QObject *parent = 0);

    void run(Application *application);
signals:

public slots:
};

#endif // MIGRATOR_2_X_TO_3_X_H
