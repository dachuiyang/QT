#ifndef SERV_DLG_H
#define SERV_DLG_H

#include <QDialog>
#include <QTableView>
#include <QStandardItemModel>
#include "ps_func.h"

namespace Ui 
{
    class Serv_Dlg;
}

class Serv_Dlg : public QDialog, public Ps_Func
{
    Q_OBJECT

public:
    explicit Serv_Dlg(QWidget *parent = 0);
    ~Serv_Dlg();
    
public:
    virtual void dps_data(QByteArray buf);

private:
    Ui::Serv_Dlg *ui;
};

#endif // SERV_DLG_H
