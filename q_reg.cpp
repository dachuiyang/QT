#include "q_reg.h"
#include <QSettings>
#include <QStringList>
#include <QDebug>

#define REG_ROOT_NUM 5

Col_Data reg_col_ary[] =
{
  {"key", 80},
  {"value", 80}
};

char* sz_root_ary[] =
{
    "HKEY_CLASSES_ROOT",
    "HKEY_CURRENT_USER",
    "HKEY_LOCAL_MACHINE",
    "HKEY_USERS",
    "HKEY_CURRENT_CONFIG"
};


Q_Reg::Q_Reg()
{
    this->m_tv_reg = 0;
    this->m_model_reg = 0;

    this->m_model_reg = new QStandardItemModel;
}

int Q_Reg::get_key_count(QString &reg_path)
{
    QSettings *p_set = 0;
    p_set = new QSettings(reg_path,
                          QSettings::NativeFormat);
    //获取所有 childkey
    QStringList key_list = p_set->childKeys();
    delete p_set;

    return key_list.count();
}

int Q_Reg::get_group_count(QString &reg_path)
{
    QSettings *p_set = 0;
    p_set = new QSettings(reg_path,
                          QSettings::NativeFormat);

    //获取所有 childgroup
    QStringList group_list = p_set->childGroups();

    delete p_set;

    return group_list.count();
}

//枚举目录的所有 childkey 返回
QMap<QString,QString> Q_Reg::enum_reg_key(QString& reg_path)
{
    QString str_key;
    QString str_value;  
    QMap<QString, QString>reg_map;

    QSettings *p_set = 0;
    p_set = new QSettings(reg_path,
                          QSettings::NativeFormat);

    //遍历 childKeys
    QStringList key_list = p_set->childKeys();

    foreach(str_key, key_list)
    {
        str_value = p_set->value(str_key).toString();
        reg_map.insert(str_key, str_value);
    }

    delete p_set;
    return reg_map;
}

//枚举目录的所有 childgroup  返回grouplst
QStringList Q_Reg::enum_reg_group(QString& reg_path)
{
    QSettings *p_set = new QSettings(reg_path,
                                     QSettings::NativeFormat);

    //遍历 childGroups
    QStringList group_list = p_set->childGroups();

    delete p_set;
    return group_list;
}
//枚举控件的子项 如果已有成员 返回
QStringList Q_Reg::enum_reg_group(QTreeWidgetItem *item)
{
    QStringList group_lst;

    if(0 == item || item->childCount() > 0)
    {
        return group_lst;
    }

    QString cur_path = Q_Reg::get_reg_path(item);
    group_lst = enum_reg_group(cur_path);

    return group_lst;
}

//获取当前目录全路径
QString Q_Reg::get_reg_path(QTreeWidgetItem *item, int column)
{
    //获取父目录全路径
    QString str_ret;
    QTreeWidgetItem * parent_item = 0;

    parent_item = item->parent();

    // 父目录\\父目录\\ 当前目录
    while(parent_item != 0)
    {
        str_ret.prepend('\\');
        str_ret.prepend(parent_item->text(column));

        parent_item = parent_item->parent();
    }

    QString str_cur = item->text(column);
    str_ret.append(str_cur);

    return str_ret;
}

//二级遍历功能函数
bool Q_Reg::sec_tra(QTreeWidgetItem *item, int column)
{
    if(0 == item)
    {
        return false;
    }

    //获取当前路径
//    QString cur_path = get_reg_path(item, column);

    //枚举当前子项 加入控件
    QStringList son_group_lst = Q_Reg::enum_reg_group(item);
    Q_Reg::add_child_group(item, son_group_lst);

    for(int n_index = 0; n_index < item->childCount(); n_index++)
    {
        QTreeWidgetItem* p_son_item = item->child(n_index);

        //获取子控件全路径
//        QString son_path = Q_Reg::get_reg_path(p_son_item);

        //枚举子项的子项 加入控件
        QStringList gson_group_lst = Q_Reg::enum_reg_group(p_son_item);
        Q_Reg::add_child_group(p_son_item, gson_group_lst);

    }

//    foreach (QTreeWidgetItem* p_son_item, item->children())
//    {
//        //获取子控件全路径
//        QString son_path = Q_Reg::get_reg_path(p_son_item);

//        //枚举子项的子项 加入控件
//        QStringList gson_group_lst = Q_Reg::enum_reg_group(son_path);
//        Q_Reg::add_child_group(p_son_item, gson_group_lst);

//    }

    //这里子项 如果为空 根本不会进入循环
//    int n_index = 0;
//    foreach (QString son_group, son_group_lst)
//    {
//        //枚举当前子项的子项 加入控件
//        QStringList gson_group_lst = Q_Reg::enum_reg_group(son_group);

//        if(gson_group_lst.isEmpty())
//        {
//            continue;
//        }

//        Q_Reg::add_child_group(item->child(n_index), gson_group_lst);

//        n_index++;
//    }
}

