#ifndef GENCERTIFICATEWINDOW_H
#define GENCERTIFICATEWINDOW_H

#include <QDialog>

namespace Ui {
class GenCertificateWindow;
}

class GenCertificateWindow : public QDialog
{
    Q_OBJECT

public:
    explicit GenCertificateWindow(QWidget *parent = nullptr);
    ~GenCertificateWindow();

private slots:
    void on_create_btn_clicked();

private:
    Ui::GenCertificateWindow *ui;
};

#endif // GENCERTIFICATEWINDOW_H
