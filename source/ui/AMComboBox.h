#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>
#include <QString>
#include <QList>
#include <QVBoxLayout>
#include <dataman/AMRun.h>
#include "dataman/AMDatabase.h"


class AMComboBox: public QComboBox
{
	Q_OBJECT


public:
	AMComboBox (QWidget *parent); //Constructor



	//virtual ~AMComboBox();            //Destructor
	~AMComboBox();

	int currentRunId() const ;

protected slots:
	void onComboBoxActivated(int index);
	//void facilitySelectChanged();

signals:
	void currentRunIdChanged(int runId);

protected:

	void autoAddRuns();

	AMDatabase* database_;

};

#endif // COMBOBOX_H
