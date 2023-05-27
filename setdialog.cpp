#include "setdialog.h"
#include "ui_setdialog.h"
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QFileDialog>

SetDialog::SetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetDialog)
{
    ui->setupUi(this);
    filePath = CONFIG_XML_NAME;
    setWindowTitle(CHINESE_CONVERT("菜单配置"));
    ui->settingTree->resize(250,ui->settingTree->height());
    ui->cancelBtn->setFocus();
    initMenu();
    initConnect();
    readXml();
}

SetDialog::~SetDialog()
{
    delete ui;
}
void SetDialog::initConnect()
{
    connect(ui->settingTree,&QTreeWidget::customContextMenuRequested,this,&SetDialog::slotTreeMenu);
    connect(addAppAct,&QAction::triggered,[=](){
        qDebug()<<"mapFromGlobal"<<markPos;
        QTreeWidgetItem *curItem = ui->settingTree->itemAt(markPos);
        QTreeWidgetItem * newItem = new QTreeWidgetItem;
        newItem->setText(0,CHINESE_CONVERT("新建应用"));
        newItem->setData(0,ItemRole::itemType,ItemType::appType);
        if(curItem && curItem->data(0,ItemRole::itemType).value<ItemType>()==ItemType::groupType)
        {
            curItem->addChild(newItem);
        }
        else
        {
            ui->settingTree->addTopLevelItem(newItem);
        }
        ui->settingTree->setCurrentItem(newItem);
        refreshWidget();
    });
    connect(addGroupAct,&QAction::triggered,[=](){
        QTreeWidgetItem * newItem = new QTreeWidgetItem;
        newItem->setText(0,CHINESE_CONVERT("新建组"));
        newItem->setData(0,ItemRole::itemType,ItemType::groupType);
        ui->settingTree->addTopLevelItem(newItem);
        ui->settingTree->setCurrentItem(newItem);
        refreshWidget();
    });
    connect(delGroupAct,&QAction::triggered,[=](){
        QTreeWidgetItem *curItem = ui->settingTree->currentItem();
        if(curItem && curItem->data(0,ItemRole::itemType).value<ItemType>()==ItemType::groupType)
        {
            if(curItem->childCount()>0)
            {
                QMessageBox::StandardButton reply = QMessageBox::question(nullptr,CHINESE_CONVERT("提示"),CHINESE_CONVERT("此组下存在应用,是否删除"),QMessageBox::No|QMessageBox::Yes);
                if(reply == QMessageBox::Yes)
                {
                    curItem->takeChildren();
                    ui->settingTree->takeTopLevelItem(ui->settingTree->indexOfTopLevelItem(curItem));
                }
            }
            else
            {
                ui->settingTree->takeTopLevelItem(ui->settingTree->indexOfTopLevelItem(curItem));
            }
        }
        refreshWidget();
    });
   connect(delAppAct,&QAction::triggered,[=](){
        QTreeWidgetItem *curItem = ui->settingTree->currentItem();
        if(curItem && curItem->data(0,ItemRole::itemType).value<ItemType>()==ItemType::appType)
        {
            QMessageBox::StandardButton reply = QMessageBox::question(nullptr,CHINESE_CONVERT("提示"),CHINESE_CONVERT("是否删除此应用"),QMessageBox::No|QMessageBox::Yes);
            if(reply == QMessageBox::Yes)
            {
                QTreeWidgetItem *parItem = curItem->parent();
                if(parItem) parItem->removeChild(curItem);
                else ui->settingTree->takeTopLevelItem(ui->settingTree->indexOfTopLevelItem(curItem));
            }
        }
        refreshWidget();
    });
   connect(ui->settingTree,&QTreeWidget::itemClicked,[=](){refreshWidget();});
   connect(ui->nameLed,&QLineEdit::textChanged,this,&SetDialog::slotInfoChanged);
   connect(ui->paraLed,&QLineEdit::textChanged,this,&SetDialog::slotInfoChanged);
   connect(ui->pathLed,&QLineEdit::textChanged,this,&SetDialog::slotInfoChanged);
   connect(ui->iconLed,&QLineEdit::textChanged,this,&SetDialog::slotInfoChanged);
}


void SetDialog::initMenu()
{
    treeMenu = new QMenu(this);
    addAppAct = new QAction(CHINESE_CONVERT("添加应用"));
    addGroupAct = new QAction(CHINESE_CONVERT("添加组"));
    delAppAct = new QAction(CHINESE_CONVERT("删除应用"));
    delGroupAct = new QAction(CHINESE_CONVERT("删除组"));
}

QTreeWidget *SetDialog::getTree()
{
    return ui->settingTree;
}

