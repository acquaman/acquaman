#ifndef AMSAMPLEEDITOR_H
#define AMSAMPLEEDITOR_H

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>

#include "dataman/AMSample.h"
#include "dataman/AMDatabase.h"

/// The AMSampleEditor class provides a widget that lets the user choose a sample from the list of samples, and edit characteristics of the sample.  The changes made to the sample are saved back to the database immediately.
class AMSampleEditor : public QWidget
{
Q_OBJECT
public:
	/// Default constructor
	explicit AMSampleEditor(AMDatabase* db, QWidget *parent = 0);

	/// Returns the id of the currently-selected sample
	int currentSample() const;

signals:

public slots:
	/// Set the currently-selected sample, by ID
	void setCurrentSample(int id);

protected slots:
	/// Call this to refresh the list of samples in the ComboBox from the database
	void refreshSamples();

	/// This is called when a row in the database is updated:
	void onDatabaseUpdated(const QString& tableName, int id);
	/// This is called when a row in the database is created:
	void onDatabaseCreated(const QString& tableName, int id);
	void onDatabaseRemoved(const QString& tableName, int id);

	/// Called when the current index of the combo box changes, indicating new sample selected
	void onCBCurrentIndexChanged(int);

signals:
	/// Emitted when the currently-selected sample changes
	void currentSampleChanged(int id);

protected:
	/// The currently-selected sample
	AMSample sample_;

	// UI components:
	QComboBox* sampleSelector_;
	QLineEdit* sampleName_, *sampleElements_;
	QLabel* sampleDate_, *sampleTime_;

	/// Flag to indicate that a refresh is required, because the database was updated.
	bool refreshRequired_;

	/// The database searched by this editor
	AMDatabase* db_;


};

#endif // AMSAMPLEEDITOR_H
