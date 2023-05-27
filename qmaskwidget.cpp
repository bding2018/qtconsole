#include "qmaskwidget.h"
#include "ui_qmaskwidget.h"
#include <qbitmap.h>
#include <QMouseEvent>
#include <qpropertyanimation.h>
#include "console_movie.h"
#include <qdesktopwidget.h>
#include "setdialog.h"
#include <qdebug.h>
#include <qmessagebox.h>
#include <qprocess.h>

QMaskWidget::QMaskWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::mainframe)
{
    ui->setupUi(this);


        QPixmap btnImg;
        btnImg.load(":/images/state_grid_mask.bmp");
        this->resize(btnImg.size());
        this->setMask(btnImg);

        QPixmap bgImg;
        bgImg.load(":/images/state_grid_bg.png");

        ui->labelMovie->resetMovie(":/images/sg.gif");

         setWindowFlags(Qt::FramelessWindowHint);

         initButtons();

           trayMenu = new QMenu(this);
           connect(trayMenu->addAction(CHINESE_CONVERT("显示")),&QAction::triggered,[this](){
               if(isHidden())
               {
                   showNormal();
               }else
               {
                   hide();
               }
           });
           connect(trayMenu->addAction(CHINESE_CONVERT("版本")),&QAction::triggered,[this](){
               QMessageBox::information(this,CHINESE_CONVERT("信息提示"),CHINESE_CONVERT("测试工具集 版权所有金智科技@2022-2023"));
           });

           connect(trayMenu->addAction(CHINESE_CONVERT("退出")),&QAction::triggered,[this](){
                closeWindowAnimation();
           });

           trayIcon = new QSystemTrayIcon(this);
           connect(trayIcon,&QSystemTrayIcon::activated,this,&QMaskWidget::slotActivated);

           trayIcon->setIcon(QIcon(":/images/icon24/btnok.png"));
           trayIcon->setContextMenu(trayMenu);
           trayIcon->show();


}

void QMaskWidget::slotActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(isHidden())
    {
        showNormal();
    }else
    {
        hide();
    }
    return;
}

void QMaskWidget::shrinkWindow()
{

    QPixmap bgImg;
    if(!m_shrinked)
    {
        bgImg.load(":/images/circle.png");
        QImage imgMask =bgImg.toImage().alphaChannel();
        QBitmap bm;
        bm = QPixmap::fromImage(imgMask.createHeuristicMask());
        setMask( bm );
    }else
    {
        bgImg.load(":/images/state_grid_mask.bmp");
        setMask(bgImg);
    }

    m_shrinked = !m_shrinked;
}

void QMaskWidget::moveWindow(int duration, int DELTX, int DELTY)
{
    QPropertyAnimation *animation = nullptr;
    animation = new QPropertyAnimation(this,"geometry");
    if(duration<100 || duration>10000)duration=1000;

    animation->setDuration(duration);
    const int COUNT = 10;
    int deltx = DELTX/COUNT;
    int delty = DELTY/COUNT;
    int xpos = this->frameGeometry().x();
    for(int i=0;i<=COUNT;i++)
    {
        xpos += deltx;
        QRect geoRect(QPoint(xpos,this->frameGeometry().y()),this->size());
        animation->setKeyValueAt(0+i*0.1,geoRect);
    }
    animation->start();
}