void Q_Reg::add_child_group(QTreeWidgetItem *item, QString &str)
{
    //参数出错
    if(0 == item || str.isEmpty())
    {
        return;
    }

    //添加子项到item
    QTreeWidgetItem* p_child_item =
            new QTreeWidgetItem(item, QStringList(str));

    item->addChild(p_child_item);
}

void Q_Reg::add_child_group(QTreeWidgetItem *item, QStringList &str_lst)
{
    //参数呵呵
    if(0 == item || str_lst.isEmpty())
    {
        return;
    }

    QList<QTreeWidgetItem*> son_item_lst;

    foreach (QString str_reg, str_lst)
    {
        son_item_lst.append(new QTreeWidgetItem(QStringList(str_reg)));
    }

    //添加子项到item
    item->addChildren(son_item_lst);
}


void Q_Reg::init_tv_reg(QTableView *tv)
{
    QStringList head_lst;

    if(0 == tv)
    {
        return;
    }

    this->m_tv_reg = tv;
    this->m_tv_reg->setModel(this->m_model_reg);

    //标题添加到 stringlist
//    foreach (Col_Data cd, reg_col_ary)
//    {
//        head_lst.append(cd.str_title);
//    }

    head_lst.append(reg_col_ary[REG_KEY].str_title);
    head_lst.append(reg_col_ary[REG_VALUE].str_title);

    //设置列表头
    this->m_model_reg->setHorizontalHeaderLabels(head_lst);
}

void Q_Reg::init_tw_reg(QTreeWidget *tr)
{
    this->m_tr_reg = tr;

    QList<QTreeWidgetItem*> root_lst;

    //添加根键数据
    for(int n_index = 0; n_index < REG_ROOT_NUM; n_index++)
    {
        QString root_str = QString(sz_root_ary[n_index]);
        QTreeWidgetItem* p_item = new QTreeWidgetItem(QStringList(root_str));

        //添加到跟键列表
        root_lst.append(p_item);

        //二级遍历
        Q_Reg::sec_tra(p_item);
    }

    tr->addTopLevelItems(root_lst);

//    QStringList root_lst;

//    //添加根键数据
//    for(int n_index = 0; n_index < REG_ROOT_NUM; n_index++)
//    {
//        root_lst << sz_root_ary[n_index];
//    }

//    //添加root数据
//    QTreeWidgetItem* p_root_item = new QTreeWidgetItem(root_lst);
    //    tr->addTopLevelItem(p_root_item);
}

void Q_Reg::tra_reg(QString cur_path)
{
    if(cur_path.isEmpty())
    {
        return;
    }

    int n_child_key = 0;
    int n_child_group = 0;

    //获取子目录和子键大小
    n_child_key = Q_Reg::get_key_count(cur_path);
    n_child_group = Q_Reg::get_group_count(cur_path);

    if(0 == n_child_group && 0 == n_child_key)
    {
        return;
    }

    if(n_child_group > 0)
    {
        this->m_tv_reg->clearSpans();

        QTreeWidgetItem* p_item = 0;
        p_item = new QTreeWidgetItem(QStringList(cur_path));
        this->m_tr_reg->addTopLevelItem(p_item);

        Q_Reg::sec_tra(p_item);
    }

    if(n_child_key > 0)
    {
        QMap<QString, QString>reg_map;

        //设置控件model
        QStandardItemModel* tv_model = 0;
        tv_model = new QStandardItemModel();

        QMap<QString,QString>::Iterator it;
        for(it = reg_map.begin(); it != reg_map.end();++it)
        {
            //初始化
            QList<QStandardItem*> row_item;
            QStandardItem* p_key_item = 0;
            QStandardItem* p_value_item = 0;

            //赋值key value item
            p_key_item = new QStandardItem(it.key());
            p_value_item = new QStandardItem(it.value());

            row_item << p_key_item
                     << p_value_item;

            //加入model
            tv_model->appendRow(row_item);
        }

        this->m_tv_reg->setModel(tv_model);

//        for(int n_index = 0; n_index < reg_map.count(); n_index++)
//        {


//           p_row_item = new QTableWidgetItem();
//           p_key_item = new QStandardItem(reg_map.key);
//           p_value_item = new QStandardItem();

//           tv_model->appendRow(QStandardItem);
//        }
    }
}

void Q_Reg::tra_reg_key(QTreeWidgetItem *item, int column)
{
    QString cur_path = Q_Reg::get_reg_path(item);

    this->m_model_reg->clear();

    QMap<QString, QString>reg_map;
    reg_map = Q_Reg::enum_reg_key(cur_path);

    //遍历插入子项
    QMap<QString,QString>::Iterator it;
    for(it = reg_map.begin(); it != reg_map.end();++it)
    {
        //初始化
        QList<QStandardItem*> row_item;
        QStandardItem* p_key_item = 0;
        QStandardItem* p_value_item = 0;

        //赋值key value item
        p_key_item = new QStandardItem(it.key());
        p_value_item = new QStandardItem(it.value());

        row_item << p_key_item
                 << p_value_item;

        //加入model
        this->m_model_reg->appendRow(row_item);
    }
}
