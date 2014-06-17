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


#ifndef AMSAMPLEPLATEVIEWPRE2013_H
#define AMSAMPLEPLATEVIEWPRE2013_H



#include <QListView>
#include <QPushButton>
#include <QStyledItemDelegate>


#include "dataman/AMSamplePlatePre2013.h"
#include "dataman/database/AMDatabase.h"

#include "ui_AMSamplePlateSelector.h"
#include "ui/dataman/AMSamplePre2013Editor.h"

#include "util/AMDeferredFunctionCall.h"
#include "util/AMDateTimeUtils.h"


/* Classes

AMSamplePlateItemModel:
 - exposes information from the sample plate's items (positions) as a QAbstractListModel

AMSamplePlateItemDelegate: custom drawing for samples

AMSamplePositionViewActionsWidget: editor widget provided by the delegate; placed on top of AMSamplePosition items in view; with buttons to mark, move, remove

*/

class AMSamplePlatePre2013ItemModel : public QAbstractListModel {

	Q_OBJECT

public:
	/// Constructor requires a valid \c plate to expose as a QAbstractListModel. The sample/position pairs (AMSamplePosition) will provide the data for the items in the list.
	AMSamplePlatePre2013ItemModel(AMSamplePlatePre2013* plate, QObject* parent = 0);

	/// Reimplemented from QAbstractListModel
	int rowCount(const QModelIndex &parent) const {
		if(parent.isValid())
			return 0;

		return plate_->count();
	}

	/// Reimplemented from QAbstractListModel. Exposes data from the samples (DisplayRole and EditRole: sample name; DecorationRole: picture, DateTimeRole and DescriptionRole: creation date/time; AM::UserRole: sample elements, as a string of element symbols; AM::UserRole + 1: string of positions and labels; AM::UserRole+2: facility name)
	QVariant data(const QModelIndex &index, int role) const {
		if(index.parent().isValid() || index.column() > 0)
			return QVariant();

		switch(role) {
		case Qt::DisplayRole:
		case Qt::EditRole:
			return sampleName(index.row());
		case Qt::DecorationRole:
			return QApplication::style()->standardIcon(QStyle::SP_FileIcon);	/// \todo implement pictures
		case AM::DateTimeRole:
			return sampleDateTime(index.row());
		case AM::DescriptionRole:
			return QString("created ") +  AMDateTimeUtils::prettyDateTime(sampleDateTime(index.row()));

		case AM::UserRole:
			return sampleElements(index.row());
		case AM::UserRole + 1:
			return positionsString(index.row());
		case AM::UserRole + 2:
			return facilityName(index.row());

			/// Returns a pointer to the AMSamplePosition object here. Can access all the additional information directly.
		case AM::PointerRole:
			return QVariant::fromValue((QObject*)&(plate_->at(index.row())));

		default:
			return QVariant();
		}
	}

	Qt::ItemFlags flags(const QModelIndex &index) const {
		Q_UNUSED(index)

		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role) const {
		Q_UNUSED(section)

		if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
			return QString("Samples");
		else
			return QVariant();
	}


	/// Return the name of the sample
	QString sampleName(int index) const {
		qDebug() << "plate_->count() is " << plate_->count() << " index is " << index;
		if (index < plate_->count() && plate_->at(index).sample())
			return plate_->at(index).sample()->name();

		return "";
//		return getCachedSample(index).name();
	}

	/// Return the creation date/time of a sample
	QDateTime sampleDateTime(int index) const {
		if (index < plate_->count() && plate_->at(index).sample())
			return plate_->at(index).sample()->dateTime();

		return QDateTime();
//		return getCachedSample(index).dateTime();
	}

	/// Return the elements in a sample, as a string: ex: "B, N, Cl"
	QString sampleElements(int index) const {
		if (index < plate_->count() && plate_->at(index).sample())
			return plate_->at(index).sample()->elementString();

		return "";
//		return getCachedSample(index).elementString();
	}

	/// Return the sample position, formatted as a string: ex: X: 33mm Y: 47.9mm Z: -92mm
	QString positionsString(int index) const;

