#ifndef CLIENT_H
#define CLIENT_H


#include <QObject>
#include <QDebug>
#include <QThreadPool>
#include <QtGui>
#include <QRegExp>
#include <QWidget>
#include <QMessageBox>
#include <QTcpSocket>
#include "ui_dialog.h"
#include "server.h"
class Server;

class Client : public QObject
{
    friend class Server;
    Q_OBJECT

public:
    static const QString nameUnknown;
    static const quint8 commAutchReq = 1;
    static const quint8 commUserJoin = 2;
    static const quint8 commUserLeft = 3;
    static const quint8 commMessage = 4;
    static const quint8 commServerMessage = 5;
    static const quint8 commAutchSuccess = 6;
    static const quint8 commErrNameInvalid = 201;
    static const quint8 commErrNameUsed = 202;

    explicit Client(int desc, Server *serv, QObject *parent = 0);
    ~Client();

    void setName(QString name) {name = name;}
    QString getName() const {return name;}
    bool getAutched() const {return isAutched;}
    void doSendCommand(quint8 comm) const;
    void doSendSuccesAuth() const;

signals:
    void addUserToGui(QString name);
    void removeUserFromGui(QString name);
    void removeUser(Client *client);
    void messageForGui(QString message, QString from);

private slots:
    void Connect();
    void Disconnect();
    void Ready();
    void Error(QAbstractSocket::SocketError socketError) const;

private:
    QTcpSocket *socket;
    Server *serv;
    quint16 blockSize;
    QString name;
    bool isAutched;

};

#endif // CLIENT_H
