#include "StripChart/StripToolView.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    StripToolView s;
    s.show();
    
    return app.exec();
}
