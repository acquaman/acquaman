#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>
#include <QString>
#include <QList>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QInputDialog>
#include <dataman/AMRun.h>
#include "dataman/AMDatabase.h"

class AMComboBox: public QComboBox
{
	Q_OBJECT


public:
	AMComboBox (QWidget *parent = 0); //Constructor

	void addRun();

	//virtual ~AMComboBox();            //Destructor
	~AMComboBox();
protected slots:
	void onComboBoxActivated(int index);

protected:
	void autoAddRuns();

	AMDatabase* database_;

};

#endif // COMBOBOX_H
