#ifndef AMSTARTSCREEN_H
#define AMSTARTSCREEN_H


#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include "ui/AMRunSelector.h"
#include "dataman/AMDatabase.h"

class AMStartScreen : public QWidget
{
	Q_OBJECT
public:
	AMStartScreen(QWidget *parent = 0);

signals:

public slots:
	bool storeCurrentRun();

protected:
	AMRunSelector *runSelector_;

	virtual void closeEvent(QCloseEvent *);

};




#endif // AMSTARTSCREEN_H
