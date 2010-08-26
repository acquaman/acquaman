#ifndef AMSAMPLEEDITOR_H
#define AMSAMPLEEDITOR_H

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>

#include "dataman/AMSample.h"
#include "dataman/AMDatabase.h"
#include <QHash>


/// The AMSampleEditor class provides a widget that lets the user choose a sample from the list of samples, and edit characteristics of the sample.  The changes made to the sample are saved back to the database immediately.
class AMSampleEditor : public QWidget
{
Q_OBJECT
public:
	/// Default constructor
	explicit AMSampleEditor(AMDatabase* db, QWidget *parent = 0);

	/// Returns the id of the currently-selected sample, or -1 if it's a non-existent/invalid sample
	int currentSample() const;

signals:

public slots:
	/// Set the currently-selected sample, by ID
	void setCurrentSample(int id);

	/// Create a new sample and set it as the currently-selected sample
	void createNewSample();

protected slots:
	/// Call this to refresh the list of samples in the ComboBox from the database
	void refreshSamples();

	/// This is called when a row in the database is updated:
	void onDatabaseUpdated(const QString& tableName, int id);
	/// In the future, the onDatabaseCreatedd() and onDatabaseRemoved() functions could be used to handle specific situations/optimizations. Right now they just call onDatabaseUpdated().
	void onDatabaseCreated(const QString& tableName, int id) { onDatabaseUpdated(tableName, id); }
	void onDatabaseRemoved(const QString& tableName, int id) { onDatabaseUpdated(tableName, id); }

	/// Called when the current index of the combo box changes, indicating new sample selected
	void onCBCurrentIndexChanged(int);

signals:
	/// Emitted when the currently-selected sample changes. \c id is -1 if there is no valid sample selected
	void currentSampleChanged(int id);

protected:
	/// The currently-selected sample
	AMSample* sample_;

	// UI components:
	QComboBox* sampleSelector_;
	QLineEdit* sampleName_, *sampleElements_;
	QLabel* sampleDate_, *sampleTime_;

	/// Flag to indicate that a refresh is required, because the database was updated.
	bool refreshScheduled_;

	/// The database searched by this editor
	AMDatabase* db_;

	/// This maintains a map from sample IDs to combobox indices. It's faster than searching through all the items in the model combobox to find the index with a given id.
	QHash<int, int> sampleId2Index_;

	/// This state flag is set when the current sample is a brand-newly added sample, and should be edited immediately.
	bool newSampleActive_;


};

#endif // AMSAMPLEEDITOR_H
