#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>
#include <QString>
#include <QList>
#include <QVBoxLayout>
#include <dataman/AMRun.h>
#include "dataman/AMDatabase.h"
#include "ui/AMNewRunDialog.h"


class AMComboBox: public QComboBox
{
	Q_OBJECT


public:
	AMComboBox (QWidget *parent=0); //Constructor
	//virtual ~AMComboBox();            //Destructor
	~AMComboBox();
	int currentRunId() const ;

protected slots:
	void onComboBoxActivated(int index);
	void newRunBox();
	void addNewRun();

signals:
	void currentRunIdChanged(int runId);

protected:
	void autoAddRuns();
	AMDatabase* database_;
	AMNewRunDialog *box;

};

#endif // COMBOBOX_H
