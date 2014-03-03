#include "StripChart/StripTool.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	StripTool s;
	s.show();
    
	return app.exec();
}
