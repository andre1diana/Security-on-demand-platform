#include "genkeywindow.h"
#include "ui_genkeywindow.h"
#include "DeviceController.h"
#include "Codes.h"


GenKeyWindow::GenKeyWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GenKeyWindow)
{
    ui->setupUi(this);
}

GenKeyWindow::~GenKeyWindow()
{
    delete ui;
}

void GenKeyWindow::setAction(int a)
{
    action = a;
}

void GenKeyWindow::on_genKey_btn_clicked()
{
    DeviceController::instance()->sendInt(action);
    if(!ui->name->text().isEmpty())
    {
        DeviceController* _controller = DeviceController::instance();
        _controller->sendData(ui->name->text());
        QString str = ui->size->currentText();
        if(str == "1024")
            _controller->sendInt(1024);
        else if(str == "2048")
            _controller->sendInt(2048);
        else
            _controller->sendInt(3096);
        if(_controller->receiveInt() == OPERATION_SUCCESS)
        {
            ui->message->setText("Succes!");
        }
    }
    else
        return;
}


void GenKeyWindow::on_download_btn_clicked()
{
    DeviceController* _controller = DeviceController::instance();
    _controller->sendInt(DOWNLOAD_KEY);

    _controller->sendData(ui->name->text());
    QString filename = ui->name->text() + ".pem";

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly))
    {
        QString privKey = _controller->receiveData();
        file.write(privKey.toUtf8());
        file.write("\n");
        QString pubKey = _controller->receiveData();
        file.write(pubKey.toUtf8());
    }
    file.close();
    ui->download_msg->setText("Download succes!");
}

