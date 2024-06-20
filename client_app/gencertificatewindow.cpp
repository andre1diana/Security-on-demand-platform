#include "gencertificatewindow.h"
#include "ui_gencertificatewindow.h"
#include "DeviceController.h"
#include "Codes.h"
#include <QDesktopServices>
#include <Windows.h>

GenCertificateWindow::GenCertificateWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GenCertificateWindow)
{
    ui->setupUi(this);
}

GenCertificateWindow::~GenCertificateWindow()
{
    delete ui;
}

void GenCertificateWindow::on_create_btn_clicked()
{
    DeviceController* _controller = DeviceController::instance();
    _controller->sendInt(CERTIFICATE);

    if (!ui->name->text().isEmpty() && !ui->common_name->text().isEmpty())
    {
        //file to save
        _controller->sendData(ui->name->text());
        //key
        _controller->sendData(ui->keyName_line->text());
        //country
        _controller->sendData(ui->country->text());
        //organization
        _controller->sendData(ui->organization->text());
        //common name
        _controller->sendData(ui->common_name->text());
        //validity
        _controller->sendInt(ui->validity->value());

        int result = _controller->receiveInt();
        if(result == OPERATION_SUCCESS)
        {
            QString filename = _controller->receiveFile();
            if (QFile::exists(filename)) {
                _controller->sendInt(OPERATION_SUCCESS);
                Sleep(200);
                QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
                this->close();
            } else {
                _controller->sendInt(OPERATION_FAILED);
                qWarning() << "Fișierul nu a fost găsit: " << filename;
            }
        }
        else
        {
            qDebug() << "The certificate generation failed!";
            this->close();
            return;
        }
    }
}
