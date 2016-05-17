#ifndef SERVER_H
#define SERVER_H


#include <QTcpServer>
#include <QDebug>
#include "client.h"
class Client;

class Server : public QTcpServer
{
    Q_OBJECT

public:
    explicit Server(QWidget *widget = 0, QObject *parent = 0);
    bool doStartServer(QHostAddress addr, qint16 port);
    void doSendToAllUserJoin(QString name);
    void doSendToAllUserLeft(QString name);
    void doSendMessage(QString message, QString fromUsername);
    void doSendServerMessage(QString message);
    QStringList getUsersOnline() const;
    bool isNameValid(QString name) const;
    bool isNameUsed(QString name) const;
    QHostAddress GetAddress() {return servAdr;}
    void StopServer();
    QString GetHistoryMessage();
    qint16 GetPort() {return servPort;}

signals:
    void addLogToGui(QString string, QColor color = Qt::black);

public slots:
    void onMessageFromGui(QString message);
    void onRemoveUser(Client *client);

protected:
    void incomingConnection(qintptr handle);

private:
    QList<Client *> clients;
    void DisconnectAll();
    QStringList history;
    QWidget *widget;
    QHostAddress servAdr;
    qint16 servPort;

};

#endif // SERVER_H
