#ifndef Q_REG_H
#define Q_REG_H

#include <QMap>
#include <QString>
#include <QTableView>
#include <QTreeWidgetItem>
#include <QAbstractItemModel>
#include <QStandardItemModel>


enum Em_tv_reg
{
    REG_KEY,
    REG_VALUE
};

typedef struct
{
    QString str_title;
    int n_width;
}Col_Data;


//class Q_Reg;
//typedef void (Q_Reg::*PFN_REG_GROUP)(QTreeWidgetItem* item,
//                              QString& str);

//typedef void (Q_Reg::*PFN_REG_KEY)(int n_index,
//                            QString& key,
//                            QString& value);

class Q_Reg
{
public:
    Q_Reg();

public:    
    static int get_key_count(QString& reg_path);
    static int get_group_count(QString& reg_path);

    static QMap<QString,QString> enum_reg_key(QString& reg_path);
    static QStringList enum_reg_group(QString& reg_path);

    static QStringList enum_reg_group(QTreeWidgetItem* item);

public:
public:
    static void add_child_group(QTreeWidgetItem* item,
                                QString& str);
    static void add_child_group(QTreeWidgetItem* item,
                                QStringList& str_lst);

public:
    static QString get_reg_path(QTreeWidgetItem* item,
                                int column = 0);
    static bool sec_tra(QTreeWidgetItem* item,
                        int column = 0);

public:
    void init_tv_reg(QTableView *tv);   //初始化键值对的列表框
    void init_tw_reg(QTreeWidget *tr);  //注册表树形图

public:
    void tra_reg(QString cur_path);
    void tra_reg_key(QTreeWidgetItem *item, int column);

private:
    QTableView* m_tv_reg;
    QTreeWidget* m_tr_reg;
    QStandardItemModel* m_model_reg;
};
#endif // Q_REG_H
