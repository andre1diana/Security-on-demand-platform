#include "signupwindow.h"
#include "ui_signupwindow.h"
#include"DeviceController.h"

SignupWindow::SignupWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SignupWindow)
{
    ui->setupUi(this);
    setWindowTitle("SafeZone");
}

SignupWindow::~SignupWindow()
{
    delete ui;
}

void SignupWindow::on_pushButton_2_clicked()
{
    this->close();
}

bool stringConstraints(QString str)
{
    if(str.contains("*") ||
        str.contains("!") ||
        str.contains("#") ||
        str.contains("$") ||
        str.contains("%") ||
        str.contains("^") ||
        str.contains("&") ||
        str.contains("(") ||
        str.contains(")") ||
        str.contains("{") ||
        str.contains("}") ||
        str.contains("[") ||
        str.contains("]") ||
        str.contains("'") ||
        str.contains("|") ||
        str.contains(";") ||
        str.contains(":") ||
        str.contains("/") ||
        str.contains("?") ||
        str.contains(">") ||
        str.contains("<") ||
        str.contains(","))
        return true;
    return false;
}

void SignupWindow::on_create_btn_clicked()
{
    if(ui->password_line->text() != ui->password_line_2->text())
    {
        ui->succes_message->setText("Password repeted incorrectly! Please try again!");
        ui->password_line_2->clear();
        return;
    }
    else if(stringConstraints(ui->username_line->text()))
    {
        ui->succes_message->setText("Username can not contain these characters []{}!#$%^&*()<>?':;|~`");
        ui->username_line->clear();
        return;
    }
    else
    {
        if(DeviceController::instance()->sendSignupSignal(ui->username_line->text(), ui->password_line->text()))
        {
            ui->succes_message->setText("Succes! Return to previous page to Log in!");
        }
        else
        {
            ui->succes_message->setText("This username is already used! Please try another one!");
        }
    }

}
void SignupWindow::on_username_line_textEdited(const QString &arg1)
{
    if(ui->username_line->isModified())
    {
        ui->succes_message->clear();
    }
}


void SignupWindow::on_password_line_textEdited(const QString &arg1)
{
    if(ui->password_line->isModified())
    {
        ui->succes_message->clear();
    }
}


void SignupWindow::on_password_line_2_textEdited(const QString &arg1)
{
    if(ui->password_line_2->isModified())
    {
        ui->succes_message->clear();
    }
}

