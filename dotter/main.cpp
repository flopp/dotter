#include <QtCore/QObject>
#include "app.h"
#include "mainwindow.h"

int main(int argc, char** argv)
{
    App app{argc, argv};
    QObject::connect(&app, &QCoreApplication::aboutToQuit, &app, &App::quitting);

    MainWindow w;
    w.show();

    if (argc == 2)
    {
        w.openFile(argv[1]);
    }

    return app.exec();
}