QMaskWidget::~QMaskWidget()
{
    delete ui;
}
void QMaskWidget::initButtons()
{
    QString strFile =QString("%1/etc/config.xml").arg(qgetenv("CAT_ROOT").data());
    if(!QFile::exists(strFile))
    {
        strFile =QString("config.xml");
        if(!QFile::exists(strFile))
        {
            QMessageBox::warning(nullptr,CHINESE_CONVERT("告警"),CHINESE_CONVERT("配置文件打开不存在:%1").arg(strFile));
            return;
        }
    }
    QVariantList varList = readConfigGroup(strFile);
    if(varList.count()==0)
    {
        return;
    }

    if(mapButton.count()>0)
    {
        for(auto k: mapButton.keys())
        {
            k->deleteLater();
        }
    }
    mapButton.clear();

    for(auto& group : varList)
    {
        QVariantMap varMap = group.toMap();
        if(varMap[MKEY_NAME].toString()==CHINESE_CONVERT("巡视系统"))
        {
            QVariantList varSubList = varMap[MKEY_SUBAPP].toList();
            int row=0,col=0;
            for(int i=0;i<varSubList.count() && i<6;i++)
            {
                QVariantMap varSubApp = varSubList[i].toMap();
                QToolButton* tb = new QToolButton(ui->layoutWidget);
                tb->setObjectName(QStringLiteral("tb%1").arg(i));
                tb->setText(varSubApp[MKEY_NAME].toString());
                tb->setMinimumSize(QSize(0, 0));
                tb->setMaximumSize(QSize(99999, 99999));
                tb->setToolButtonStyle(Qt::ToolButtonTextOnly);
                tb->setAutoRaise(false);
                tb->resize(120,40);

                col = i;
                if( col && (col%3==0))row++;
                col = col%3;
                ui->gridLayout->addWidget(tb, row, col, 1, 1);
                mapButton.insert(tb,varSubApp);

                connect(tb,&QToolButton::clicked,this,&QMaskWidget::slotClickToolButton);
            }

        }else if(varMap[MKEY_NAME].toString()==CHINESE_CONVERT("辅控系统"))
        {
            QVariantList varSubList = varMap[MKEY_SUBAPP].toList();
            int row=0,col=0;
            for(int i=0;i<varSubList.count() && i<6;i++)
            {
                QVariantMap varSubApp = varSubList[i].toMap();
                QToolButton* tb = new QToolButton(ui->layoutWidget);
                tb->setObjectName(QStringLiteral("tb%1").arg(i));
                tb->setText(varSubApp[MKEY_NAME].toString());
                tb->setMinimumSize(QSize(0, 0));
                tb->setMaximumSize(QSize(99999, 99999));
                tb->setToolButtonStyle(Qt::ToolButtonTextOnly);
                tb->setAutoRaise(false);
                tb->resize(120,40);

                col = i;
                if( col && (col%3==0))row++;
                col = col%3;
                ui->gridLayout_2->addWidget(tb, row, col, 1, 1);
                mapButton.insert(tb,varSubApp);
                connect(tb,&QToolButton::clicked,this,&QMaskWidget::slotClickToolButton);
            }
        }
    }

}
/*窗口拖动*/
void QMaskWidget::mousePressEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::LeftButton )
        {
            m_start_pos = event->globalPos() - this->frameGeometry().topLeft();
            m_button_pressed = true;
            QFrame::mousePressEvent(event);

        }
        else if ( event->button() == Qt::MiddleButton)
            closeWindowAnimation();
        else if(  event->button() == Qt::RightButton )
        shakeWindow();

}
void QMaskWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_button_pressed = false;
    QRect r =geometry();
    int   w = QApplication::desktop()->screen(-1)->width();
    int   h = QApplication::desktop()->screenGeometry(-1).height();
    if(!m_shrinked)
    {
        if(r.right() > w &&  r.left()<(w-124) )
        {
             moveWindow(1000,w-r.left()-124,0);
        }

        if(r.left()>=(w-124) )
        {
             moveWindow(1000,(w-r.width()) -r.left(),0);
        }
    }
}
void QMaskWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_button_pressed && event->buttons() == Qt::LeftButton )
    {
            m_end_pos = event->globalPos() - m_start_pos;
            this->move(m_end_pos);
    }else
    {
        QFrame::mouseMoveEvent(event);
    }
}



void QMaskWidget::shakeWindow()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this,"geometry");
    qDebug()<<frameGeometry();
    animation->setDuration(500);
    animation->setKeyValueAt(0,QRect(QPoint(this->frameGeometry().x()-3,this->frameGeometry().y()),this->size()));
    animation->setKeyValueAt(0.1,QRect(QPoint(this->frameGeometry().x()+6,this->frameGeometry().y()),this->size()));
    animation->setKeyValueAt(0.2,QRect(QPoint(this->frameGeometry().x()-6,this->frameGeometry().y()),this->size()));
    animation->setKeyValueAt(0.3,QRect(QPoint(this->frameGeometry().x()+6,this->frameGeometry().y()),this->size()));
    animation->setKeyValueAt(0.4,QRect(QPoint(this->frameGeometry().x()-6,this->frameGeometry().y()),this->size()));
    animation->setKeyValueAt(0.5,QRect(QPoint(this->frameGeometry().x()+6,this->frameGeometry().y()),this->size()));
    animation->setKeyValueAt(0.6,QRect(QPoint(this->frameGeometry().x()-6,this->frameGeometry().y()),this->size()));
    animation->setKeyValueAt(0.7,QRect(QPoint(this->frameGeometry().x()+6,this->frameGeometry().y()),this->size()));
    animation->setKeyValueAt(0.8,QRect(QPoint(this->frameGeometry().x()-6,this->frameGeometry().y()),this->size()));
    animation->setKeyValueAt(0.9,QRect(QPoint(this->frameGeometry().x()+6,this->frameGeometry().y()),this->size()));
    animation->setKeyValueAt(1,QRect(QPoint(this->frameGeometry().x()-3,this->frameGeometry().y()),this->size()));
    animation->start();
}

