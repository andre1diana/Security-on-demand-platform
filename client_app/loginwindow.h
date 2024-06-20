#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include"DeviceController.h"

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:

    void on_signup_btn_clicked();

    void on_login_btn_clicked();

    void on_close_btn_clicked();

private:
    Ui::LoginWindow *ui;
};

#endif // LOGINWINDOW_H
