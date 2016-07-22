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


#ifndef AMNEWRUNDIALOG_H
#define AMNEWRUNDIALOG_H

#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QSqlQuery>
#include <QDialog>

#include "dataman/database/AMDatabase.h"
#include "dataman/AMRun.h"

/// author: Rachel Si (2010)
class AMNewRunDialog: public QDialog
{
	Q_OBJECT

public:
	AMNewRunDialog(AMDatabase* db, QWidget *parent = 0); //constructor
	virtual ~AMNewRunDialog();

protected slots:
	void facilitySelectCbChanged(int index);
	void okButtonPressed();
	void cancelButtonPressed();

signals:
	/// Emitted when the dialog box is cancelled or accepted. The newRunId is the id of the newly created run, or -1 if cancelled.
	void dialogBoxClosed(int newRunId);

protected:
	void addFacility();

	QComboBox *facilitySelectCb_;

	QLineEdit *runNameLineEdit_;

	AMDatabase* database_;
};
#endif // AMNEWRUNDIALOG_H
