#ifndef DEVICECONTROLLER_H
#define DEVICECONTROLLER_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>
#include <QFileDialog>

class DeviceController : public QObject
{
    Q_OBJECT
public:
    static DeviceController* instance();

    void connectToDevice(const QString &ip, int port);
    void disconnect();
    QAbstractSocket::SocketState state() const;
    bool isConnected() const;
    void sendData(const QString &message);
    void sendFile(const QString &filePath);
    QString receiveData();
    void sendInt(int value);
    int receiveInt();
    QString receiveFile();

    bool verifyLoginData(const QString& username, const QString& password);
    bool sendSignupSignal(const QString& username, const QString& password);

signals:
    void connected();
    void disconnected();
    void stateChanged(QAbstractSocket::SocketState);
    void errorOccurred(QAbstractSocket::SocketError);
    void dataReady(QByteArray);
    void dataReceived(const QString &string);

private slots:
    void socket_stateChanged(QAbstractSocket::SocketState state);
    //void onReadyRead();

private:
    explicit DeviceController(QObject *parent = nullptr);
    static DeviceController* _instance;

    QTcpSocket _socket;
    QString _ip;
    int _port;

    // QFile _file;
    // qint64 _fileSize = 0;
    // qint64 _bytesReceived = 0;
};

#endif // DEVICECONTROLLER_H
