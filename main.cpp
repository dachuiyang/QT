#include "pr_m_wnd.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Pr_M_Wnd w;
    w.show();

    return a.exec();
}
