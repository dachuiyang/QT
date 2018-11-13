/********************************************************************************
** Form generated from reading UI file 'pr_m_wnd.ui'
**
** Created by: Qt User Interface Compiler version 5.9.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PR_M_WND_H
#define UI_PR_M_WND_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Pr_M_Wnd
{
public:
    QWidget *centralWidget;
    QTabWidget *tabWidget;
    QWidget *tab_task;
    QPushButton *btn_task;
    QTableWidget *tw_proc;
    QTableWidget *tw_mod;
    QWidget *tab_cmd;
    QTextEdit *te_cmd;
    QLineEdit *le_cmd;
    QPushButton *btn_cmd;
    QWidget *tab_reg;
    QLineEdit *le_reg;
    QPushButton *btn_reg;
    QTreeWidget *tr_reg;
    QTableView *te_reg;
    QWidget *tab;
    QTableView *tv_serv;
    QMenuBar *menuBar;
    QMenu *menu;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *Pr_M_Wnd)
    {
        if (Pr_M_Wnd->objectName().isEmpty())
            Pr_M_Wnd->setObjectName(QStringLiteral("Pr_M_Wnd"));
        Pr_M_Wnd->resize(870, 654);
        centralWidget = new QWidget(Pr_M_Wnd);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(0, 10, 841, 591));
        tab_task = new QWidget();
        tab_task->setObjectName(QStringLiteral("tab_task"));
        btn_task = new QPushButton(tab_task);
        btn_task->setObjectName(QStringLiteral("btn_task"));
        btn_task->setGeometry(QRect(720, 0, 101, 41));
        tw_proc = new QTableWidget(tab_task);
        tw_proc->setObjectName(QStringLiteral("tw_proc"));
        tw_proc->setGeometry(QRect(30, 50, 791, 301));
        tw_mod = new QTableWidget(tab_task);
        tw_mod->setObjectName(QStringLiteral("tw_mod"));
        tw_mod->setGeometry(QRect(30, 370, 791, 171));
        tabWidget->addTab(tab_task, QString());
        tab_cmd = new QWidget();
        tab_cmd->setObjectName(QStringLiteral("tab_cmd"));
        te_cmd = new QTextEdit(tab_cmd);
        te_cmd->setObjectName(QStringLiteral("te_cmd"));
        te_cmd->setGeometry(QRect(30, 90, 781, 431));
        QFont font;
        font.setFamily(QStringLiteral("Comic Sans MS"));
        te_cmd->setFont(font);
        le_cmd = new QLineEdit(tab_cmd);
        le_cmd->setObjectName(QStringLiteral("le_cmd"));
        le_cmd->setGeometry(QRect(30, 20, 561, 41));
        btn_cmd = new QPushButton(tab_cmd);
        btn_cmd->setObjectName(QStringLiteral("btn_cmd"));
        btn_cmd->setGeometry(QRect(640, 20, 141, 41));
        tabWidget->addTab(tab_cmd, QString());
        tab_reg = new QWidget();
        tab_reg->setObjectName(QStringLiteral("tab_reg"));
        le_reg = new QLineEdit(tab_reg);
        le_reg->setObjectName(QStringLiteral("le_reg"));
        le_reg->setGeometry(QRect(20, 30, 641, 41));
        btn_reg = new QPushButton(tab_reg);
        btn_reg->setObjectName(QStringLiteral("btn_reg"));
        btn_reg->setGeometry(QRect(690, 30, 131, 41));
        tr_reg = new QTreeWidget(tab_reg);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        tr_reg->setHeaderItem(__qtreewidgetitem);
        tr_reg->setObjectName(QStringLiteral("tr_reg"));
        tr_reg->setGeometry(QRect(20, 90, 471, 431));
        te_reg = new QTableView(tab_reg);
        te_reg->setObjectName(QStringLiteral("te_reg"));
        te_reg->setGeometry(QRect(520, 100, 331, 411));
        tabWidget->addTab(tab_reg, QString());
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        tv_serv = new QTableView(tab);
        tv_serv->setObjectName(QStringLiteral("tv_serv"));
        tv_serv->setGeometry(QRect(10, 10, 831, 541));
        tabWidget->addTab(tab, QString());
        Pr_M_Wnd->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(Pr_M_Wnd);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 870, 22));
        menu = new QMenu(menuBar);
        menu->setObjectName(QStringLiteral("menu"));
        Pr_M_Wnd->setMenuBar(menuBar);
        mainToolBar = new QToolBar(Pr_M_Wnd);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        Pr_M_Wnd->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(Pr_M_Wnd);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        Pr_M_Wnd->setStatusBar(statusBar);

        menuBar->addAction(menu->menuAction());

        retranslateUi(Pr_M_Wnd);

        tabWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(Pr_M_Wnd);
    } // setupUi

    void retranslateUi(QMainWindow *Pr_M_Wnd)
    {
        Pr_M_Wnd->setWindowTitle(QApplication::translate("Pr_M_Wnd", "Pr_M_Wnd", Q_NULLPTR));
        btn_task->setText(QApplication::translate("Pr_M_Wnd", "\350\216\267\345\217\226\350\277\233\347\250\213\345\210\227\350\241\250", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_task), QApplication::translate("Pr_M_Wnd", "\350\277\233\347\250\213\345\210\227\350\241\250", Q_NULLPTR));
        btn_cmd->setText(QApplication::translate("Pr_M_Wnd", "\346\211\247\350\241\214", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_cmd), QApplication::translate("Pr_M_Wnd", "cmd\346\211\247\350\241\214", Q_NULLPTR));
        le_reg->setText(QApplication::translate("Pr_M_Wnd", "HKEY_CURRENT_USER", Q_NULLPTR));
        btn_reg->setText(QApplication::translate("Pr_M_Wnd", "\346\237\245\350\257\242", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_reg), QApplication::translate("Pr_M_Wnd", "\346\263\250\345\206\214\350\241\250\347\256\241\347\220\206", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("Pr_M_Wnd", "\346\234\215\345\212\241\347\256\241\347\220\206", Q_NULLPTR));
        menu->setTitle(QString());
    } // retranslateUi

};

namespace Ui {
    class Pr_M_Wnd: public Ui_Pr_M_Wnd {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PR_M_WND_H
