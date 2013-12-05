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


#include "AMSamplePlateView.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "ui/dataman/AMSamplePositionViewActionsWidget.h"

#include "ui/AMDetailedItemDelegate.h"

#include "beamline/AMSampleManipulator.h"
#include "util/AMErrorMonitor.h"

AMSamplePlateItemModel::AMSamplePlateItemModel(AMSamplePlate* plate, QObject* parent) :
	QAbstractListModel(parent)
{
	plate_ = plate;

	sampleTableName_ = AMDbObjectSupport::s()->tableNameForClass<AMSample>();

	connect(plate_, SIGNAL(samplePositionAboutToBeAdded(int)), this, SLOT(onSamplePositionAboutToBeAdded(int)));
	connect(plate_, SIGNAL(samplePositionAdded(int)), this, SLOT(onSamplePositionAdded(int)));
	connect(plate_, SIGNAL(samplePositionAboutToBeRemoved(int)), this, SLOT(onSamplePositionAboutToBeRemoved(int)));
	connect(plate_, SIGNAL(samplePositionRemoved(int)), this, SLOT(onSamplePositionRemoved(int)));
	connect(plate_, SIGNAL(samplePositionChanged(int)), this, SLOT(onSamplePositionChanged(int)));

	connect(AMDatabase::database("user"), SIGNAL(updated(QString,int)), this, SLOT(onDatabaseItemUpdated(QString,int)), Qt::QueuedConnection);
	connect(AMDatabase::database("user"), SIGNAL(removed(QString,int)), this, SLOT(onDatabaseItemRemoved(QString,int)), Qt::QueuedConnection);

	cachedSamples_.reserve(plate_->count());
	for(int i=plate_->count()-1; i>=0; i--)
		cachedSamples_ << AMSample();
}


// Return the sample position, formatted as a string: ex: X: 33mm Y: 47.9mm Z: -92mm
QString AMSamplePlateItemModel::positionsString(int index) const {

	const AMControlInfoList& positions = plate_->at(index).position();

	QStringList sl;
	for(int i=0; i<positions.count(); i++) {
		const AMControlInfo& pos = positions.at(i);

		//QString s = QString("%1: %2%3").arg(pos.name()).arg(pos.value()).arg(pos.units());
		QString s;
		if(pos.contextKnownDescription() != "")
			s = QString("%1: %2%3").arg(pos.contextKnownDescription()).arg(pos.value(), 0, 'g', 3).arg(pos.units());
		else if(pos.description() != "")
			s = QString("%1: %2%3").arg(pos.description()).arg(pos.value(), 0, 'g', 3).arg(pos.units());
		else
			s = QString("%1: %2%3").arg(pos.name()).arg(pos.value(), 0, 'g', 3).arg(pos.units());

		sl << s;
	}

	return sl.join("  ");

}

// Received from AMSamplePlate. Used to implement beginInsertRows.
void AMSamplePlateItemModel::onSamplePositionAboutToBeAdded(int index) {
	beginInsertRows(QModelIndex(), index, index);
	cachedSamples_.insert(index, AMSample());
}

// Received from AMSamplePlate. Used to implement endInsertRows.
void AMSamplePlateItemModel::onSamplePositionAdded(int index) {
	Q_UNUSED(index)

	endInsertRows();
}

// Received from AMSamplePlate. Used to implement beginRemoveRows.
void AMSamplePlateItemModel::onSamplePositionAboutToBeRemoved(int index) {

	beginRemoveRows(QModelIndex(), index, index);
}

// Received from AMSamplePlate. Used to implement endRemoveRows.
void AMSamplePlateItemModel::onSamplePositionRemoved(int index) {

	cachedSamples_.removeAt(index);
	endRemoveRows();
}

// Access a cached sample object, ensuring it is loaded from the database and up-to-date
const AMSample& AMSamplePlateItemModel::getCachedSample(int index) const {
	int correctId = plate_->at(index).sampleId();

	// we can tell if this one's been loaded by looking at its id. If it's 0 (or a wrong id), it's not the sample info we want.
	if(cachedSamples_.at(index).id() != correctId) {
		if(!cachedSamples_[index].loadFromDb(AMDatabase::database("user"), correctId)) {
			cachedSamples_[index].setName("[Unknown Sample]");
		}
	}

	return cachedSamples_.at(index);
}


// Watches the database for update signals... To see if sample information changes for one of our existing sampleIds...
void AMSamplePlateItemModel::onDatabaseItemUpdated(const QString &tableName, int id) {
	if(tableName != sampleTableName_)
		return;

	// ok, this update was on the sample table. Check all of our cached samples to see if it was their id that was updated.
	for(int i=0; i<cachedSamples_.count(); i++) {
		if(cachedSamples_.at(i).id() == id) {
			// it was this sample! reload from db
			if(!cachedSamples_[i].loadFromDb(AMDatabase::database("user"), id)) {
				cachedSamples_[i].setName("[Unknown Sample]");
			}
			emit dataChanged(index(i), index(i));
		}
	}
}

void AMSamplePlateItemModel::onDatabaseItemRemoved(const QString &tableName, int id) {
	if(tableName != sampleTableName_)
		return;

	// ok, this update was on the sample table. Check all of our cached samples to see if it was their id that was deleted.
	for(int i=0; i<cachedSamples_.count(); i++) {
		if(cachedSamples_.at(i).id() == id) {
			// it was this sample! Oh dear, It's gone.. what to do?
			cachedSamples_[i] = AMSample();
			cachedSamples_[i].setName("[Deleted Sample]");
			emit dataChanged(index(i), index(i));
		}
	}
}



AMSamplePlateItemDelegate::AMSamplePlateItemDelegate(QObject* parent) : QStyledItemDelegate(parent) {

}

AMSamplePlateItemDelegate::~AMSamplePlateItemDelegate() {

}

void AMSamplePlateItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {

	QString sampleName(index.data(Qt::DisplayRole).toString());
	QString elementString(index.data(AM::UserRole).toString());
	if(!elementString.isEmpty()) {
		elementString.prepend("(");
		elementString.append(")");
	}
	QString createdString(index.data(AM::DescriptionRole).toString());
	QString positionsString(index.data(AM::UserRole+1).toString());
	if(positionsString.isEmpty()) {
		positionsString = "[Unknown sample position]";
	}

	QStyleOptionViewItemV4 opt(option);
	initStyleOption(&opt, index);


	QStyle* sty = QApplication::style();

	// Draw the background: (this will handle selection for us. You can also probe selection directly with opt.state & QStyle::State_Selected)
	sty->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter);


	int textStartingPoint = 0;
	// Do we have a pixmap available?
	QRect iconRect = opt.rect;
	iconRect.adjust(6, 0, 0, 0);

	if(opt.features & QStyleOptionViewItemV4::HasDecoration) {
		QPixmap p = opt.icon.pixmap(opt.decorationSize);
		sty->drawItemPixmap(painter, iconRect, Qt::AlignLeft | Qt::AlignVCenter, p);
		textStartingPoint += 6 + p.width() + 6;
	}

	// textRect starts 6px from the edge of the decoration, and is inset 6px on top and bottom and right. It's the full room we have available for text.
	QRect textRect = opt.rect;
	textRect.adjust( textStartingPoint, 6, -6, -6);



	QFont font = opt.font;
	font.setPointSize(font.pointSize()+4);
	painter->setFont(font);
	painter->setPen(opt.palette.text().color());
	QRect actualNameTextRect;
	painter->drawText(textRect, Qt::AlignLeft | Qt::AlignTop, sampleName, &actualNameTextRect);

	// Move over by the amount that we filled with text
	QRect elementTextRect(actualNameTextRect.right() + 6,
						  textRect.top(),
						  textRect.width()-actualNameTextRect.width() - 6,
						  actualNameTextRect.height());
	font.setPointSize(font.pointSize()-4);
	font.setItalic(true);
	font.setBold(true);
	painter->setFont(font);
	painter->setPen(Qt::darkGray);
	QFontMetrics fm(font);
	QString elidedElements = fm.elidedText(elementString, Qt::ElideRight, elementTextRect.width());
	painter->drawText(elementTextRect, Qt::AlignLeft | Qt::AlignBottom, elidedElements);

	QRect descriptionTextRect(textRect.left(), actualNameTextRect.bottom(), textRect.width(), textRect.height()-actualNameTextRect.height());
	font.setPointSize(font.pointSize()-2);
	font.setItalic(false);
	font.setBold(false);
	painter->setFont(font);
	painter->setPen(Qt::darkGray);
	painter->drawText(descriptionTextRect, Qt::AlignLeft | Qt::AlignTop, createdString);


	QFontMetrics fm2(font);
	QRect actualPositionsTextRect;
	painter->drawText(textRect, Qt::AlignLeft | Qt::AlignBottom, fm2.elidedText(positionsString, Qt::ElideMiddle, textRect.width()), &actualPositionsTextRect);

	int dividerLineY = actualPositionsTextRect.top() - 4;
	painter->drawLine(QPoint(textRect.left(), dividerLineY), QPoint(textRect.right(), dividerLineY));
}

QSize AMSamplePlateItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {

	QSize original = QStyledItemDelegate::sizeHint(option, index);
	return original.expandedTo(QSize(10, 66));
}

// create an editor widget with buttons to mark, move to, and remove this sample.
QWidget* AMSamplePlateItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {

	Q_UNUSED(option)

	AMSamplePositionViewActionsWidget* editor = new AMSamplePositionViewActionsWidget(index.row(), parent);

	connect(editor, SIGNAL(rowMarkPressed(int)), this, SIGNAL(rowMarkPressed(int)));
	connect(editor, SIGNAL(rowMoveToPressed(int)), this, SIGNAL(rowMoveToPressed(int)));
	connect(editor, SIGNAL(rowRemovePressed(int)), this, SIGNAL(rowRemovePressed(int)));

	connect(editor, SIGNAL(additionalInformationRequested(int)), this, SIGNAL(additionalInformationRequested(int)));

	return editor;
}

// no editor information is required... We get the row number from the constructor, and that's all we care about.
void AMSamplePlateItemDelegate::setEditorData(QWidget *iEditor, const QModelIndex &index) const {

	Q_UNUSED(iEditor)
	Q_UNUSED(index)
}

// Place the editor below the item...
void AMSamplePlateItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	Q_UNUSED(index)
	QRect itemRect = option.rect;
	QRect editorRect(itemRect.left(), itemRect.top()+itemRect.height()-26, itemRect.width(), 26);

	editor->setGeometry(editorRect);
}

// Don't do anything to set back the model data...
void AMSamplePlateItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {

	Q_UNUSED(editor)
	Q_UNUSED(model)
	Q_UNUSED(index)
}


AMSamplePlateSelector::AMSamplePlateSelector(AMSamplePlate* sourcePlate, QWidget *parent)
	: QWidget(parent) {

	samplePlateTableName_ = AMDbObjectSupport::s()->tableNameForClass<AMSamplePlate>();
	// Either use an external plate (if specified in sourcePlate), or make an internal one.
	plate_ = sourcePlate ? sourcePlate : new AMSamplePlate(this);

	ui_.setupUi(this);
	ui_.notesEditor->setObjectName("notesEditor");
	ui_.notesEditor->setStyleSheet("#notesEditor { background-image: url(:/notepadBackground.png); font: bold 15px \"Marker Felt\";}");

	AMDetailedItemDelegate* del = new AMDetailedItemDelegate(this);
	// Setting a new view fixes a grayed-menu-background drawing bug on mac
	QListView* lview = new QListView(this);
	lview->setItemDelegate(del);
	lview->setAlternatingRowColors(true);
	ui_.plateComboBox->setView(lview);

	ui_.notesEditor->setMaximumHeight(80);
	this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

	connect(&plateRefreshScheduler_, SIGNAL(executed()), this, SLOT(populateSamplePlates()));
	plateRefreshScheduler_.schedule();

	onSamplePlateChanged(/*plate_->valid()*/);

	connect(AMDatabase::database("user"), SIGNAL(updated(QString,int)), this, SLOT(onDatabaseUpdated(QString,int)), Qt::QueuedConnection);
	connect(AMDatabase::database("user"), SIGNAL(created(QString,int)), this, SLOT(onDatabaseCreated(QString,int)), Qt::QueuedConnection);
	connect(AMDatabase::database("user"), SIGNAL(removed(QString,int)), this, SLOT(onDatabaseRemoved(QString,int)), Qt::QueuedConnection);

	// GUI event connections
	connect(ui_.plateComboBox, SIGNAL(activated(int)), this, SLOT(onComboBoxActivated(int)));
	connect(ui_.nameEdit, SIGNAL(textEdited(QString)), this, SLOT(onNameEdited(QString)));
	connect(ui_.nameEdit, SIGNAL(editingFinished()), this, SLOT(onPlateEditingFinished()));
	connect(ui_.notesEditor, SIGNAL(textChanged()), this, SLOT(onNotesEdited()));
	connect(ui_.notesEditor, SIGNAL(editingFinished(int)), this, SLOT(onPlateEditingFinished()));

	ui_.notesHeaderButton->setChecked(false);
	ui_.notesEditor->setVisible(false);
	connect(ui_.notesHeaderButton, SIGNAL(clicked(bool)), ui_.notesEditor, SLOT(setVisible(bool)));

	// when our current sample plate is re-loaded out of the database, respond to update the GUI values
	connect(plate_, SIGNAL(loadedFromDb()), this, SLOT(onSamplePlateChanged()), Qt::QueuedConnection);
}

