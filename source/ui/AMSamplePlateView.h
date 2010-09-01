#ifndef AMSAMPLEPLATEVIEW_H
#define AMSAMPLEPLATEVIEW_H

#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>
#include <QFrame>
#include <QScrollArea>
#include <QPushButton>
#include <QMouseEvent>
#include <QIcon>
#include <QScrollBar>
#include <QDebug>
#include <QListView>
#include <QLineEdit>
#include <QPropertyAnimation>


#include "dataman/AMSamplePlate.h"
#include "beamline/SGMBeamline.h"
#include "dataman/AMDatabase.h"
#include "AMDetailedItemDelegate.h"

#include "ui_AMSamplePlateSelector.h"

class AMSampleListView;
class AMSamplePositionItemView;
class AMSamplePositionItemExpandingAdder;

/// This class provides a widget that can be used to select and load a sample plate object, out of the available set of user-defined sample plates. It can also be used to create new sample plates.
/*! This widget operates on an "active" or "current" sample plate, and causes it to be re-loaded to become a different plate when activated by the user.  This active plate can be retrieved with samplePlate().  If an AMSamplePlate pointer is provided in the constructor, we use this as the active plate; otherwise we create an internal AMSamplePlate object.  You can use the complete AMSamplePlate interface, including the changeSamplePlate(newId) slot and samplePlateChanged(newId) signal, on the object returned with samplePlate().
  */
class AMSamplePlateSelector : public QWidget, private Ui::AMSamplePlateSelector {
	Q_OBJECT
public:
	explicit AMSamplePlateSelector(AMSamplePlate* sourcePlate = 0, QWidget* parent = 0);

	/// Returns the active sample plate object
	AMSamplePlate* samplePlate() { return plate_; }

	/// Convenience function for samplePlate()->id()
	int samplePlateId() const { return plate_->id(); }
	/// Convenience function for samplePlate()->changeSamplePlate(newId);
	void changeSamplePlate(int newId) { plate_->changeSamplePlate(newId); }

signals:
	/// Convenience signal, connected to samplePlate()->samplePlateChanged(bool isValid)
	void samplePlateChanged(bool);





protected slots:
	/// Called when the active sample plate is changed substantially (ie: reloaded out of the db)... we need to refresh what's currently highlighted in the list, and refresh the name, date, and notes display
	void onSamplePlateChanged(bool isValid);

	/// called when an option in the list of sample plates is activated by the user
	void onComboBoxActivated(int index);

	/// Fill the list of available sample plates from the database. Make the first entry an 'Add New Sample Plate...' option
	void populateSamplePlates();
	/// Start the process of getting a name from the user to create a new sample plate
	void startCreatingNewPlate();
	/// This is called when editing is finished on the name lineedit... during a "Create New Plate" process.
	void onFinishCreatingNewPlate();

	/// Called when an item in the database is updated. Need to watch for changes in the sample plate table.
	void onDatabaseUpdated(const QString& tableName, int id);
	/// Called when an item in the database is created. Need to watch for changes in the sample plate table.
	void onDatabaseCreated(const QString& tableName, int id);
	/// Called when an item in the database is removed. Need to watch for changes in the sample plate table.
	void onDatabaseRemoved(const QString& tableName, int id);

	/// Call this to request a refresh of the list of sample plates
	void schedulePlateRefresh();

	// UI event responders:
	/// Called when the user finishes editing the text in a plate name, or the notes box
	void onPlateEditingFinished() { plate_->storeToDb(plate_->database()); }
	/// Called when the user edits the name of the plate
	void onNameEdited(const QString& name) { plate_->setName(name); }
	/// Called when the user edits the notes for the plate
	void onNotesEdited() { /*! \todo plate_->setNotes(notesEditor_.toPlainText()); */ }



protected:
	Ui::AMSamplePlateSelector ui_;

	bool plateRefreshRequired_;
	AMSamplePlate* plate_;

};

class AMSamplePlateView : public QWidget
{
Q_OBJECT
public:
	/// Create a sample plate editor, to modify/view an existing sample plate \c existingPlate.  If \c existingPlate is 0, it will create a new sample plate to work with.
	explicit AMSamplePlateView(AMSamplePlate* existingPlate = 0, QWidget *parent = 0);



public slots:
	void setManipulator(AMControlSet *manipulator);

protected slots:
	void changeSamplePlate(int newPlateId);

