#include "AMGithubProjectManagerMainView.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	AMGithubProjectManagerMainView w;
	w.show();

	return a.exec();
}