bool SetDialog::readXml()
{
    QFile f(filePath);
    if(!f.open(QFile::ReadOnly))
    {
        QMessageBox::warning(nullptr,CHINESE_CONVERT("告警"),CHINESE_CONVERT("配置文件打开失败"));
        return false;
    }
    QDomDocument doc;
    if(!doc.setContent(&f))
    {
        return false;
    }
    f.close();
    QDomElement root = doc.documentElement();
    if(root.tagName()!="menu") return false;
    QDomNodeList nodeList = root.childNodes();
    for(int index = 0 ; index < nodeList.count();index++)
    {
        QDomNode node = nodeList.at(index);
        if(node.toElement().tagName()=="app")
        {
            ui->settingTree->addTopLevelItem(readAppFromNode(node));
        }
        else if(node.toElement().tagName()=="group")
        {
            ui->settingTree->addTopLevelItem(readGroupFromNode(node));

        }
    }
    return true;
}

bool SetDialog::saveXml()
{
    QFile f(filePath);
    if(!f.open(QIODevice::Truncate|QIODevice::ReadWrite)) return false;
    QTextStream stream(&f);
    QDomDocument doc;
    QDomProcessingInstruction xmlInstruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"");
    doc.appendChild(xmlInstruction);  // 开始文档（XML 声明）
    //添加根节点
    QDomElement root = doc.createElement("menu");
    doc.appendChild(root);
    int num = ui->settingTree->topLevelItemCount();
    for(int i = 0 ; i < num ;i++)
    {
        QTreeWidgetItem * p_item = ui->settingTree->topLevelItem(i);
        ItemType p_item_type = p_item->data(0,ItemRole::itemType).value<ItemType>();
        if(p_item_type==ItemType::appType)
        {
            QDomElement app_elmt = doc.createElement("app");
            app_elmt.setAttribute("name",p_item->text(0));
            app_elmt.setAttribute("path",p_item->data(0,ItemRole::appPath).toString());
            app_elmt.setAttribute("para",p_item->data(0,ItemRole::appPara).toString());
            app_elmt.setAttribute("icon",p_item->data(0,ItemRole::iconPath).toString());
            root.appendChild(app_elmt);
        }
        else if(p_item_type ==ItemType::groupType)
        {
            QDomElement group_elmt = doc.createElement("group");
            group_elmt.setAttribute("name",p_item->text(0));
            group_elmt.setAttribute("icon",p_item->data(0,ItemRole::iconPath).toString());
            int app_num = p_item->childCount();
            for(int j = 0 ; j < app_num ;j++)
            {
                QTreeWidgetItem * i_item = p_item->child(j);
                if(i_item->data(0,ItemRole::itemType).value<ItemType>()==ItemType::appType)
                {
                    QDomElement app_elmt = doc.createElement("app");
                    app_elmt.setAttribute("name",i_item->text(0));
                    app_elmt.setAttribute("path",i_item->data(0,ItemRole::appPath).toString());
                    app_elmt.setAttribute("para",i_item->data(0,ItemRole::appPara).toString());
                    app_elmt.setAttribute("icon",i_item->data(0,ItemRole::iconPath).toString());
                    group_elmt.appendChild(app_elmt);
                }
            }
            root.appendChild(group_elmt);
        }
    }
    //保存到xml文件
    doc.save(stream, 4, QDomNode::EncodingFromTextStream);
    f.close();
    return true;
}


QTreeWidgetItem *SetDialog::readAppFromNode(QDomNode node)
{
    QTreeWidgetItem * retItem = nullptr;
    QDomElement element =  node.toElement();
    if(element.tagName()=="app")
    {
        QTreeWidgetItem * item = new QTreeWidgetItem;
        item->setText(0,element.attribute("name"));
        item->setData(0,ItemRole::itemType,ItemType::appType);
        item->setData(0,ItemRole::appPara,element.attribute("para"));
        item->setData(0,ItemRole::appPath,element.attribute("path"));
        item->setData(0,ItemRole::iconPath,element.attribute("icon"));
        retItem = item;
    }
    return retItem;
}

QTreeWidgetItem *SetDialog::readGroupFromNode(QDomNode node)
{
    QTreeWidgetItem * retItem = nullptr;
    QDomElement element =  node.toElement();
    if(node.toElement().tagName()=="group")
    {
        QTreeWidgetItem * g_item = new QTreeWidgetItem;
        g_item->setText(0,node.toElement().attribute("name"));
        g_item->setData(0,ItemRole::itemType,ItemType::groupType);
        g_item->setData(0,ItemRole::iconPath,node.toElement().attribute("icon"));
        ui->settingTree->addTopLevelItem(g_item);
        QDomNodeList n_nodeList = node.childNodes();
        for(int j = 0 ; j < n_nodeList.count(); j++)
        {
            QDomNode n_node = n_nodeList.at(j);
            if(n_node.toElement().tagName()=="app")
            {
                g_item->addChild(readAppFromNode(n_node));
            }
        }
        retItem = g_item;
    }
    return retItem;
}

