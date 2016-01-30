#include <QApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  
  QCoreApplication::setApplicationName("OpenTodoList");
  QCoreApplication::setApplicationVersion(VERSION);
  QCoreApplication::setOrganizationDomain("www.rpdev.net");
  QCoreApplication::setOrganizationName("RPdev");
  
  QQmlApplicationEngine engine;
  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
  
  return app.exec();
}
