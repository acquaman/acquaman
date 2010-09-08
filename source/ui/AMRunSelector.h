#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>
#include <QString>
#include <QList>
#include <QVBoxLayout>
#include <dataman/AMRun.h>
#include "dataman/AMDatabase.h"
#include "ui/AMNewRunDialog.h"


/// author: Rachel Si (August 2010)
class AMRunSelector: public QComboBox
{
	Q_OBJECT


public:
	AMRunSelector (AMDatabase* db, QWidget *parent=0); //Constructor
	virtual ~AMRunSelector();            //Destructor

	int currentRunId() const;


public slots:
	void setCurrentRunId(int runId);
	void showAddRunDialog();

protected slots:
	void populateRuns();
	void onComboBoxActivated(int index);

	void onAddRunDialogClosed(int newRunId);

	void onDatabaseUpdate(const QString&, int);

signals:
	void currentRunIdChanged(int runId);

protected:

	AMDatabase* database_;
	AMNewRunDialog *newRunDialog_;

	bool runUpdateScheduled_;
	int lastValidRunId_;


};

#endif // COMBOBOX_H
