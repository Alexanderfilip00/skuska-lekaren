
#include "lekaren.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
   
    lekaren* w = new lekaren;
    //lekaren w;

    w->show();
    //w.show();   
    return a.exec();
}