void AMSamplePlateSelector::onSamplePlateChanged(/*bool isValid*/) {
	if(/*isValid &&*/ plate_->id() > 0) {
		ui_.nameEdit->setText(plate_->name());
		ui_.createdLabel->setText(AMDateTimeUtils::prettyDateTime(plate_->dateTime()));
		ui_.notesEditor->setText("todo");

		// select as current in combo box. If this plate's id is not in the list, findData will return -1 and setCurrentIndex() will select nothing
		ui_.plateComboBox->setCurrentIndex( ui_.plateComboBox->findData(plate_->id(), AM::IdRole) );

		emit samplePlateChanged();
	}
	else {
		ui_.nameEdit->setText("[no sample plate selected]");
		ui_.createdLabel->setText("");
		ui_.notesEditor->setText("");

		ui_.plateComboBox->setCurrentIndex( -1 );
	}
}



void AMSamplePlateSelector::populateSamplePlates() {

	plateRefreshScheduler_.unschedule();

	ui_.plateComboBox->clear();
	ui_.plateComboBox->addItem("Create New Sample Plate...");
	ui_.plateComboBox->setItemData(0, -777);	// as a safety check, this makes sure that the "Sample Plate Id" of the "Create New..." item is an invalid index.

	QSqlQuery q2 = AMDatabase::database("user")->query();
	q2.prepare(QString("SELECT id,name,dateTime FROM %1 ORDER BY dateTime DESC").arg(samplePlateTableName_));
	q2.exec();
	int id;
	QString name;
	QDateTime dateTime;
	int index = 0;
	while(q2.next()) {
		index++;
		id = q2.value(0).toInt();
		name = q2.value(1).toString();
		dateTime = q2.value(2).toDateTime();
		ui_.plateComboBox->addItem(name);
		ui_.plateComboBox->setItemData(index, id, AM::IdRole);
		ui_.plateComboBox->setItemData(index, dateTime, AM::DateTimeRole);
		ui_.plateComboBox->setItemData(index, "created " + AMDateTimeUtils::prettyDateTime(dateTime), AM::DescriptionRole);
	}
	q2.finish();

	// highlight the item corresponding to our current plate index
	ui_.plateComboBox->setCurrentIndex( ui_.plateComboBox->findData(plate_->id(), AM::IdRole) );

}


void AMSamplePlateSelector::onComboBoxActivated(int index){

	// index is the activated index within the existingPlates_ combo box. It could mean...

	// nothing at all selected
	if(index < 0)
		return;

	// 'Create New Plate..." option selected
	else if(index == 0)
		startCreatingNewPlate();

	// In this case, someone has chosen an existing sample plate
	else {
		changeSamplePlate(ui_.plateComboBox->itemData(index, AM::IdRole).toInt());
	}
}


void AMSamplePlateSelector::startCreatingNewPlate() {
	// We're going to steal the name edit as the place for the user to enter the name of this new plate
	disconnect(ui_.nameEdit, SIGNAL(textEdited(QString)), this, SLOT(onNameEdited(QString)));
	disconnect(ui_.nameEdit, SIGNAL(editingFinished()), this, SLOT(onPlateEditingFinished()));
	ui_.nameEdit->setText("[New Sample Plate Name]");
	ui_.nameEdit->setFocus();
	ui_.nameEdit->selectAll();
	connect(ui_.nameEdit, SIGNAL(editingFinished()), this, SLOT(onFinishCreatingNewPlate()));

}

void AMSamplePlateSelector::onFinishCreatingNewPlate() {

	// restore usual connections for the name edit box...
	disconnect(ui_.nameEdit, SIGNAL(editingFinished()), this, SLOT(onFinishCreatingNewPlate()));

	ui_.nameEdit->clearFocus();
	connect(ui_.nameEdit, SIGNAL(textEdited(QString)), this, SLOT(onNameEdited(QString)));
	connect(ui_.nameEdit, SIGNAL(editingFinished()), this, SLOT(onPlateEditingFinished()));

	QString newName = ui_.nameEdit->text();
	if(newName.isEmpty())
		return;

	AMSamplePlate newPlate;
	newPlate.setName(ui_.nameEdit->text());
	if(newPlate.storeToDb(AMDatabase::database("user")))
		changeSamplePlate(newPlate.id());

	// this update will be picked up by our 'onDatabaseUpdated()' slot

}

void AMSamplePlateSelector::onDatabaseUpdated(const QString &tableName, int id) {

	if(tableName != samplePlateTableName_ )
		return;

	if(id < 1)	// invalid ids here mean need a full table update
		return onDatabaseCreated(tableName, id);

	int index = ui_.plateComboBox->findData(id, AM::IdRole);
	// do we have it?
	if(index == -1)
		return;

	/// \badcode Assuming here that it was this object that caused the update with a storeToDb, so the updated values are already inside it. This will be incorrect if someone else updated the database behind our back.  Maybe we need to implement some kind of locking, or some way to tell if this is "our" object's update or caused by another edit.
	if(id == plate_->id()) {
		// update combo box entries:
		ui_.plateComboBox->setItemData(index, plate_->name(), Qt::DisplayRole);
		ui_.plateComboBox->setItemData(index, plate_->dateTime(), AM::DateTimeRole);
		ui_.plateComboBox->setItemData(index, "created " + AMDateTimeUtils::prettyDateTime(plate_->dateTime()), AM::DescriptionRole);

		// update editor widgets
		ui_.nameEdit->setText(plate_->name());
		ui_.createdLabel->setText(AMDateTimeUtils::prettyDateTime(plate_->dateTime()));
		ui_.notesEditor->setText("todo");
	}
	else {
		AMSamplePlate p;
		/// \todo optimize to not require a full loadFromDb.  All we care about is the name and the dateTime... don't need to load all the samples and positions.
		p.loadFromDb(AMDatabase::database("user"), id);
		ui_.plateComboBox->setItemData(index, p.name(), Qt::DisplayRole);
		ui_.plateComboBox->setItemData(index, p.dateTime(), AM::DateTimeRole);
		ui_.plateComboBox->setItemData(index, "created " + AMDateTimeUtils::prettyDateTime(p.dateTime()), AM::DescriptionRole);
	}
}

