#include <QApplication>
#include "STAppController.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Strip Tool");

    STAppController *appController = new STAppController();

    int returnVal = -1;

    if (appController->startup()) {
        returnVal = app.exec();
    }

    if (appController->isRunning()) {
        appController->shutdown();
    }

    delete appController;

    return returnVal;
}