	/// Return the facility name corresponding to the facility ID:
	QString facilityName(int index) const {
		return QString("%1").arg(plate_->at(index).facilityId());	/// \todo Map to name
	}

protected slots:
	/// Received from AMSamplePlate. Used to implement beginInsertRows.
	void onSamplePositionAboutToBeAdded(int index);

	/// Received from AMSamplePlate. Used to implement endInsertRows.
	void onSamplePositionAdded(int index);

	/// Received from AMSamplePlate. Used to implement beginRemoveRows.
	void onSamplePositionAboutToBeRemoved(int index);

	/// Received from AMSamplePlate. Used to implement endRemoveRows.
	void onSamplePositionRemoved(int index);

	/// Received fomr AMSamplePlate. Used to implement dataChanged().
	void onSamplePositionChanged(int index);


	/// Watches the database for update signals... To see if sample information changes for one of our existing sample ids...
	void onDatabaseItemUpdated(const QString& tableName, int id);
	void onDatabaseItemRemoved(const QString& tableName, int id);

protected:

	/// Access a cached sample object, ensuring it is loaded from the database and up-to-date
//	const AMSamplePre2013& getCachedSample(int index) const;

	AMSamplePlatePre2013* plate_;
//	mutable QList<AMSamplePre2013> cachedSamples_;

	QString sampleTableName_;
};



/// This delegate draws samples and their position (from AMSamplePlateItemModel) in a list view.  You can use it by calling setItemDelegate(new AMSamplePlateItemDelegate) on your QListView.
class AMSamplePlatePre2013ItemDelegate : public QStyledItemDelegate {
	Q_OBJECT
public:

	AMSamplePlatePre2013ItemDelegate(QObject* parent = 0);
	virtual ~AMSamplePlatePre2013ItemDelegate();

	// Reimplemented delegate functions
	////////////////////////
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	/// Don't do anything to set back the model data...
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	///////////////////////////

signals:
	/// Emitted when the "Mark" button is pressed on a specific row in the list
	void rowMarkPressed(int row);
	/// Emitted when the "Move To" button is pressed on a specific row in the list
	void rowMoveToPressed(int row);
	/// Emitted when the "Remove" button is pressed on a specific row in the list
	void rowRemovePressed(int row);

	/// Emitted when the custom context menu for "Additional Information" is activated
	void additionalInformationRequested(int row);

protected:
	/// disable the editor closing / committing?
	bool eventFilter(QObject *object, QEvent *event) {
		// qdebug() << "Event:" << event << "Type:" << event->type();

		if(event->type() == QEvent::FocusOut)
			return false;

		return QStyledItemDelegate::eventFilter(object, event);
	}
};

/// This class provides a widget that can be used to select and load a sample plate object, out of the available set of user-defined sample plates. It can also be used to create new sample plates.
/*! This widget operates on an "active" or "current" sample plate, and causes it to be re-loaded to become a different plate when activated by the user.  This active plate can be retrieved with samplePlate().  If an AMSamplePlate pointer is provided in the constructor, we use this as the active plate; otherwise we create an internal AMSamplePlate object.
  */
class AMSamplePlatePre2013Selector : public QWidget, private Ui::AMSamplePlateSelector {
	Q_OBJECT
public:
	explicit AMSamplePlatePre2013Selector(AMSamplePlatePre2013* sourcePlate = 0, QWidget* parent = 0);

	/// Returns the active sample plate object
	AMSamplePlatePre2013* samplePlate() { return plate_; }

	/// Convenience function for samplePlate()->id()
	int samplePlateId() const { return plate_->id(); }

	/// reload the sample plate out of the database to become a different sample plate
	void changeSamplePlate(int newId) { plate_->loadFromDb(AMDatabase::database("user"), newId); }

signals:
	/// Emitted when the sample plate is changed
	void samplePlateChanged();

protected slots:
	/// Called when the active sample plate is changed substantially (ie: reloaded out of the db)... we need to refresh what's currently highlighted in the list, and refresh the name, date, and notes display
	void onSamplePlateChanged(/*bool isValid*/);

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



	// UI event responders:
	/// Called when the user finishes editing the text in a plate name, or the notes box
	void onPlateEditingFinished() { plate_->storeToDb(plate_->database()); }
	/// Called when the user edits the name of the plate
	void onNameEdited(const QString& name) { plate_->setName(name); }
	/// Called when the user edits the notes for the plate
	void onNotesEdited() { /*! \todo plate_->setNotes(notesEditor_.toPlainText()); */ }



protected:
	Ui::AMSamplePlateSelector ui_;

