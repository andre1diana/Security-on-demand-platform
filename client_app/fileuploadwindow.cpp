#include "fileuploadwindow.h"
#include "ui_fileuploadwindow.h"
#include "DeviceController.h"
#include "Codes.h"
#include <qfiledialog.h>
#include <QDesktopServices>
#include <QString>
#include <windows.h>

FileUploadWindow::FileUploadWindow(QWidget *parent)
    :QDialog(parent)
    , ui(new Ui::FileUploadWindow)
{
    ui->setupUi(this);
}

FileUploadWindow::~FileUploadWindow()
{
    delete ui;
}

void FileUploadWindow::setAction(int a)
{
    action = a;
}


void FileUploadWindow::on_pushButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("All Files (*)"));
    ui->filePath->setText(filePath);
}

void FileUploadWindow::on_sendFile_btn_clicked()
{
    DeviceController* _controller = DeviceController::instance();
    _controller->sendInt(action);

    QString filePath = ui->filePath->text();
    if (!filePath.isEmpty()) {
        _controller->sendFile(filePath);

        int result = _controller->receiveInt();
        if(result == SEND_FILE_ERROR)
        {
            qDebug("Error while send the file to server");
            return;
        }
        else
        {
            QString outFile = _controller->receiveFile();

            if (QFile::exists(outFile)) {
                _controller->sendInt(OPERATION_SUCCESS);
                Sleep(200);
                QDesktopServices::openUrl(QUrl::fromLocalFile(outFile));
                this->close();
            } else {
                _controller->sendInt(OPERATION_FAILED);
                qWarning() << "Fișierul nu a fost găsit: " << outFile;
            }
        }
    }
}

