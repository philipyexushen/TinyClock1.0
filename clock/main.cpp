#include <QApplication>
#include "digtalwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DigtalWidget *widget = new DigtalWidget;
    
    widget->show();
    
    return a.exec();
}
