#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>
#include <QString>
#include <QList>
#include <QVBoxLayout>
#include <dataman/AMRun.h>
#include "dataman/AMDatabase.h"
#include "ui/AMNewRunDialog.h"


#include <QDebug>
class AMRunSelector: public QComboBox
{
	Q_OBJECT


public:
	AMRunSelector (QWidget *parent=0); //Constructor
	virtual ~AMRunSelector();            //Destructor

	int currentRunId() const ;



public slots:
	void setCurrentRunId(int runId);

protected slots:
	void autoAddRuns();
	void onComboBoxActivated(int index);
	void newRunBox();
	void addNewRun();
	void onDatabaseUpdate(const QString&, int);

signals:
	void currentRunIdChanged(int runId);

protected:

	AMDatabase* database_;
	AMNewRunDialog *box;
	bool runUpdateScheduled_;


};

#endif // COMBOBOX_H