void QMaskWidget::closeWindowAnimation()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this,"windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(1.0);
    animation->setKeyValueAt(0.3,0.5f);
    animation->setKeyValueAt(0.6,0.4f);
    animation->setKeyValueAt(0.8,0.2f);
    animation->setEndValue(0.0);
    animation->start();
    shakeWindow();
    connect(animation, &QPropertyAnimation::finished, qApp, &QApplication::quit);

    /*QSettings settings("wiscom", "lcd");
    QString group("paras");
    settings.beginGroup(group);


    settings.setValue("grpNetwork",ui->grpNetwork->isChecked() ? 1 : 0);
    settings.setValue("lineIPAddr",ui->lineIPAddr->text());
    settings.setValue("spinPort",ui->spinPort->value());

    settings.setValue("spinDataInterval",ui->spinDataInterval->value());
    settings.setValue("spinMsgRetryInterval",ui->spinMsgRetryInterval->value());
    settings.setValue("lineReverseFolder",ui->lineReverseFolder->text());
    settings.setValue("spinPacketSlice",ui->spinPacketSlice->value());
    settings.setValue("spinTimeout",ui->spinTimeout->value());

    settings.setValue("cbAutoConnect",ui->cbAutoConnect->isChecked()?  1 : 0);
    settings.setValue("cbAutoRefresh",ui->cbAutoRefresh->isChecked() ? 1 : 0);
    settings.endGroup();*/
}

void QMaskWidget::startAnimation()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this,"windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();
}
void QMaskWidget::slotClickToolButton()
{
    QToolButton* tb = dynamic_cast<QToolButton*>(sender());
    if(tb)
    {
        QVariantMap& mapApp = mapButton[tb];
        QString strPara = QString("%1").arg(mapApp[MKEY_PARA].toString());
        DEBUG_VAR(mapApp[MKEY_PATH]);
        DEBUG_VAR(mapApp[MKEY_PARA]);
        if(mapApp[MKEY_PATH].isValid() && !mapApp[MKEY_PATH].toString().isEmpty())
        {
            if(AppManager::getInstance()->checkApp(mapApp[MKEY_REG].toString())<0)
            {
                QProcess process;
                if(!process.startDetached(mapApp[MKEY_PATH].toString(),strPara.split(" ",QString::SkipEmptyParts)))
                {
                    QMessageBox::warning(nullptr,CHINESE_CONVERT("告警"),CHINESE_CONVERT("程序启动失败"));
                }
            }else
            {
                AppManager::getInstance()->setState(mapApp[MKEY_NAME].toString(),Appstate::activeWindow);
            }
        }
    }
}
void  QMaskWidget::timerEvent(QTimerEvent *event)
{

}

void QMaskWidget::on_pbClose_clicked()
{
    hide();
}

void QMaskWidget::on_pbConfig_clicked()
{
    SetDialog *s_dlg = new SetDialog(this);
    s_dlg->show();

    if(s_dlg->exec()==QDialog::Accepted)
    {

    }
    delete s_dlg;
    s_dlg = nullptr;
    /*connect(s_dlg,&SetDialog::menuCfgChanged,[=](){
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
    });*/
}