	void onSampleTableItemUpdated(QString tableName, int id);
	void onSampleTableItemCreated(QString tableName, int id);
	void onSampleTableItemRemoved(QString tableName, int id);

	void refreshSamples();

protected:
	AMSamplePlateSelector* samplePlateSelector_;
	AMSampleListView *sampleListView_;
	QVBoxLayout *vl_;

	AMSamplePlate* samplePlate_;
	AMControlSet *manipulator_;

	QStandardItemModel *sampleTableModel_;
	bool sampleRefreshScheduled_;
	QList<int> sampleRefreshIDs_;
	QList<int> sampleRefreshInstructions_;


};

class AMSampleListView : public QFrame
{
Q_OBJECT
public:
	AMSampleListView(AMSamplePlate *samplePlate, QStandardItemModel *sampleTableModel, QWidget *parent = 0);

public slots:
	void setManipulator(AMControlSet *manipulator);

signals:

protected slots:
	void addNewSampleToPlate(int id);
	void onSamplePositionChanged(int index);
	void onSamplePositionAdded(int index);
	void onSamplePositionRemoved(int index);

protected:
	QSize sizeHint() const;

protected:
	AMSamplePlate *samplePlate_;
	AMControlSet *manipulator_;
	QStandardItemModel *sampleTableModel_;

	QScrollArea *sa_;
	QVBoxLayout *il_;
	QFrame *saf_;
	//QPushButton *adder_;
	AMSamplePositionItemExpandingAdder *adder_;
};

class AMSamplePositionItemView : public QFrame
{
Q_OBJECT
public:
	AMSamplePositionItemView(AMSamplePosition *samplePosition, QStandardItemModel *sampleTableModel, AMControlSet *manipulator = 0, int index = -1, QWidget *parent = 0);
	int index();
	AMControlSet* manipulator();

public slots:
	void setIndex(int index);
	void setManipulator(AMControlSet* manipulator);

protected slots:
	bool onSavePositionClicked();
	bool onRecallPositionClicked();
	bool onSampleBoxIndexChanged(int index);
	bool onSampleNameChanged();

	void updateLook();
	void onSamplePositionUpdate(int index);
	void defocusItem();

protected:
	void mousePressEvent(QMouseEvent *event);

protected:
	AMSamplePosition *samplePosition_;

	AMControlSet *manipulator_;
	QStandardItemModel *sampleTableModel_;
	bool ignoreNameChanged_;
	int index_;
	bool inFocus_;

	QHBoxLayout *hl_;
	QVBoxLayout *vl_;
	QLabel *indexLabel_;
	QComboBox *sampleBox_;
	QLabel *positionLabel_;
	QPushButton *savePositionButton_;
	QPushButton *recallPositionButton_;
};

class AMTrickComboBox;

class AMSamplePositionItemExpandingAdder : public QFrame
{
Q_OBJECT
public:
	AMSamplePositionItemExpandingAdder(QStandardItemModel *sampleTableModel, QWidget *parent = 0);
	const bool expanded() const;

public slots:
	void resetAdder();

signals:
	void sampleToAddChosen(int id);

protected slots:
	void onMarkNewButtonClicked();
	void onGoNewButtonClicked();
	void onGoExistingButtonClicked();

	void shrinkBack();
	void switchBoxes();

protected:
	QStandardItemModel *sampleTableModel_;

	QPushButton *markNewButton_;
	QLineEdit *newNameEdit_;
	QLabel *newNameLabel_;
	QComboBox *chooseExistingBox_;
	//QComboBox *emptyChooseExistingBox_;
	AMTrickComboBox *emptyChooseExistingBox_;
	QPushButton *goNewButton_;
	QPushButton *goExistingButton_;
	QPushButton *cancelButton_;

	QGridLayout *gl_;
};

class AMTrickComboBox : public QComboBox
{
Q_OBJECT
public:
	AMTrickComboBox(QWidget *parent = 0);

signals:
	void clicked();

protected:
	void mousePressEvent(QMouseEvent *e);
};

#endif // AMSAMPLEPLATEVIEW_H