	AMSamplePlatePre2013* plate_;

	/// Deferred call to re-load all the available plates from the database
	AMDeferredFunctionCall plateRefreshScheduler_;

	/// name of the sample plate table in the database
	QString samplePlateTableName_;

};





class AMSampleManipulator;

class AMSamplePlatePre2013View : public QWidget
{
	Q_OBJECT
public:
	/// Create a sample plate editor, to modify/view an existing sample plate \c existingPlate.  If \c existingPlate is 0, it will create a new sample plate to work with.
	explicit AMSamplePlatePre2013View(AMSamplePlatePre2013* existingPlate = 0, QWidget *parent = 0);

	/// Returns the current sample plate
	AMSamplePlatePre2013* samplePlate() { return samplePlate_; }

public slots:
	void setManipulator(AMSampleManipulator *manipulator) { manipulator_ = manipulator; }

signals:
	void newSamplePlateSelected();

protected slots:

	void onAddSampleButtonClicked();

	/// \todo Should these be handled by a controller-type object outside of the UI class?

	/// called by the delegate when the editor buttons (Mark, Move To, Remove) are clicked
	void onRowMarkPressed(int row);
	/// called by the delegate when the editor buttons (Mark, Move To, Remove) are clicked
	void onRowMoveToPressed(int row);
	/// called by the delegate when the editor buttons (Mark, Move To, Remove) are clicked
	void onRowRemovePressed(int row);

	void onAdditionalInformationRequested(int row);
	void onAdditionalPlateInformationRequested();

	void onSamplePlateChanged();

protected:
	/// Widget to select and swap current sample plate
	AMSamplePlatePre2013Selector* samplePlateSelector_;
	/// List view (with custom delegate) to see the samples and positions on the plate
	QListView *sampleListView_;
	/// A widget to select a sample
	AMSamplePre2013Editor* sampleSelector_;

	QPushButton *moreInformationButton_;

	/// A button to add a sample / current position to the sample plate
	QPushButton* addSampleButton_;
	/// Layout for GUI items
	QVBoxLayout *vl_;

	/// Pointer to the sample plate object we display / change
	AMSamplePlatePre2013* samplePlate_;
	/// Manipulator used to move samples into position / grab current positions
	AMSampleManipulator *manipulator_;

	/// A model that wraps an AMSamplePlate object for exposing as a list view
	AMSamplePlatePre2013ItemModel* samplePlateModel_;
};

#include "MPlot/MPlotRectangle.h"
#include "MPlot/MPlotPoint.h"
class AMSamplePlatePre2013PositionInfo : public QObject
{
Q_OBJECT
public:
	AMSamplePlatePre2013PositionInfo(AMSamplePlatePre2013 *samplePlate, int index, const QString &description, MPlotAxisScale *horizontalScale, MPlotAxisScale *verticalScale, QObject *parent = 0);

	MPlotRectangle* area() const;
	MPlotPoint* position() const;

	QString description() const;
	QString errors() const;

	QColor getAreaColor(bool isHighlighted = false) const;
	QColor getPositionColor(bool isHighlighted = false) const;

protected:
	AMSamplePlatePre2013 *samplePlate_;
	int index_;
	QString description_;

	MPlotAxisScale *horizontalScale_;
	MPlotAxisScale *verticalScale_;
	MPlotRectangle *area_;
	MPlotPoint *position_;
};

class AMSamplePlatePre2013PositionInfoView : public QFrame
{
Q_OBJECT
public:
	AMSamplePlatePre2013PositionInfoView(AMSamplePlatePre2013PositionInfo *positionInfo, QWidget *parent = 0);

public slots:
	void setHighlighted(bool isHighlighted);

signals:
	void becameHighlighted(bool isHighlighted);

protected:
	virtual void mouseReleaseEvent(QMouseEvent *e);

protected:
	QLabel *nameLabel_;
	QLabel *errorsLabel_;

	QPalette normalPalette_;
	QPalette highlightedPalette_;

	bool isHighlighted_;