void SetDialog::refreshWidget()
{
    QTreeWidgetItem* curItem = ui->settingTree->currentItem();
    if(curItem)
    {
        if(curItem->data(0,ItemRole::itemType).value<ItemType>()==ItemType::groupType)
        {
            ui->nameLab->setText(CHINESE_CONVERT("组名"));
            ui->nameLed->setEnabled(true);
            ui->paraLed->setEnabled(false);
            ui->pathLed->setEnabled(false);
            ui->pathChooseBtn->setEnabled(false);
        }
        else
        {
            ui->nameLab->setText(CHINESE_CONVERT("应用名"));
            ui->nameLed->setEnabled(true);
            ui->paraLed->setEnabled(true);
            ui->pathLed->setEnabled(true);
            ui->pathChooseBtn->setEnabled(true);
        }
        blockSignal(true);
        ui->nameLed->setText(curItem->text(0));
        ui->iconLed->setText(curItem->data(0,ItemRole::iconPath).toString());
        ui->pathLed->setText(curItem->data(0,ItemRole::appPath).toString());
        ui->paraLed->setText(curItem->data(0,ItemRole::appPara).toString());
        blockSignal(false);
    }
    else
    {
        ui->nameLab->setText(CHINESE_CONVERT("组名"));
        blockSignal(true);
        ui->nameLed->clear();
        ui->iconLed->clear();
        ui->pathLed->clear();
        ui->paraLed->clear();
        blockSignal(false);
        ui->nameLed->setEnabled(false);
        ui->paraLed->setEnabled(false);
        ui->pathLed->setEnabled(false);
        ui->pathChooseBtn->setEnabled(false);
    }

}

void SetDialog::blockSignal(bool flag)
{
    ui->nameLed->blockSignals(flag);
    ui->iconLed->blockSignals(flag);
    ui->pathLed->blockSignals(flag);
    ui->paraLed->blockSignals(flag);
}

void SetDialog::slotTreeMenu(const QPoint &pos)
{
    qDebug()<<" pos pos"<<pos;
    qDebug()<<"pos"<<QCursor::pos();
    qDebug()<<"mapFromGlobal"<<ui->settingTree->mapFromParent(QCursor::pos());
    markPos = pos;
    QTreeWidgetItem * currentItem = ui->settingTree->itemAt(pos);
    treeMenu->clear();
    if(!currentItem)
    {
        treeMenu->addAction(addGroupAct);
        treeMenu->addAction(addAppAct);
    }
    else if(currentItem->data(0,ItemRole::itemType).value<ItemType>()==ItemType::appType)
    {
        treeMenu->addAction(delAppAct);
    }
    else if(currentItem->data(0,ItemRole::itemType).value<ItemType>()==ItemType::groupType)
    {
        treeMenu->addAction(addAppAct);
        treeMenu->addAction(delGroupAct);
    }
    treeMenu->exec(QCursor::pos());
}

void SetDialog::slotInfoChanged()
{
    QTreeWidgetItem* curItem = ui->settingTree->currentItem();
    curItem->setText(0,ui->nameLed->text());
    curItem->setData(0,ItemRole::appPara,ui->paraLed->text());
    curItem->setData(0,ItemRole::appPath,ui->pathLed->text());
    curItem->setData(0,ItemRole::iconPath,ui->iconLed->text());
}

void SetDialog::on_pathChooseBtn_clicked()
{
   QString appPath = QFileDialog::getOpenFileName(nullptr,CHINESE_CONVERT("选择程序"),DEFAULT_DIR);
   if(!appPath.isEmpty())
   {
       ui->pathLed->setText(appPath);
   }
}

void SetDialog::on_iconChooseBtn_clicked()
{
    QString iconPath = QFileDialog::getOpenFileName(nullptr,CHINESE_CONVERT("选择图标"),DEFAULT_DIR);
    if(!iconPath.isEmpty())
    {
        ui->iconLed->setText(iconPath);
    }
}

void SetDialog::on_cancelBtn_clicked()
{
    reject();
}

void SetDialog::on_savebtn_clicked()
{
    saveXml();
    emit menuCfgChanged();
    accept();
}
