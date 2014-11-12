/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>
#include <QString>
#include <QList>
#include <QVBoxLayout>
#include "dataman/AMRun.h"
#include "dataman/database/AMDatabase.h"
#include "ui/dataman/AMNewRunDialog.h"

class AMDetailedItemDelegate;

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

	int findIndexForRunId(int runId) const;

signals:
	void currentRunIdChanged(int runId);

protected:

	AMDatabase* database_;
	AMNewRunDialog *newRunDialog_;

	bool runUpdateScheduled_;
	int lastValidRunId_;

	AMDetailedItemDelegate *itemDelegate_;
};

#endif // COMBOBOX_H