	AMSamplePlatePre2013PositionInfo *positionInfo_;
};

class AMSamplePlatePre2013PositionInfoListView : public QWidget
{
Q_OBJECT
public:
	AMSamplePlatePre2013PositionInfoListView(QList<AMSamplePlatePre2013PositionInfoView*> infoViews, QWidget *parent = 0);

protected slots:
	void onSamplePlatePositionInfoViewBecameHighlighted(bool isHighlighted);

protected:
	QList<AMSamplePlatePre2013PositionInfoView*> infoViews_;
};

class AMScrollViewWidget : public QWidget
{
Q_OBJECT
public:
	AMScrollViewWidget(QLayout *layout, QWidget *parent = 0);
};

#include "MPlot/MPlotWidget.h"
class AMSamplePlatePre2013AdditionalInformationView : public QWidget
{
Q_OBJECT
public:
	AMSamplePlatePre2013AdditionalInformationView(AMSamplePlatePre2013 *samplePlate, AMSamplePlatePre2013ItemModel* samplePlateModel, QWidget *parent = 0);

protected:
	AMSamplePlatePre2013 *samplePlate_;
	AMSamplePlatePre2013ItemModel* samplePlateModel_;

	QList<AMSamplePlatePre2013PositionInfo*> positionInfos_;
	QList<AMSamplePlatePre2013PositionInfoView*> positionInfoViews_;

	MPlotWidget *imageView_;
	MPlot *imagePlot_;


};

#include <QDialog>
#include <QDoubleSpinBox>
class AMSamplePositionPre2013ManuallyEnterView : public QDialog
{
Q_OBJECT
public:
	AMSamplePositionPre2013ManuallyEnterView(QWidget *parent = 0);

signals:
	void finished(double upDown, double inOut, double upStDownSt, double rot);

protected slots:
	void onCancelButtonClicked();
	void onApplyButtonClicked();

	void hideAndFinish();

protected:
	virtual void closeEvent(QCloseEvent *);

protected:
	QPushButton *applyButton_;
	QPushButton *cancelButton_;

	QDoubleSpinBox *upDownDSBox_;
	QDoubleSpinBox *inOutDSBox_;
	QDoubleSpinBox *upStDownStDSBox_;
	QDoubleSpinBox *rotDSBox_;

	double upDown_, inOut_, upStDownSt_, rot_;
};

class AMSamplePositionPre2013AdditionalInformationView : public QDialog
{
Q_OBJECT
public:
	AMSamplePositionPre2013AdditionalInformationView(AMSampleManipulator *manipulator, AMSamplePositionPre2013 *samplePosition, QWidget *parent = 0);

signals:
	void finished();

protected slots:
	void onTopLeftSetFromManipulator();
	void onBottomRightSetFromManipulator();
	void onTopLeftManuallyEnterClicked();
	void onBottomRightManuallyEnterClicked();

	void onTopLeftManualEnterFinished(double upDown, double inOut, double upStDownSt, double rot);
	void onBottomRightManualEnterFinished(double upDown, double inOut, double upStDownSt, double rot);

	void onCancelButtonClicked();
	void onApplyButtonClicked();

	void hideAndFinish();

protected:
	virtual void closeEvent(QCloseEvent *);

	void setTopLeftText();
	void setBottomRightText();
	void checkValidity();

protected:
	AMSampleManipulator *manipulator_;
	AMSamplePositionPre2013 *samplePosition_;

	AMControlInfoList originalTopLeft_;
	AMControlInfoList originalBottomRight_;

	QLabel *topLeftLabel_;
	QLabel *bottomRightLabel_;

	QPushButton *setTopLeftFromManipulatorButton_;
	QPushButton *manuallyEnterTopLeftButton_;
	QPushButton *setBottomRightFromManipulatorButton_;
	QPushButton *manuallyEnterBottomRightButton_;

	QPushButton *applyButton_;
	QPushButton *cancelButton_;
	QLabel *errorLabel_;

	AMSamplePositionPre2013ManuallyEnterView *enterTopLeftDialog_;
	AMSamplePositionPre2013ManuallyEnterView *enterBottomRightDialog_;
};

#endif // AMSAMPLEPLATEVIEWPRE2013_H
