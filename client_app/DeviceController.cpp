#include "DeviceController.h"
#include "Codes.h"
#include "DeviceController.h"

#include <QCoreApplication>
#include <QFile>
#include <QFileDialog>
//#include <cstdint>


// Initialize the static member
DeviceController* DeviceController::_instance = nullptr;

DeviceController* DeviceController::instance()
{
    if (_instance == nullptr)
    {
        _instance = new DeviceController();
    }
    return _instance;
}

DeviceController::DeviceController(QObject *parent)
    : QObject(parent)
{
    connect(&_socket, &QTcpSocket::connected, this, &DeviceController::connected);
    connect(&_socket, &QTcpSocket::disconnected, this, &DeviceController::disconnected);
    connect(&_socket, &QTcpSocket::errorOccurred, this, &DeviceController::errorOccurred);
    connect(&_socket, &QTcpSocket::stateChanged, this, &DeviceController::socket_stateChanged);
}

void DeviceController::connectToDevice(const QString &ip, int port)
{
    if (_socket.isOpen()) {
        if (ip == _ip && port == _port) {
            return;
        }
        _socket.close();
    }
    _ip = ip;
    _port = port;
    _socket.connectToHost(_ip, _port);
}

void DeviceController::disconnect()
{
    sendInt(CLOSE_SIG);
    _socket.close();
}

QAbstractSocket::SocketState DeviceController::state() const
{
    return _socket.state();
}

bool DeviceController::isConnected() const
{
    return _socket.state() == QAbstractSocket::ConnectedState;
}

void DeviceController::sendData(const QString &message)
{
    sendInt(message.size());
    if (isConnected()) {
        _socket.write(message.toUtf8());
    }
    qDebug() << "Data sent successfuly!";
}

void DeviceController::sendFile(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    QString fileName = fileInfo.fileName();
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {

        //send the name of the file
        sendData(fileName);
        // send the size of file
        int fileSize = file.size();
        sendInt(fileSize);

        int bytesSend = 0;
        QByteArray data;
        while(!file.atEnd()) {
            data = file.read(512);
            if(data.isEmpty())
            {
                qWarning() << "No data.";
                file.close();
                return;
            }
            _socket.write(data);
            bytesSend += data.size();
            qInfo() << "Send" << bytesSend << "bytes out of" << fileSize;
        }
        file.close();
    } else {
        qWarning() << "Couldn't open file to read:" << file.errorString();
    }
}

QString DeviceController::receiveData()
{
    if (isConnected())
    {
        int sizeOfData = receiveInt();
        if (!_socket.waitForReadyRead(5000)) { // Wait for up to 3 seconds for the data
            if (_socket.bytesAvailable() <= 0) {
                qWarning() << "Timeout or error while waiting for data";
                return QString();
            }
        }
        if (_socket.bytesAvailable() >= sizeOfData)
        {
            qDebug() << "receiveData() - Available Bytes: " << _socket.bytesAvailable();
            QByteArray data = _socket.read(sizeOfData);
            qDebug() << "receiveData() - Available Bytes (after read): " << _socket.bytesAvailable();
            emit dataReceived(data);
            if(data.size() != sizeOfData)
            {
                qDebug() << "Data not received completely!";
            }
            return QString::fromUtf8(data);
        } else {
            qDebug() << "Not enough bytes available!";
        }
        qDebug() << "Data received successfuly";
    }
    return QString();
}

void DeviceController::sendInt(int value) {
    QByteArray data = QByteArray::fromRawData(reinterpret_cast<const char*>(&value), sizeof(value));
    _socket.write(data);
}

int DeviceController::receiveInt() {
    QByteArray data;
    while (data.size() < sizeof(int)) {
        if (!_socket.waitForReadyRead(5000)) {
            if (_socket.bytesAvailable() <= 0) {
                qWarning() << "Timeout or error while waiting for int";
                return -1;
            }
        }
        //if (_socket.bytesAvailable() > 0) {
            qDebug() << "receiveInt() - Available Bytes: " << _socket.bytesAvailable();
            data.append(_socket.read(sizeof(int)));
            qDebug() << "receiveInt() - Available Bytes (after read): " << _socket.bytesAvailable();
        //}
    }

    int value;
    memcpy(&value, data.constData(), sizeof(value));
    data.remove(0, sizeof(value));
    return value;
}

QString DeviceController::receiveFile()
{
    QString savePath = receiveData();

    qDebug() << "The path to file is : " << savePath;
    if (QFile::exists(savePath)) {
        if (QFile::remove(savePath)) {
            qDebug() << "File deleted successfuly!";
        } else {
            qDebug() << "File could not be deleted!";
        }
    }

    int bytesReceived = 0;
    if (isConnected()) {
        QFile file(savePath);
        if (file.open(QIODevice::WriteOnly)) {
            int fileSize = receiveInt(); //size of file (content)
            QByteArray fileData;
            while (bytesReceived < fileSize) {
                if (!_socket.waitForReadyRead(5000)) {
                    if (_socket.bytesAvailable() <= 0) {
                        qWarning() << "Timeout waiting for file data.";
                        file.close();
                        return NULL;
                    }
                }


                //if (_socket.bytesAvailable() > 0) {
                    fileData.append(_socket.read(fileSize - bytesReceived));
                    bytesReceived += fileData.size();
                    qInfo() << "Received" << bytesReceived << "bytes out of" << fileSize;
                //}
                // else {
                //     qWarning() << "No data received.";
                //     // file.close();
                //     // return NULL;
                // }

            }
            file.write(fileData);
        } else {
            qWarning() << "File could not be open for read : " << file.errorString();
        }
    }
    return savePath;
}

void DeviceController::socket_stateChanged(QAbstractSocket::SocketState state)
{
    if (state == QAbstractSocket::UnconnectedState) {
        _socket.close();
        QCoreApplication::quit();
    }
    emit stateChanged(state);
}

bool DeviceController::verifyLoginData(const QString &username, const QString &password)
{

    sendInt(LOGIN_VERIFY);

    sendData(username);
    sendData(password);

    int result = receiveInt();
    if(result == LOGIN_ACCEPT)
        return true;

    return false;
}

bool DeviceController::sendSignupSignal(const QString &username, const QString &password)
{
    sendInt(SIGNUP_SIG);

    sendData(username);
    sendData(password);

    int result = receiveInt();
    if(result == SIGNUP_ACCEPT)
        return true;

    return false;
}
