#ifndef AMSTARTSCREEN_H
#define AMSTARTSCREEN_H


#include <QSplashScreen>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include "ui/AMRunSelector.h"
#include "dataman/AMDatabase.h"

class AMStartScreen : public QSplashScreen
{
	Q_OBJECT
public:
	AMStartScreen(QWidget *parent = 0);

signals:

public slots:
	void storeCurrentRun();
private:
	AMRunSelector *box;

};




#endif // AMSTARTSCREEN_H
