#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStyle>
#include <QHostAddress>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_aes_cbc_btn_clicked();

    void on_aes_ofb_btn_clicked();

    void on__3des_btn_clicked();

    void on_sha1_btn_clicked();

    void on_sha2_btn_clicked();

    void on_sha3_btn_clicked();

    void on_mac_btn_clicked();

    void on_decrypt_btn_clicked();

    void on_rsa_btn_clicked();

    void on_dsa_btn_clicked();

    void on_certificate_btn_clicked();

    void on_close_btn_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
