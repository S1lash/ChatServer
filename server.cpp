#include "server.h"

Server::Server(QWidget *widget, QObject *parent) :QTcpServer(parent)
{
    this->widget = widget;
}

bool Server::doStartServer(QHostAddress addr, qint16 port)
{
    if (!listen(addr, port))
    {
        qDebug() << "Server not started at" << addr << ":" << port;
        return false;
    }
    qDebug() << "Server started at" << addr << ":" << port;
    return true;
}

void Server::doSendToAllUserJoin(QString name)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0 << Client::commUserJoin << name;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    for (int i = 0; i < clients.length(); ++i)
        if (clients.at(i)->getName() != name && clients.at(i)->getAutched())
            clients.at(i)->socket->write(block);
}

void Server::doSendToAllUserLeft(QString name)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0 << Client::commUserLeft << name;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    for (int i = 0; i < clients.length(); ++i)
        if (clients.at(i)->getName() != name && clients.at(i)->getAutched())
            clients.at(i)->socket->write(block);
}

void Server::doSendMessage(QString message, QString fromUsername)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0 << Client::commMessage << fromUsername << message;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    for (int i = 0; i < clients.length(); ++i)
        if (clients.at(i)->getAutched()){
            clients.at(i)->socket->write(block);
        }
    QString tempStr = (QString)"[" + fromUsername + (QString)"]: " +message;
    this->history << tempStr;
}

void Server::doSendServerMessage(QString message)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0 << Client::commServerMessage << message;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    for (int i = 0; i < clients.length(); ++i)
        if (clients.at(i)->getAutched()){
            clients.at(i)->socket->write(block);
        }
    QString tempStr = (QString)"[Server message]: " +message;
    this->history << tempStr;
}


QStringList Server::getUsersOnline() const
{
    QStringList l;
    foreach (Client * c, clients)
        if (c->getAutched())
            l << c->getName();
    return l;
}

bool Server::isNameValid(QString name) const
{
    if (name.length() > 20 || name.length() < 5)
        return false;
    QRegExp r("[A-Za-z0-9]+");
    return r.exactMatch(name);
}

bool Server::isNameUsed(QString name) const
{
    for (int i = 0; i < clients.length(); ++i)
        if (clients.at(i)->getName() == name)
            return true;
    return false;
}

void Server::incomingConnection(qintptr handle)
{
    Client *client = new Client(handle, this, this);
    if (widget != 0)
    {
        connect(client, SIGNAL(addUserToGui(QString)), widget, SLOT(onAddUserToGui(QString)));
        connect(client, SIGNAL(removeUserFromGui(QString)), widget, SLOT(onRemoveUserFromGui(QString)));
        connect(client, SIGNAL(messageForGui(QString,QString)), widget, SLOT(onMessageForGui(QString,QString)));
    }
    connect(client, SIGNAL(removeUser(Client*)), this, SLOT(onRemoveUser(Client*)));
    clients.append(client);
}

void Server::onRemoveUser(Client *client)
{
    clients.removeAt(clients.indexOf(client));
}

void Server::onMessageFromGui(QString message)
{
        doSendServerMessage(message);
}

void Server::DisconnectAll()
{
    for (int i = clients.size()-1; i >= 0; --i) {
           clients[i]->Disconnect();
    }
}

void Server::StopServer(){
    this->close();
    DisconnectAll();
    QStringList temp;
    this->history = temp;
}

QString Server::GetHistoryMessage()
{
    QString str = "";
    for (int i = 0; i < this->history.size(); ++i) {
       str+= history.at(i);
       if (i != history.size()-1){
           str+=(QString)"!_next_msg_!";
       }
    }
    if (str =="")
        str = " ";
    return str;
}
