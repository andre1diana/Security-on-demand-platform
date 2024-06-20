#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "signupwindow.h"
#include "mainwindow.h"
#include "Codes.h"
#include "DeviceController.h"
#include <QCloseEvent>

LoginWindow::LoginWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    ui->signup_btn->setFixedSize(70,25);
    setWindowTitle("SafeZone");
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_signup_btn_clicked()
{
    hide();
    SignupWindow sig;
    sig.exec();
    if(!sig.isActiveWindow())
    {
        show();
    }
}

void LoginWindow::on_login_btn_clicked()
{
    if(!DeviceController::instance()->verifyLoginData(ui->username_line->text(), ui->password_line->text()))
    {
        ui->message->setText("The account with this username does not exist! Click on 'Sign up' to create an account!");
        ui->username_line->clear();
        ui->password_line->clear();
        return;
    }

    hide();
    MainWindow *mainWindow = new MainWindow();
    mainWindow->show();
    close();
}

void LoginWindow::on_close_btn_clicked()
{
    DeviceController::instance()->sendInt(CLOSE_SIG);
    DeviceController::instance()->disconnect();
    QCloseEvent event;
    event.accept();
}

