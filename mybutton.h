#ifndef MYBUTTON_H
#define MYBUTTON_H

#include <QPushButton>
#include "setdialog.h"
#include <QDialog>
#include <QGridLayout>
#include <QEvent>
#include <qtoolbutton.h>

class BtnDialog;

class MyButton : public QToolButton
{
    Q_OBJECT
public:
    explicit MyButton(QWidget *parent = nullptr);
    void loadFromItem(QTreeWidgetItem*item);

    ItemType btnType;
    QString name="";
    QString appPath="";
    QString para="";
    QString iconPath="";
    QTreeWidgetItem* p_item;

signals:
    void signalAppClicked(const QString &appPath,const QString &para);
private:
    BtnDialog *appListDlg;

public slots:

    virtual void  mousePressEvent(QMouseEvent *e);

};

class BtnDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BtnDialog(QWidget *parent = nullptr);
    void loadAppList(QTreeWidgetItem* groupItem);
    QList<MyButton*> btnList;
signals:
    void signalAppClicked(const QString &appPath,const QString &para);
private:
    QGridLayout *mainLayout;

    //对group按钮弹出的dialog处理，当离开时dialog消失
    void leaveEvent(QEvent * eve);
};
#endif // MYBUTTON_H
