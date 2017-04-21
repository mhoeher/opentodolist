#include <QtSingleApplication>


int main(int argc, char **argv)
{
    QtSingleApplication app("MySingleInstance", argc, argv);

    if (app.sendMessage("Do I exist?"))
        return 0;

    app.initialize();

    return app.exec();
}
