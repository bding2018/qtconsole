#include "consolewidget.h"
#include "ui_consolewidget.h"
#include <QProcess>
#include "common_head.h"
#include <QFile>

consoleWidget::consoleWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::consoleWidget)
{
    ui->setupUi(this);
	initAppBtn();
    this->setWindowTitle("NRR_tool");

    QFile fileQss(QString(":/logo.qss"));
    {
        if (fileQss.open(QIODevice::ReadOnly))
        {
            QTextStream ts(&fileQss);
            setStyleSheet(ts.readAll());
            fileQss.close();
        }
    }

}

consoleWidget::~consoleWidget()
{
    delete ui;
}

void consoleWidget::initAppBtn()
{
    int row_btn_num = 3;
    SetDialog *s_dlg = new SetDialog(this);
    QTreeWidget*tree = s_dlg->getTree();
    for(int i = 0 ; i < tree->topLevelItemCount();i++)
    {
        QTreeWidgetItem* f_item = tree->topLevelItem(i);
        MyButton * btn = new MyButton(this);
        btn->loadFromItem(f_item);
        ui->mainLayout->addWidget(btn,i/row_btn_num,i%row_btn_num);
        connect(btn,&MyButton::signalAppClicked,this,&consoleWidget::slotAppBtnClicked);
    }
    qDebug()<<pos();
}

void consoleWidget::on_btnSet_clicked()
{
    SetDialog *s_dlg = new SetDialog(this);
    s_dlg->show();
    connect(s_dlg,&SetDialog::menuCfgChanged,[=](){
        int row = ui->mainLayout->rowCount();
        int column = ui->mainLayout->columnCount();
        QList<QWidget*> widgetList;

        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < column; j++)
            {
                if (ui->mainLayout->itemAtPosition(i, j))
                {
                    QWidget* childWidget = ui->mainLayout->itemAtPosition(i, j)->widget();
                    widgetList.append(childWidget);
                }
            }
        }

        for (int no = 0; no < widgetList.count(); no++)
        {
            ui->mainLayout->removeWidget(widgetList.at(no));
            delete widgetList.at(no);
        }

        initAppBtn();
    });
}

void consoleWidget::slotAppBtnClicked(const QString &appPath, const QString &para)
{
    qDebug()<<"appPath"<<appPath<<"para"<<para;
    QString appName = appPath.split("/").last();
    appName = appName.remove(".exe");
    if(AppManager::getInstance()->checkApp(appName)<0)
    {
        qDebug()<<"start app"<<appName;
        qDebug()<<"para"<<para;
        QProcess process;
        if(!process.startDetached(appPath,para.split(" ",QString::SkipEmptyParts)))
        {
            QMessageBox::warning(nullptr,CHINESE_CONVERT("告警"),CHINESE_CONVERT("程序启动失败"));
        }

    }
    else
    {
        qDebug()<<"set app "<<appName<<" active";
        AppManager::getInstance()->setState(appName,Appstate::activeWindow);
    }
}