void AMSamplePlateSelector::onDatabaseRemoved(const QString &tableName, int id) {
	if(tableName == samplePlateTableName_ ) {
		if(id == plate_->id()) {
			// deleted the current sample plate... This could be a problem.
			changeSamplePlate(plate_->id());	// will cause a failed load and trigger error handling as usual... hopefully
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -33, "Some part of the program has decided to delete the current sample plate. How rude!"));
		}
		plateRefreshScheduler_.schedule();
	}
}

void AMSamplePlateSelector::onDatabaseCreated(const QString &tableName, int id) {
	Q_UNUSED(id)
	if(tableName != samplePlateTableName_)
		return;

	plateRefreshScheduler_.schedule();
}

#include <QGroupBox>
AMSamplePlateView::AMSamplePlateView(AMSamplePlate *existingPlate, QWidget *parent) : QWidget(parent) {

	manipulator_ = 0;

	if(existingPlate)
		samplePlate_ = existingPlate;
	else
		samplePlate_ = new AMSamplePlate(this);

	QGroupBox* samplePlateGroupBox = new QGroupBox("Current sample plate");
	QGroupBox* samplesGroupBox = new QGroupBox("Samples");
	QVBoxLayout* vl1 = new QVBoxLayout();
	QVBoxLayout* vl2 = new QVBoxLayout();

	samplePlateSelector_ = new AMSamplePlateSelector(samplePlate_);
	moreInformationButton_ = new QPushButton("More Information");
	moreInformationButton_->setEnabled(false);
	sampleSelector_ = new AMSampleEditor(AMDatabase::database("user"));
	addSampleButton_ = new QPushButton("Add Sample to Plate");

	sampleListView_ = new QListView();
	AMSamplePlateItemDelegate* listViewDelegate = new AMSamplePlateItemDelegate;
	sampleListView_->setItemDelegate(listViewDelegate);
	samplePlateModel_ = new AMSamplePlateItemModel(samplePlate_, this);
	sampleListView_->setIconSize(QSize(45, 60));
	sampleListView_->setAlternatingRowColors(true);
	sampleListView_->setModel(samplePlateModel_);
	sampleListView_->setEditTriggers(QAbstractItemView::CurrentChanged | QAbstractItemView::SelectedClicked);
	sampleListView_->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

	vl1->addWidget(samplePlateSelector_);
	vl1->addWidget(moreInformationButton_);
	vl1->addWidget(sampleListView_, 1);
	samplePlateGroupBox->setLayout(vl1);

	vl2->addWidget(sampleSelector_);
	vl2->addWidget(addSampleButton_);
	samplesGroupBox->setLayout(vl2);

	vl_ = new QVBoxLayout();
	vl_->addWidget(samplePlateGroupBox, 1);
	vl_->addWidget(samplesGroupBox);
	setLayout(vl_);

	connect(addSampleButton_, SIGNAL(clicked()), this, SLOT(onAddSampleButtonClicked()));

	connect(listViewDelegate, SIGNAL(rowMarkPressed(int)), this, SLOT(onRowMarkPressed(int)));
	connect(listViewDelegate, SIGNAL(rowMoveToPressed(int)), this, SLOT(onRowMoveToPressed(int)));
	connect(listViewDelegate, SIGNAL(rowRemovePressed(int)), this, SLOT(onRowRemovePressed(int)));

	connect(listViewDelegate, SIGNAL(additionalInformationRequested(int)), this, SLOT(onAdditionalInformationRequested(int)));
	connect(moreInformationButton_, SIGNAL(clicked()), this, SLOT(onAdditionalPlateInformationRequested()));

	//connect(samplePlateSelector_, SIGNAL(samplePlateChanged()), this, SIGNAL(newSamplePlateSelected()));
	connect(samplePlateSelector_, SIGNAL(samplePlateChanged()), this, SLOT(onSamplePlateChanged()));

	addSampleButton_->setIcon(QIcon(":/add.png"));

}

void AMSamplePlateView::onAddSampleButtonClicked() {


	if(manipulator_)
		samplePlate_->append(
					AMSamplePosition(
						sampleSelector_->currentSample(),
						manipulator_->position(),
						manipulator_->facilityId()));
	else
		samplePlate_->append(
					AMSamplePosition(
						sampleSelector_->currentSample(),
						AMControlInfoList(),
						0));

	// save the sample plate, because it's been modified.
	samplePlate_->storeToDb(AMDatabase::database("user"));

}

#include "util/AMErrorMonitor.h"

// called by the delegate when the editor buttons (Mark, Move To, Remove) are clicked
void AMSamplePlateView::onRowMarkPressed(int row) {
	if(manipulator_)
		(*samplePlate_)[row].setPosition( manipulator_->position() );
	else {
		(*samplePlate_)[row].setPosition( AMControlInfoList() );
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -3, "We couldn't mark that sample position, because we don't know how to move or measure the sample manipulator."));
	}

	// save the sample plate, because it's been modified.
	samplePlate_->storeToDb(AMDatabase::database("user"));
}


// called by the delegate when the editor buttons (Mark, Move To, Remove) are clicked
void AMSamplePlateView::onRowMoveToPressed(int row) {

	if(manipulator_){

		if(!manipulator_->moveToPosition(samplePlate_->at(row).position()))
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -6, "We couldn't move that sample into position, because the sample manipulator is not ready."));
	}
	else {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -3, "We couldn't move that sample into position, because we don't know how to move the sample manipulator."));
	}
}

// called by the delegate when the editor buttons (Mark, Move To, Remove) are clicked
void AMSamplePlateView::onRowRemovePressed(int row) {
	samplePlate_->remove(row);

	// save the sample plate, because it's been modified.
	samplePlate_->storeToDb(AMDatabase::database("user"));
}

void AMSamplePlateView::onAdditionalInformationRequested(int row){
	if(manipulator_){
		AMSamplePosition *positionInQuestion = &(samplePlate_->operator [](row));
		AMSamplePositionAdditionalInformationView *additionalInformationView = new AMSamplePositionAdditionalInformationView(manipulator_, positionInQuestion );
		additionalInformationView->show();
	}
}

