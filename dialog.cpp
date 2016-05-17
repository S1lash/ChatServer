#include "dialog.h"

Dialog::Dialog(QWidget *parent) :QDialog(parent), ui(new Ui::Dialog)
{
    ui->setupUi(this);

    serv = new Server(this, this);

    connect(this, SIGNAL(messageFromGui(QString)), serv, SLOT(onMessageFromGui(QString)));
    connect(serv, SIGNAL(addLogToGui(QString,QColor)), this, SLOT(onAddLogToGui(QString,QColor)));


}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::onAddUserToGui(QString name)
{
    ui->usersLog->addItem(name);
    ui->msgLog->insertItem(0, QTime::currentTime().toString()+" "+name+" joined");
    ui->msgLog->item(0)->setTextColor(Qt::green);
}

void Dialog::onRemoveUserFromGui(QString name)
{
    for (int i = 0; i < ui->usersLog->count(); ++i)
        if (ui->usersLog->item(i)->text() == name)
        {
            ui->usersLog->takeItem(i);
            ui->msgLog->insertItem(0, QTime::currentTime().toString()+" "+name+" left");
            ui->msgLog->item(0)->setTextColor(Qt::green);
            break;
        }
}

void Dialog::onMessageForGui(QString message, QString from)
{
        ui->msgLog->insertItem(0, QTime::currentTime().toString()+" message from "+from+": "+message);
}

void Dialog::onAddLogToGui(QString string, QColor color)
{
    addToLog(string, color);
}

void Dialog::on_send_clicked()
{
    if (ui->usersLog->count() == 0)
    {
        QMessageBox::information(this, "", "No clients connected");
        return;
    }

    emit messageFromGui(ui->msgEdit->document()->toPlainText());
    addToLog(" Sended server message" + (QString)": " + ui->msgEdit->document()->toPlainText(), Qt::black);
    ui->msgEdit->clear();
}


void Dialog::addToLog(QString text, QColor color)
{
    ui->msgLog->insertItem(0, QTime::currentTime().toString()+text);
    ui->msgLog->item(0)->setTextColor(color);
}


void Dialog::on_startStop_clicked()
{
    if (serv->isListening())
    {
        QString adr = serv->serverAddress().toString();
        QString port = QString::number(serv->serverPort());
        serv->StopServer();
        addToLog(" server stopped at "+ adr +":"+port, Qt::green);

        ui->startStop->setText("Start server");
    } else
    {
        QHostAddress addr;
        if (!addr.setAddress(ui->host->text()))
        {
            addToLog(" invalid address "+ui->host->text(), Qt::red);
            return;
        }
        if (serv->doStartServer(addr, ui->port->text().toInt()))
        {
            addToLog(" server strated at "+ui->host->text()+":"+ui->port->text(), Qt::green);
            ui->startStop->setText("Stop server");
        }
        else
        {
            addToLog(" server not strated at "+ui->host->text()+":"+ui->port->text(), Qt::red);
        }
    }
}
