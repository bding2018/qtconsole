#include "mybutton.h"
#include <QMessageBox>
#include <QApplication>

MyButton::MyButton(QWidget *parent) : QToolButton(parent)
{
    appListDlg = nullptr;
    setFixedSize(60,60);
}

void MyButton::loadFromItem(QTreeWidgetItem *item)
{
    p_item = item->clone();
    btnType = item->data(0,ItemRole::itemType).value<ItemType>();
    if(btnType == ItemType::groupType)
    {
        name = item->text(0);
        iconPath = item->data(0,ItemRole::iconPath).toString();
    }
    else if(btnType == ItemType::appType)
    {
        name = item->text(0);
        iconPath = item->data(0,ItemRole::iconPath).toString();
        para = item->data(0,ItemRole::appPara).toString();
        appPath = item->data(0,ItemRole::appPath).toString();
    }
    QString style = QString("QPushButton{border-image:url(%1);}").arg(iconPath);
    setStyleSheet(this->styleSheet() + style);
    setToolTip(name);
}

void MyButton::mousePressEvent(QMouseEvent *e)
{
    if(btnType == ItemType::appType)
    {
        emit signalAppClicked(appPath,para);
    }
    else if(btnType == ItemType::groupType)
    {
        if(appListDlg==nullptr)
        {
            appListDlg = new BtnDialog;
            appListDlg->loadAppList(p_item);
            connect(appListDlg,&BtnDialog::signalAppClicked,this,&MyButton::signalAppClicked);
        }
        QPoint destPos = this->pos()+QPoint((width()-appListDlg->width())/2,height());
        appListDlg->move(mapToGlobal(mapFromParent(destPos)));
        appListDlg->exec();
    }
    QToolButton::mousePressEvent(e);
}

BtnDialog::BtnDialog(QWidget *parent) : QDialog(parent)
{
    mainLayout = new QGridLayout(this);
    this->setLayout(mainLayout);
    setWindowFlags(Qt::FramelessWindowHint);
    setFixedWidth(220);
}

void BtnDialog::loadAppList(QTreeWidgetItem *groupItem)
{
    if(groupItem->data(0,ItemRole::itemType).value<ItemType>()== ItemType::groupType)
    {
        int num = groupItem->childCount();
        for(int i = 0 ; i< num ;i++)
        {
            MyButton *btn = new MyButton(this);
            btn->loadFromItem(groupItem->child(i));
            btnList.append(btn);
            connect(btn,&MyButton::pressed,this,&BtnDialog::accept);
            connect(btn,&MyButton::signalAppClicked,this,&BtnDialog::signalAppClicked);
        }
    }
    for(int i = 0 ;i <btnList.count();i++)
    {
        mainLayout->addWidget(btnList.at(i),i/3,i%3);
    }
}

void BtnDialog::leaveEvent(QEvent *eve)
{
    this->close();
    return QDialog::leaveEvent(eve);
}