void AMSamplePlateView::onAdditionalPlateInformationRequested(){
	AMSamplePlateAdditionalInformationView *additionalPlateInformation = new AMSamplePlateAdditionalInformationView(samplePlate_, samplePlateModel_);
	additionalPlateInformation->show();
}

void AMSamplePlateView::onSamplePlateChanged(){
	moreInformationButton_->setEnabled(true);
	emit newSamplePlateSelected();
}

void AMSamplePlateItemModel::onSamplePositionChanged(int r)
{
	if(r < plate_->count()) {
		QModelIndex i = index(r,0);
		emit dataChanged(i,i);
	}
}

AMSamplePlatePositionInfo::AMSamplePlatePositionInfo(AMSamplePlate *samplePlate, int index, const QString &description, MPlotAxisScale *horizontalScale, MPlotAxisScale *verticalScale, QObject *parent) :
	QObject(parent)
{
	samplePlate_ = samplePlate;
	index_ = index;
	description_ = description;
	horizontalScale_ = horizontalScale;
	verticalScale_ = verticalScale;

	position_ = 0;
	area_ = 0;

	QColor rColor = Qt::green;
	QPen rPen = QPen(rColor, 0, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
	rColor.setAlphaF(0.5);

	position_ = new MPlotPoint();
	position_->setValue(QPointF(samplePlate_->at(index_).position().at(0).value(), samplePlate_->at(index_).position().at(2).value()));
	position_->setMarker(MPlotMarkerShape::Cross, 10, QPen(QColor(Qt::blue)));
	position_->setXAxisTarget(horizontalScale_);
	position_->setYAxisTarget(verticalScale_);

	if(samplePlate_->at(index_).topLeftPosition().count() == 4 && samplePlate_->at(index_).bottomRightPosition().count() == 4){
		double left, top, width, height;
		left = std::min(samplePlate_->at(index_).topLeftPosition().at(0).value(), samplePlate_->at(index_).bottomRightPosition().at(0).value());
		top = std::min(samplePlate_->at(index_).topLeftPosition().at(2).value(), samplePlate_->at(index_).bottomRightPosition().at(2).value());
		width = fabs(samplePlate_->at(index_).topLeftPosition().at(0).value() - samplePlate_->at(index_).bottomRightPosition().at(0).value());
		height = fabs(samplePlate_->at(index_).topLeftPosition().at(2).value() - samplePlate_->at(index_).bottomRightPosition().at(2).value());
		area_ = new MPlotRectangle(QRectF(left, top, width, height), rPen, QBrush(rColor));
		area_->setDescription(description_);
		area_->setXAxisTarget(horizontalScale_);
		area_->setYAxisTarget(verticalScale_);
	}

	QPen positionPen = QPen(getPositionColor(), 0, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
	position_->marker()->setPen(positionPen);
	if(area_){
		QPen areaPen = QPen(getAreaColor(), 0, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
		area_->setPen(areaPen);
		area_->setBrush(getAreaColor());
	}
	/*
	if(!samplePlate_->at(index_).positionWithinBounds()){
		QColor badColor = Qt::darkRed;
		QPen badPen = QPen(badColor, 0, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
		position_->marker()->setPen(badPen);
		area_->setPen(badPen);
		badColor.setAlphaF(0.5);
		area_->setBrush(badColor);
	}

	if(samplePlate_->sampleAtIndexOverlaps(index_)){
		QColor badColor = Qt::red;
		QPen badPen = QPen(badColor, 0, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
		position_->marker()->setPen(badPen);
		if(area_){
			area_->setPen(badPen);
			badColor.setAlphaF(0.5);
			area_->setBrush(badColor);
		}
	}
	*/
}

MPlotRectangle* AMSamplePlatePositionInfo::area() const{
	return area_;
}

MPlotPoint* AMSamplePlatePositionInfo::position() const{
	return position_;
}

QString AMSamplePlatePositionInfo::description() const{
	return description_;
}

QString AMSamplePlatePositionInfo::errors() const{
	QString retVal;
	if(!samplePlate_->at(index_).positionWithinBounds())
		retVal.append("Position is not within its boundaries.");
	if(samplePlate_->sampleAtIndexOverlaps(index_))
		retVal.append(" Position boundaries overlap another sample.");

	return retVal;
}

QColor AMSamplePlatePositionInfo::getAreaColor(bool isHighlighted) const{
	QColor retVal;
	if(!samplePlate_->at(index_).positionWithinBounds() && isHighlighted)
		retVal = Qt::darkMagenta;
	else if(!samplePlate_->at(index_).positionWithinBounds())
		retVal = Qt::darkRed;
	else if(samplePlate_->sampleAtIndexOverlaps(index_) && isHighlighted)
		retVal = Qt::magenta;
	else if(samplePlate_->sampleAtIndexOverlaps(index_))
		retVal = Qt::red;
	else if(isHighlighted)
		retVal = Qt::yellow;
	else
		retVal = Qt::green;

	retVal.setAlphaF(0.5);
	return retVal;
}

QColor AMSamplePlatePositionInfo::getPositionColor(bool isHighlighted) const{
	QColor retVal;
	if(!samplePlate_->at(index_).positionWithinBounds() && isHighlighted)
		retVal = Qt::black;
	else if(!samplePlate_->at(index_).positionWithinBounds())
		retVal = Qt::black;
	else if(samplePlate_->sampleAtIndexOverlaps(index_) && isHighlighted)
		retVal = Qt::black;
	else if(samplePlate_->sampleAtIndexOverlaps(index_))
		retVal = Qt::black;
	else if(isHighlighted)
		retVal = Qt::darkBlue;
	else
		retVal = Qt::blue;

	return retVal;
}

AMSamplePlatePositionInfoView::AMSamplePlatePositionInfoView(AMSamplePlatePositionInfo *positionInfo, QWidget *parent) :
	QFrame(parent)
{
	setFrameStyle(QFrame::StyledPanel);
	setLineWidth(2);
	setMidLineWidth(0);

	setMinimumWidth(200);
	setMaximumWidth(250);

	isHighlighted_ = false;
	positionInfo_ = positionInfo;

	nameLabel_ = new QLabel(positionInfo_->description());
	QString errorsString = positionInfo_->errors();
	QColor errorLabelColor;
	if(errorsString.isEmpty()){
		errorLabelColor = Qt::green;
		errorsString = "No Problems";
	}
	else
		errorLabelColor = Qt::red;
	errorsLabel_ = new QLabel(errorsString);
	QPalette errorLabelPalette = errorsLabel_->palette();
	errorLabelPalette.setBrush(QPalette::WindowText, QBrush(errorLabelColor));
	errorsLabel_->setPalette(errorLabelPalette);

	normalPalette_ = QPalette(palette());
	highlightedPalette_ = QPalette(palette());
	highlightedPalette_.setBrush(QPalette::Window, Qt::darkGray);
	setAutoFillBackground(true);

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(nameLabel_);
	vl->addWidget(errorsLabel_);
	vl->setContentsMargins(1,1,1,1);
	setLayout(vl);
}

void AMSamplePlatePositionInfoView::mouseReleaseEvent(QMouseEvent *e){
	setHighlighted(!isHighlighted_);
	e->accept();
}

void AMSamplePlatePositionInfoView::setHighlighted(bool isHighlighted){
	if(isHighlighted == isHighlighted_)
		return;

	isHighlighted_ = isHighlighted;
	QPen positionPen = QPen(positionInfo_->getPositionColor(isHighlighted_), 0, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
	positionInfo_->position()->marker()->setPen(positionPen);
	if(positionInfo_->area()){
		QPen areaPen = QPen(positionInfo_->getAreaColor(isHighlighted_), 0, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
		positionInfo_->area()->setPen(areaPen);
		positionInfo_->area()->setBrush(positionInfo_->getAreaColor(isHighlighted_));
	}

	if(isHighlighted_)
		setPalette(highlightedPalette_);
	else
		setPalette(normalPalette_);

	emit becameHighlighted(isHighlighted_);
}

AMSamplePlatePositionInfoListView::AMSamplePlatePositionInfoListView(QList<AMSamplePlatePositionInfoView *> infoViews, QWidget *parent) :
	QWidget(parent)
{
	infoViews_ = infoViews;

	QVBoxLayout *positionInfosVL = new QVBoxLayout();

	for(int x = 0; x < infoViews_.count(); x++){
		positionInfosVL->addWidget(infoViews_.at(x));
		connect(infoViews_.at(x), SIGNAL(becameHighlighted(bool)), this, SLOT(onSamplePlatePositionInfoViewBecameHighlighted(bool)));
	}

	positionInfosVL->addSpacing(10);
	positionInfosVL->setContentsMargins(2,2,2,2);
	positionInfosVL->setSpacing(0);
	AMScrollViewWidget *positionInfosScroll = new AMScrollViewWidget(positionInfosVL);
	QScrollArea *positionInfosScrollArea = new QScrollArea();
	positionInfosScrollArea->setWidget(positionInfosScroll);
	positionInfosScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(positionInfosScrollArea);
	setLayout(vl);
}

void AMSamplePlatePositionInfoListView::onSamplePlatePositionInfoViewBecameHighlighted(bool isHighlighted){
	AMSamplePlatePositionInfoView *infoView = qobject_cast<AMSamplePlatePositionInfoView*>(QObject::sender());
	if(infoView && isHighlighted)
		for(int x = 0; x < infoViews_.count(); x++)
			if(infoViews_.at(x) != infoView)
				infoViews_.at(x)->setHighlighted(false);
}

AMScrollViewWidget::AMScrollViewWidget(QLayout *layout, QWidget *parent) :
	QWidget(parent)
{
	setLayout(layout);
}

AMSamplePlateAdditionalInformationView::AMSamplePlateAdditionalInformationView(AMSamplePlate *samplePlate, AMSamplePlateItemModel *samplePlateModel, QWidget *parent) :
	QWidget(parent)
{
	setWindowModality(Qt::ApplicationModal);

	samplePlate_ = samplePlate;
	samplePlateModel_ = samplePlateModel;

	imageView_ = new MPlotWidget();
	imageView_->setMinimumHeight(400);
	imagePlot_ = new MPlot();

	MPlotAxisScale *horizontalScale = new MPlotAxisScale(Qt::Horizontal);
	MPlotAxisScale *verticalScale = new MPlotAxisScale(Qt::Vertical);
	horizontalScale->setDataRange(MPlotAxisRange(-10, 10));
	verticalScale->setDataRange(MPlotAxisRange(-10, 15));
	imagePlot_->addAxisScale(horizontalScale);
	imagePlot_->addAxisScale(verticalScale);
	imagePlot_->axisLeft()->setAxisScale(verticalScale);
	imagePlot_->axisBottom()->setAxisScale(horizontalScale);

	imageView_->setPlot(imagePlot_);
	imagePlot_->axisScaleLeft()->setPadding(2);
	imagePlot_->axisScaleBottom()->setPadding(2);
	imagePlot_->setMarginBottom(10);
	imagePlot_->setMarginLeft(10);
	imagePlot_->setMarginRight(5);
	imagePlot_->setMarginTop(5);
	imagePlot_->plotArea()->setBrush(QBrush(QColor(Qt::white)));
	imagePlot_->axisRight()->setTicks(4);
	imagePlot_->axisBottom()->setTicks(4);
	imagePlot_->axisLeft()->showGrid(false);
	imagePlot_->axisBottom()->showAxisName(false);
	imagePlot_->axisLeft()->showAxisName(false);
	imagePlot_->legend()->hide();

	//QVBoxLayout *positionInfosVL = new QVBoxLayout();

	AMSamplePlatePositionInfo *positionInfo;
	for(int x = 0; x < samplePlate_->count(); x++){
		positionInfo = new AMSamplePlatePositionInfo(samplePlate_, x, samplePlateModel_->sampleName(x), horizontalScale, verticalScale);
		imagePlot_->addItem(positionInfo->position());
		if(positionInfo->area())
			imagePlot_->addItem(positionInfo->area());

		positionInfos_.append(positionInfo);
		positionInfoViews_.append(new AMSamplePlatePositionInfoView(positionInfo));
		//positionInfosVL->addWidget(positionInfoViews_.last());
	}

	/*
	positionInfosVL->addSpacing(10);
	positionInfosVL->setContentsMargins(2,2,2,2);
	positionInfosVL->setSpacing(0);
	AMScrollViewWidget *positionInfosScroll = new AMScrollViewWidget(positionInfosVL);
	QScrollArea *positionInfosScrollArea = new QScrollArea();
	positionInfosScrollArea->setWidget(positionInfosScroll);
	positionInfosScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	*/
	AMSamplePlatePositionInfoListView *infoViewsList = new AMSamplePlatePositionInfoListView(positionInfoViews_);

	QHBoxLayout *hl = new QHBoxLayout();
	hl->addWidget(imageView_);
	//hl->addLayout(positionInfosVL);
	//hl->addWidget(positionInfosScrollArea);
	hl->addWidget(infoViewsList);
	setLayout(hl);
}

#include <QFormLayout>

AMSamplePositionManuallyEnterView::AMSamplePositionManuallyEnterView(QWidget *parent) :
	QDialog(parent)
{
	QFormLayout *fl = new QFormLayout();

	inOutDSBox_ = new QDoubleSpinBox();
	inOutDSBox_->setMaximum(10);
	inOutDSBox_->setMinimum(-10);
	fl->addRow("In/Out", inOutDSBox_);

	upStDownStDSBox_ = new QDoubleSpinBox();
	upStDownStDSBox_->setMaximum(10);
	upStDownStDSBox_->setMinimum(-10);
	fl->addRow("UpStr/DownStr", upStDownStDSBox_);

	upDownDSBox_ = new QDoubleSpinBox();
	upDownDSBox_->setMaximum(40);
	upDownDSBox_->setMinimum(-40);
	fl->addRow("Up/Down", upDownDSBox_);

	rotDSBox_ = new QDoubleSpinBox();
	rotDSBox_->setMaximum(180);
	rotDSBox_->setMinimum(-180);
	fl->addRow("Rotation", rotDSBox_);

	QHBoxLayout *hl = new QHBoxLayout();
	applyButton_ = new QPushButton("Apply");
	cancelButton_ = new QPushButton("Cancel");
	hl->addWidget(applyButton_);
	hl->addWidget(cancelButton_);

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addLayout(fl);
	vl->addLayout(hl);

	setLayout(vl);

	connect(applyButton_, SIGNAL(clicked()), this, SLOT(onApplyButtonClicked()));
	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked()));

	upDown_ = -999999;
	inOut_ = -999999;
	upStDownSt_ = -999999;
	rot_ = -999999;
}

void AMSamplePositionManuallyEnterView::onCancelButtonClicked(){
	hideAndFinish();
}

void AMSamplePositionManuallyEnterView::onApplyButtonClicked(){
	upDown_ = upDownDSBox_->value();
	inOut_ = inOutDSBox_->value();
	upStDownSt_ = upStDownStDSBox_->value();
	rot_ = rotDSBox_->value();
	hideAndFinish();
}

void AMSamplePositionManuallyEnterView::hideAndFinish(){
	hide();
	emit finished(upDown_, inOut_, upStDownSt_, rot_);
}

void AMSamplePositionManuallyEnterView::closeEvent(QCloseEvent *e){
	e->accept();
	hideAndFinish();
}

AMSamplePositionAdditionalInformationView::AMSamplePositionAdditionalInformationView(AMSampleManipulator *manipulator, AMSamplePosition *samplePosition, QWidget *parent) :
	QDialog(parent)
{
	manipulator_ = manipulator;
	samplePosition_ = samplePosition;

	enterTopLeftDialog_ = 0; //NULL
	enterBottomRightDialog_ = 0; //NULL

	QGroupBox *positionGroupBox = new QGroupBox("Position");
	QFormLayout *fl = new QFormLayout();
	AMControlInfoList samplePositionList = samplePosition_->position();
	for(int x = 0; x < samplePositionList.count(); x++)
		fl->addRow(samplePositionList.at(x).description(), new QLabel(QString("%1").arg(samplePositionList.at(x).value())));
	positionGroupBox->setLayout(fl);

	topLeftLabel_ = new QLabel();
	originalTopLeft_ = samplePosition_->topLeftPosition();
	setTopLeftText();
	setTopLeftFromManipulatorButton_ = new QPushButton("Mark as Current Position");
	manuallyEnterTopLeftButton_ = new QPushButton("Manually Enter");
	QHBoxLayout *topLeftHL = new QHBoxLayout();
	topLeftHL->addWidget(new QLabel("Top Left: "));
	topLeftHL->addWidget(topLeftLabel_);
	topLeftHL->addWidget(setTopLeftFromManipulatorButton_);
	topLeftHL->addWidget(manuallyEnterTopLeftButton_);

	bottomRightLabel_ = new QLabel();
	originalBottomRight_ = samplePosition_->bottomRightPosition();
	setBottomRightText();
	setBottomRightFromManipulatorButton_ = new QPushButton("Mark as Current Position");
	manuallyEnterBottomRightButton_ = new QPushButton("Manually Enter");
	QHBoxLayout *bottomRightHL = new QHBoxLayout();
	bottomRightHL->addWidget(new QLabel("Bottom Right: "));
	bottomRightHL->addWidget(bottomRightLabel_);
	bottomRightHL->addWidget(setBottomRightFromManipulatorButton_);
	bottomRightHL->addWidget(manuallyEnterBottomRightButton_);

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addLayout(topLeftHL);
	vl->addLayout(bottomRightHL);

	QHBoxLayout *mainHL = new QHBoxLayout();
	mainHL->addWidget(positionGroupBox);
	mainHL->addLayout(vl);

	applyButton_ = new QPushButton("Apply");
	applyButton_->setEnabled(false);
	cancelButton_ = new QPushButton("Cancel");
	errorLabel_ = new QLabel("");
	QHBoxLayout *buttonsHL = new QHBoxLayout();
	buttonsHL->addStretch();
	buttonsHL->addWidget(errorLabel_);
	buttonsHL->addWidget(applyButton_);
	buttonsHL->addWidget(cancelButton_);

	QVBoxLayout *mainVL = new QVBoxLayout();
	mainVL->addLayout(mainHL);
	mainVL->addLayout(buttonsHL);

	setLayout(mainVL);

	cancelButton_->setDefault(true);

	connect(setTopLeftFromManipulatorButton_, SIGNAL(clicked()), this, SLOT(onTopLeftSetFromManipulator()));
	connect(setBottomRightFromManipulatorButton_, SIGNAL(clicked()), this, SLOT(onBottomRightSetFromManipulator()));
	connect(manuallyEnterTopLeftButton_, SIGNAL(clicked()), this, SLOT(onTopLeftManuallyEnterClicked()));
	connect(manuallyEnterBottomRightButton_, SIGNAL(clicked()), this, SLOT(onBottomRightManuallyEnterClicked()));

	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked()));
	connect(applyButton_, SIGNAL(clicked()), this, SLOT(onApplyButtonClicked()));
}

void AMSamplePositionAdditionalInformationView::onTopLeftSetFromManipulator(){
	samplePosition_->setTopLeftPosition(manipulator_->position());
	setTopLeftText();
	checkValidity();
}

void AMSamplePositionAdditionalInformationView::onBottomRightSetFromManipulator(){
	samplePosition_->setBottomRightPosition(manipulator_->position());
	setBottomRightText();
	checkValidity();
}

void AMSamplePositionAdditionalInformationView::onTopLeftManuallyEnterClicked(){
	enterTopLeftDialog_ = new AMSamplePositionManuallyEnterView();
	connect(enterTopLeftDialog_, SIGNAL(finished(double,double,double,double)), this, SLOT(onTopLeftManualEnterFinished(double,double,double,double)));
	enterTopLeftDialog_->show();
}

void AMSamplePositionAdditionalInformationView::onBottomRightManuallyEnterClicked(){
	enterBottomRightDialog_ = new AMSamplePositionManuallyEnterView();
	connect(enterBottomRightDialog_, SIGNAL(finished(double,double,double,double)), this, SLOT(onBottomRightManualEnterFinished(double,double,double,double)));
	enterBottomRightDialog_->show();
}

void AMSamplePositionAdditionalInformationView::onTopLeftManualEnterFinished(double upDown, double inOut, double upStDownSt, double rot){
	//qdebug() << "Want to set as " << upDown << inOut << upStDownSt << rot;
	AMControlInfoList newTopLeft;
	newTopLeft.setValuesFrom(manipulator_->position());
	newTopLeft[0].setValue(inOut);
	newTopLeft[1].setValue(upStDownSt);
	newTopLeft[2].setValue(upDown);
	newTopLeft[3].setValue(rot);

	samplePosition_->setTopLeftPosition(newTopLeft);
	setTopLeftText();
	checkValidity();
	enterTopLeftDialog_->deleteLater();
	enterTopLeftDialog_ = 0; //NULL
}

void AMSamplePositionAdditionalInformationView::onBottomRightManualEnterFinished(double upDown, double inOut, double upStDownSt, double rot){
	AMControlInfoList newBottomRight;
	newBottomRight.setValuesFrom(manipulator_->position());
	newBottomRight[0].setValue(inOut);
	newBottomRight[1].setValue(upStDownSt);
	newBottomRight[2].setValue(upDown);
	newBottomRight[3].setValue(rot);

	samplePosition_->setBottomRightPosition(newBottomRight);
	setBottomRightText();
	checkValidity();
	enterBottomRightDialog_->deleteLater();
	enterBottomRightDialog_ = 0; //NULL
}

void AMSamplePositionAdditionalInformationView::onCancelButtonClicked(){
	if( (samplePosition_->topLeftPosition().count() != 0) && !(samplePosition_->topLeftPosition() == originalTopLeft_)){
		samplePosition_->setTopLeftPosition(originalTopLeft_);
		setTopLeftText();
	}
	if( (samplePosition_->bottomRightPosition().count() != 0) && !(samplePosition_->bottomRightPosition() == originalBottomRight_)){
		samplePosition_->setBottomRightPosition(originalBottomRight_);
		setBottomRightText();
	}
	hideAndFinish();
}

void AMSamplePositionAdditionalInformationView::onApplyButtonClicked(){
	if(samplePosition_->modified())
		samplePosition_->storeToDb(samplePosition_->database());
	hideAndFinish();
}

void AMSamplePositionAdditionalInformationView::hideAndFinish(){
	hide();
	emit finished();
}

void AMSamplePositionAdditionalInformationView::closeEvent(QCloseEvent *e){
	if( (samplePosition_->topLeftPosition().count() != 0) && !(samplePosition_->topLeftPosition() == originalTopLeft_)){
		samplePosition_->setTopLeftPosition(originalTopLeft_);
		setTopLeftText();
	}
	if( (samplePosition_->bottomRightPosition().count() != 0) && !(samplePosition_->bottomRightPosition() == originalBottomRight_)){
		samplePosition_->setBottomRightPosition(originalBottomRight_);
		setBottomRightText();
	}
	e->accept();
	hideAndFinish();
}

void AMSamplePositionAdditionalInformationView::setTopLeftText(){
	if(samplePosition_->topLeftPosition().count() == 0)
		topLeftLabel_->setText("No Values Set");
	else{
		QString topLeftText;
		for(int x = 0; x < samplePosition_->topLeftPosition().count(); x++)
			topLeftText.append(QString("%1, ").arg(samplePosition_->topLeftPosition().at(x).value()));
		topLeftText.remove(topLeftText.count()-2, 2);
		topLeftLabel_->setText(topLeftText);
	}
}

void AMSamplePositionAdditionalInformationView::setBottomRightText(){
	if(samplePosition_->bottomRightPosition().count() == 0)
		bottomRightLabel_->setText("No Values Set");
	else{
		QString bottomRightText;
		for(int x = 0; x < samplePosition_->bottomRightPosition().count(); x++)
			bottomRightText.append(QString("%1, ").arg(samplePosition_->bottomRightPosition().at(x).value()));
		bottomRightText.remove(bottomRightText.count()-2, 2);
		bottomRightLabel_->setText(bottomRightText);
	}
}


void AMSamplePositionAdditionalInformationView::checkValidity(){
	if( (samplePosition_->topLeftPosition().count() != 0) && (samplePosition_->bottomRightPosition().count() != 0) ){
		for(int x = 0; x < samplePosition_->topLeftPosition().count(); x++){
			double positionValue = samplePosition_->position().at(x).value();
			double topLeftValue = samplePosition_->topLeftPosition().at(x).value();
			double topLeftTolerance = samplePosition_->topLeftPosition().at(x).tolerance();
			double bottomRightValue = samplePosition_->bottomRightPosition().at(x).value();
			double bottomRightTolerance = samplePosition_->bottomRightPosition().at(x).tolerance();
			if( (positionValue > std::max(topLeftValue+topLeftTolerance, bottomRightValue+bottomRightTolerance)) || (positionValue < std::min(topLeftValue-topLeftTolerance, bottomRightValue-bottomRightTolerance)) ){
				errorLabel_->setText(QString("Problem on axis %1").arg(x));
				applyButton_->setEnabled(false);
				return;
			}
		}
		errorLabel_->setText("");
		applyButton_->setEnabled(true);
	}
}
