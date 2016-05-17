#include "client.h"

const QString Client::nameUnknown = QString(".Unknown");

Client::Client(int desc, Server *serv, QObject *parent) :QObject(parent)
{
    this->serv = serv;
    isAutched = false;
    name = nameUnknown;
    blockSize = 0;
    this->socket = new QTcpSocket(this);
    this->socket->setSocketDescriptor(desc);



    connect(socket, SIGNAL(connected()), this, SLOT(Connect()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(Disconnect()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(Ready()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(Error(QAbstractSocket::SocketError)));

    qDebug() << "Client connected" << desc;
}

Client::~Client()
{

}

void Client::Connect()
{

}

void Client::Disconnect()
{
    qDebug() << "Client disconnected";

    if (isAutched)
    {

        emit removeUserFromGui(name);
        serv->doSendToAllUserLeft(name);
        emit removeUser(this);
    }
    deleteLater();
}

void Client::Error(QAbstractSocket::SocketError socketError) const
{
    QWidget w;
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(&w, "Error", "The host was not found");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(&w, "Error", "The connection was refused by the peer.");
        break;
    default:
        QMessageBox::information(&w, "Error", "The following error occurred: "+socket->errorString());
    }
}

void Client::Ready()
{
    QDataStream in(socket);
    if (blockSize == 0) {
        if (socket->bytesAvailable() < (int)sizeof(quint16))
            return;
        in >> blockSize;
        qDebug() << "blockSize now " << blockSize;
    }
    if (socket->bytesAvailable() < blockSize)
        return;
    else
        blockSize = 0;

    quint8 command;
    in >> command;
    qDebug() << "Received command " << command;

    if (!isAutched && command != commAutchReq)
        return;

    switch(command)
    {
        case commAutchReq:
        {
            QString name;
            in >> name;
            if (!serv->isNameValid(name))
            {
                doSendCommand(commErrNameInvalid);
                return;
            }

            if (serv->isNameUsed(name))
            {
                doSendCommand(commErrNameUsed);
                return;
            }
            isAutched = true;
            this->name = name;
            emit addUserToGui(name);
            doSendSuccesAuth();
            serv->doSendToAllUserJoin(name);
        }
        break;

        case commMessage:
        {
            QString message;
            in >> message;
            serv->doSendMessage(message, name);
            emit messageForGui(message, name);
        }
        break;

    }

}

void Client::doSendCommand(quint8 comm) const
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0;
    out << comm;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    socket->write(block);
    qDebug() << "Send to" << name << "command:" << comm;
}

void Client::doSendSuccesAuth() const
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0;
    out << commAutchSuccess;
    QStringList l = serv->getUsersOnline();
    QString s;
    for (int i = 0; i < l.length(); ++i)
        if (l.at(i) != name)
            s += l.at(i)+(QString)",";
    s.remove(s.length()-1, 1);
    if (s == ""){
        s=" ";
    }
    out << s;
    out << serv->GetHistoryMessage();
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    socket->write(block);
    qDebug() << "Send user list to" << name << ":" << s;
}

