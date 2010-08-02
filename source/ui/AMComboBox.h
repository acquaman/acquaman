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



	//virtual ~AMComboBox();            //Destructor
	~AMComboBox();

	int currentRunId() const ;

protected slots:
	void onComboBoxActivated(int index);

signals:
	void currentRunIdChanged(int runId);

protected:
	int addRun();
	void autoAddRuns();

	AMDatabase* database_;

};

#endif // COMBOBOX_H
