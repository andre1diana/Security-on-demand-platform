//#include "mainwindow.h"
#include"loginwindow.h"
#include "DeviceController.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#define IP "127.0.0.1"
#define port 12345

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DeviceController* _controller = DeviceController::instance();
    _controller->connectToDevice(IP, port);
    LoginWindow loginWindow;
    loginWindow.show();

    return app.exec();
}

#include "main.moc"
