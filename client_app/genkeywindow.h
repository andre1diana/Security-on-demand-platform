#ifndef GENKEYWINDOW_H
#define GENKEYWINDOW_H

#include <QDialog>

namespace Ui {
class GenKeyWindow;
}

class GenKeyWindow : public QDialog
{
    Q_OBJECT

public:
    explicit GenKeyWindow(QWidget *parent = nullptr);
    ~GenKeyWindow();

    void setAction(int a);

private slots:
    void on_genKey_btn_clicked();

    void on_download_btn_clicked();

private:
    Ui::GenKeyWindow *ui;

    int action;
};

#endif // GENKEYWINDOW_H
