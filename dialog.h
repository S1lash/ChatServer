#ifndef DIALOG_H
#define DIALOG_H

#include <QDebug>
#include <QtGui>
#include <QtCore>
#include "server.h"
#include "ui_dialog.h"

namespace Ui {
    class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private:
    Ui::Dialog *ui;
    Server *serv;
    void addToLog(QString text, QColor color);

signals:
    void messageFromGui(QString message);

public slots:
    void onAddUserToGui(QString name);
    void onRemoveUserFromGui(QString name);
    void onMessageForGui(QString message, QString from);
    void onAddLogToGui(QString string, QColor color);

private slots:
    void on_send_clicked();
    void on_startStop_clicked();
};

#endif // DIALOG_H
