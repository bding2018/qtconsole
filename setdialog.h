#ifndef SETDIALOG_H
#define SETDIALOG_H

#include <QDialog>
#include <QDomDocument>
#include <QTreeWidget>
#include <QMenu>
#include <QDebug>

#define CONFIG_XML_NAME "menu_config.xml"
#define CHINESE_CONVERT(x) QString::fromLocal8Bit(x)
#define DEBUG_POS    qDebug()<<__FILE__<<","<<__LINE__<<">:";
#define DEBUG_VAR(x) qDebug()<<#x<<"="<<x;
#define DEFAULT_DIR ""

#define MKEY_NAME "name"
#define MKEY_REG  "reg"
#define MKEY_ICON "icon"
#define MKEY_PARA "para"
#define MKEY_PATH "path"
#define MKEY_TYPE "type"
#define MKEY_ID   "id"
#define MKEY_X    "x"
#define MKEY_Y    "y"
#define MKEY_W    "w"
#define MKEY_H    "h"
#define MKEY_TOOLTIP "tooltip"
#define MKEY_SUBAPP  "subapp"

namespace Ui {
class SetDialog;
}

enum ItemRole
{
    itemType = Qt::UserRole,
    iconPath = Qt::UserRole +1,
    appPath = Qt::UserRole +2,
    appPara = Qt::UserRole +3
};
enum ItemType
{
    appType,
    groupType
};



class SetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetDialog(QWidget *parent = nullptr);
    ~SetDialog();

    bool readXml();

    bool saveXml();

    void initConnect();

    void initMenu();

    QTreeWidget *getTree();


private:
    Ui::SetDialog *ui;
    //文件路径
    QString filePath;
    //树菜单
    QMenu* treeMenu;
    //树操作
    QAction *addGroupAct,*addAppAct,*delGroupAct,*delAppAct;
    //读取APP
    QTreeWidgetItem * readAppFromNode(QDomNode node);
    //读取group
    QTreeWidgetItem * readGroupFromNode(QDomNode node);
    //刷新信息窗口,添加或修改时刷新
    void refreshWidget();
    //记录右键按下时settingTree的坐标
    QPoint markPos;

    void blockSignal(bool flag);

private slots:
    void slotTreeMenu(const QPoint &pos);

    void slotInfoChanged();
    void on_pathChooseBtn_clicked();
    void on_iconChooseBtn_clicked();
    void on_cancelBtn_clicked();
    void on_savebtn_clicked();
signals:
    void menuCfgChanged();
};
Q_DECLARE_METATYPE(ItemType)
#endif // SETDIALOG_H
