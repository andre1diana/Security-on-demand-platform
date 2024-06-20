#ifndef FILEUPLOADWINDOW_H
#define FILEUPLOADWINDOW_H

#include <QDialog>

namespace Ui {
class FileUploadWindow;
}

class FileUploadWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FileUploadWindow(QWidget *parent = nullptr);
    ~FileUploadWindow();
    void setAction(int a);

private slots:
    void on_pushButton_clicked();

    void on_sendFile_btn_clicked();

private:
    Ui::FileUploadWindow *ui;

    int action;
};

#endif // FILEUPLOADWINDOW_H
