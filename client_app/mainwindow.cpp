#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DeviceController.h"
#include "fileuploadwindow.h"
#include "genkeywindow.h"
#include "gencertificatewindow.h"
#include "Codes.h"

#include <QMetaEnum>
#include <QFileDialog>
#include <qmessagebox.h>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("SafeZone");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_aes_cbc_btn_clicked()
{
    //DeviceController::instance()->sendInt(AES_CBC_ENC);
    FileUploadWindow* fileWindow = new FileUploadWindow;
    fileWindow->setAction(AES_CBC_ENC);
    fileWindow->setModal(false);
    fileWindow->exec();
    delete fileWindow;
}

void MainWindow::on_aes_ofb_btn_clicked()
{
    //DeviceController::instance()->sendInt(AES_OFB_ENC);
    FileUploadWindow* fileWindow = new FileUploadWindow;
    fileWindow->setAction(AES_OFB_ENC);
    fileWindow->setModal(false);
    fileWindow->exec();
    delete fileWindow;
}


void MainWindow::on__3des_btn_clicked()
{
    //DeviceController::instance()->sendInt(_3DES_ENC);
    FileUploadWindow* fileWindow = new FileUploadWindow;
    fileWindow->setAction(_3DES_ENC);
    fileWindow->setModal(false);
    fileWindow->exec();
    delete fileWindow;
}


void MainWindow::on_sha1_btn_clicked()
{
    //DeviceController::instance()->sendInt(SHA1_OPT);
    FileUploadWindow* fileWindow = new FileUploadWindow;
    fileWindow->setAction(SHA1_OPT);
    fileWindow->setModal(false);
    fileWindow->exec();
    delete fileWindow;
}


void MainWindow::on_sha2_btn_clicked()
{
    //DeviceController::instance()->sendInt(SHA2_OPT);
    FileUploadWindow* fileWindow = new FileUploadWindow;
    fileWindow->setAction(SHA2_OPT);
    fileWindow->setModal(false);
    fileWindow->exec();
    delete fileWindow;
}


void MainWindow::on_sha3_btn_clicked()
{
    //DeviceController::instance()->sendInt(SHA3_OPT);
    FileUploadWindow* fileWindow = new FileUploadWindow;
    fileWindow->setAction(SHA3_OPT);
    fileWindow->setModal(false);
    fileWindow->exec();
    delete fileWindow;
}


void MainWindow::on_mac_btn_clicked()
{
    //DeviceController::instance()->sendInt(MAC_OPT);
    FileUploadWindow* fileWindow = new FileUploadWindow;
    fileWindow->setAction(MAC_OPT);
    fileWindow->setModal(false);
    fileWindow->exec();
    delete fileWindow;
}


void MainWindow::on_decrypt_btn_clicked()
{
    //DeviceController::instance()->sendInt(DECRYPT);
    FileUploadWindow* fileWindow = new FileUploadWindow;
    fileWindow->setAction(DECRYPT);
    fileWindow->setModal(false);
    fileWindow->exec();
    delete fileWindow;
}


void MainWindow::on_rsa_btn_clicked()
{
    //DeviceController::instance()->sendInt(RSA_OPT);
    GenKeyWindow* keyWindow = new GenKeyWindow;
    keyWindow->setAction(RSA_OPT);
    keyWindow->setModal(false);
    keyWindow->exec();
    delete keyWindow;
}


void MainWindow::on_dsa_btn_clicked()
{
    //DeviceController::instance()->sendInt(DSA_OPT);
    GenKeyWindow* keyWindow = new GenKeyWindow;
    keyWindow->setAction(DSA_OPT);

    keyWindow->setModal(false);
    keyWindow->exec();
    delete keyWindow;
}


void MainWindow::on_certificate_btn_clicked()
{
    //DeviceController::instance()->sendInt(CERTIFICATE);
    GenCertificateWindow* certWindow = new GenCertificateWindow;
    //certWindow->setAction(CERTIFICATE);

    certWindow->setModal(false);
    certWindow->exec();
    delete certWindow;
}


void MainWindow::on_close_btn_clicked()
{
    DeviceController::instance()->sendInt(CLOSE_SIG);
    DeviceController::instance()->disconnect();
    QCloseEvent event;
    event.accept();
}