QVariantList QMaskWidget::readConfigGroup(const QString& strFile)
{
    QVariantList varRet;
    QFile f(strFile);
    if(!f.open(QFile::ReadOnly))
    {
        QMessageBox::warning(nullptr,CHINESE_CONVERT("告警"),CHINESE_CONVERT("配置文件打开失败:%1").arg(strFile));
        return varRet;
    }

    QDomDocument doc;

    if(!doc.setContent(&f))
    {
        QMessageBox::warning(nullptr,CHINESE_CONVERT("告警"),CHINESE_CONVERT("无效的xml文件:%1").arg(strFile));
        return varRet;
    }
    f.close();
    QDomElement root = doc.documentElement();
    if(root.tagName()!="menu")
    {
        QMessageBox::warning(nullptr,CHINESE_CONVERT("告警"),CHINESE_CONVERT("无效的xml文件:%1").arg(strFile));
        return varRet;
    }

    std::function<QVariantMap(QDomElement& element)> parseDom=[&parseDom](QDomElement& element){
        QVariantMap mapVar;

        if(element.tagName()=="app")
        {
            mapVar.insert(MKEY_TYPE,ItemType::appType);
            mapVar.insert(MKEY_X,element.attribute(MKEY_X));
            mapVar.insert(MKEY_Y,element.attribute(MKEY_Y));
            mapVar.insert(MKEY_W,element.attribute(MKEY_W));
            mapVar.insert(MKEY_H,element.attribute(MKEY_H));
            mapVar.insert(MKEY_REG,element.attribute(MKEY_REG));
        }else if(element.tagName()=="group")
        {
            mapVar.insert(MKEY_TYPE,ItemType::groupType);
            QDomNodeList& nodeList = element.childNodes();
            QVariantList  lstSubApp;
            for(int i=0;i<nodeList.count();i++)
            {
                QVariantMap mapChild = parseDom(nodeList.at(i).toElement());
                if(!mapChild.isEmpty())
                {
                     lstSubApp.append(mapChild);
                }
            }
            if(!lstSubApp.isEmpty())
            {
                mapVar.insert(MKEY_SUBAPP,lstSubApp);
            }
        }
        mapVar.insert(MKEY_ID,element.attribute(MKEY_ID));
        mapVar.insert(MKEY_NAME,element.attribute(MKEY_NAME));
        mapVar.insert(MKEY_PATH,element.attribute(MKEY_PATH));
        mapVar.insert(MKEY_PARA,element.attribute(MKEY_PARA));
        mapVar.insert(MKEY_ICON,element.attribute(MKEY_ICON));
        mapVar.insert(MKEY_TOOLTIP,element.attribute(MKEY_TOOLTIP));
        return mapVar;
    };

    QDomNodeList nodeList = root.childNodes();
    for(int index = 0 ; index < nodeList.count();index++)
    {
        QDomNode &node = nodeList.at(index);
        QDomElement element = node.toElement();
        QVariantMap mapGroup = parseDom(element);
        if(!mapGroup.isEmpty())
        {
            varRet.append(mapGroup);
        }
    }

    return varRet;
}

bool QMaskWidget::saveConfigGroup(const QString& strFile,QVariantList& listConfig)
{
    QFile f(strFile);
    if(!f.open(QIODevice::Truncate|QIODevice::ReadWrite)) return false;
    QTextStream stream(&f);
    QDomDocument doc;
    QDomProcessingInstruction xmlInstruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"");
    doc.appendChild(xmlInstruction);  // 开始文档（XML 声明）
    //添加根节点
    QDomElement root = doc.createElement("menu");

    for(auto & grp: listConfig)
    {
            QDomElement group_elmt = doc.createElement("group");
            QVariantMap mapVar = grp.toMap();
            group_elmt.setAttribute(MKEY_ID,mapVar[MKEY_ID].toString());
            group_elmt.setAttribute(MKEY_NAME,mapVar[MKEY_NAME].toString());
            group_elmt.setAttribute(MKEY_ICON,mapVar[MKEY_ICON].toString());
            group_elmt.setAttribute(MKEY_TYPE,mapVar[MKEY_TYPE].toString());
            group_elmt.setAttribute(MKEY_TOOLTIP,mapVar[MKEY_TOOLTIP].toString());

            QVariantMap mapSub = mapVar[MKEY_SUBAPP].toMap();
            if(!mapSub.isEmpty())
            {

                    QDomElement app_elmt = doc.createElement("app");
                    app_elmt.setAttribute(MKEY_ID,mapSub[MKEY_ID].toString());
                    app_elmt.setAttribute(MKEY_NAME,mapSub[MKEY_NAME].toString());
                    app_elmt.setAttribute(MKEY_PATH,mapSub[MKEY_PATH].toString());
                    app_elmt.setAttribute(MKEY_ICON,mapSub[MKEY_ICON].toString());
                    app_elmt.setAttribute(MKEY_PARA,mapSub[MKEY_PARA].toString());
                    app_elmt.setAttribute(MKEY_TYPE,mapSub[MKEY_TYPE].toString());
                    app_elmt.setAttribute(MKEY_REG, mapSub[MKEY_REG].toString());

                    app_elmt.setAttribute(MKEY_X,mapSub[MKEY_X].toString());
                    app_elmt.setAttribute(MKEY_Y,mapSub[MKEY_Y].toString());
                    app_elmt.setAttribute(MKEY_W,mapSub[MKEY_W].toString());
                    app_elmt.setAttribute(MKEY_H,mapSub[MKEY_H].toString());

                    app_elmt.setAttribute(MKEY_TOOLTIP,mapSub[MKEY_TOOLTIP].toString());
                    group_elmt.appendChild(app_elmt);
                }

            root.appendChild(group_elmt);

    }
     doc.appendChild(root);
    //保存到xml文件
    doc.save(stream, 4, QDomNode::EncodingFromTextStream);
    f.close();
    return true;
}
