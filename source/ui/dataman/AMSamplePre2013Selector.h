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


#ifndef AMSAMPLEPRE2013SELECTOR_H
#define AMSAMPLEPRE2013SELECTOR_H

#include <QComboBox>

class AMDatabase;

/// A combo box to choose an existing sample from the database, or create a new one.
class AMSamplePre2013Selector : public QComboBox
{
    Q_OBJECT
public:
	explicit AMSamplePre2013Selector(AMDatabase* db, QWidget *parent = 0);
	virtual ~AMSamplePre2013Selector();

	/// Returns the id of the currently-selected sample, or -1 if no samples / none selected.
	int currentSample() const;

public slots:
	/// Sets the currently-selected sample, by id. If there is no sample with that id, the currentSample() becomes -1.
	void setCurrentSample(int id);

	/// Displays the "Add Sample" dialog.
	void showAddSampleDialog();


signals:
	/// Emitted when the currentSampleId() changes, can be -1 if there are no samples / no sample selected.
	void currentSampleChanged(int sampleId);

protected slots:

	void onCurrentIndexChanged(int currentIndex);
	void onActivated(int currentIndex);
	void populateFromDb();

	void onDatabaseRowUpdated(const QString& tableName, int row);
	void onDatabaseRowAdded(const QString& tableName, int row);
	void onDatabaseRowRemoved(const QString& tableName, int row);

	/// Called when someone clicks a 'close button' on a sample to delete it
	void onSampleDeleteButtonClicked(const QModelIndex& index);

protected:

	AMDatabase* db_;
	QString tableName_;
	bool currentlyInsertingSample_;

	int lastIndex_;
};

#endif // AMSAMPLEPRE2013SELECTOR_H
