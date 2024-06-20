#ifndef SIGNUPWINDOW_H
#define SIGNUPWINDOW_H

#include <QDialog>

namespace Ui {
class SignupWindow;
}

class SignupWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SignupWindow(QWidget *parent = nullptr);
    ~SignupWindow();

private slots:
    void on_pushButton_2_clicked();

    void on_create_btn_clicked();

    void on_username_line_textEdited(const QString &arg1);

    void on_password_line_textEdited(const QString &arg1);

    void on_password_line_2_textEdited(const QString &arg1);

private:
    Ui::SignupWindow *ui;
};

#endif // SIGNUPWINDOW_H
