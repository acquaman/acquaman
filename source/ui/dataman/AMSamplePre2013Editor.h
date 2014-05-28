/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMSAMPLEPRE2013EDITOR_H
#define AMSAMPLEPRE2013EDITOR_H

#include <QWidget>
#include <QHash>
#include <QModelIndex>

class AMSamplePre2013;
class AMDatabase;

class QComboBox;
class QLineEdit;
class QLabel;
class QVBoxLayout;

/// The AMSampleEditor class provides a widget that lets the user choose a sample from the list of samples, and edit characteristics of the sample.  The changes made to the sample are saved back to the database immediately.
class AMSamplePre2013Editor : public QWidget
{
Q_OBJECT
public:
	/// Default constructor
	explicit AMSamplePre2013Editor(AMDatabase* db, QWidget *parent = 0);

	/// Returns the id of the currently-selected sample, or -1 if it's a non-existent/invalid sample
	int currentSampleId() const;

signals:

public slots:
	/// Set the currently-selected sample, by ID
	void setCurrentSampleFromId(int id);

	/// Create a new sample and set it as the currently-selected sample
	void createNewSample();

protected slots:
	/// Call this to refresh the list of samples in the ComboBox from the database. Set refreshId_ to -1 for a complete refresh, or to the sample id for an optimized single update.
	void refreshSamples();

	/// Helper function to save a sample when it is edited
	void saveCurrentSample();

	/// This is called when a row in the database is updated:
	void onDatabaseUpdated(const QString& tableName, int id);
	/// In the future, the onDatabaseCreatedd() and onDatabaseRemoved() functions could be used to handle specific situations/optimizations. Right now they just call onDatabaseUpdated() for a full model update.
	void onDatabaseCreated(const QString& tableName, int /*id*/) { onDatabaseUpdated(tableName, -1); }
	void onDatabaseRemoved(const QString& tableName, int /*id*/) { onDatabaseUpdated(tableName, -1); }

	/// Called when the current index of the combo box changes, indicating new sample selected
	void onCBCurrentIndexChanged(int);

	/// Called when someone clicks a 'close button' on a sample to delete it
	void onSampleDeleteButtonClicked(const QModelIndex& index);

signals:
	/// Emitted when the currently-selected sample changes. \c id is -1 if there is no valid sample selected
	void currentSampleChanged(int id);
	void currentSamplePointerChanged(const AMSamplePre2013 *sample);

protected:
	/// The currently-selected sample
	AMSamplePre2013* sample_;

	// UI components:
	QComboBox* sampleSelector_;
	QLineEdit* sampleName_, *sampleElements_;
	QLabel* sampleDate_;

	/// Flag to indicate that a refresh is required, because the database was updated.
	bool refreshScheduled_;
	/// Specifies which sample id needs to be refreshed (-1 for all)
	int refreshId_;

	/// The database searched by this editor
	AMDatabase* db_;

	/// This maintains a map from sample IDs to combobox indices. It's faster than searching through all the items in the model combobox to find the index with a given id.
	QHash<int, int> sampleId2Index_;

	/// This state flag is set when the current sample is a brand-newly added sample, and should be edited immediately.
	bool newSampleActive_;


	/// Holds the name of the sample table in the database
	QString sampleTableName_;

	/// Parse a string of elements, and return a list of atomic numbers.  The elements can be separated by whitespace, commas, and semicolons... and can be either the full element name or just the symbol. Case doesn't matter.
	QList<int> parseElementString(const QString& elementString);

	/// Layout used to hold the GUI elements
	QVBoxLayout* vl_;

};

#endif // AMSAMPLEPRE2013EDITOR_H
