/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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
#include "ui/AMSamplePositionViewActionsWidget.h"

#include "ui/AMDetailedItemDelegate.h"

AMSamplePlateItemModel::AMSamplePlateItemModel(AMSamplePlate* plate, QObject* parent) :
	QAbstractListModel(parent)
{
	plate_ = plate;

	sampleTableName_ = AMDbObjectSupport::tableNameForClass<AMSample>();

	connect(plate_, SIGNAL(samplePositionAboutToBeAdded(int)), this, SLOT(onSamplePositionAboutToBeAdded(int)));
	connect(plate_, SIGNAL(samplePositionAdded(int)), this, SLOT(onSamplePositionAdded(int)));
	connect(plate_, SIGNAL(samplePositionAboutToBeRemoved(int)), this, SLOT(onSamplePositionAboutToBeRemoved(int)));
	connect(plate_, SIGNAL(samplePositionRemoved(int)), this, SLOT(onSamplePositionRemoved(int)));
	connect(plate_, SIGNAL(samplePositionChanged(int)), this, SLOT(onSamplePositionChanged(int)));

	connect(AMDatabase::userdb(), SIGNAL(updated(QString,int)), this, SLOT(onDatabaseItemUpdated(QString,int)), Qt::QueuedConnection);
	connect(AMDatabase::userdb(), SIGNAL(removed(QString,int)), this, SLOT(onDatabaseItemRemoved(QString,int)), Qt::QueuedConnection);

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
		if(!cachedSamples_[index].loadFromDb(AMDatabase::userdb(), correctId)) {
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
			if(!cachedSamples_[i].loadFromDb(AMDatabase::userdb(), id)) {
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

	samplePlateTableName_ = AMDbObjectSupport::tableNameForClass<AMSamplePlate>();
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

	connect(AMDatabase::userdb(), SIGNAL(updated(QString,int)), this, SLOT(onDatabaseUpdated(QString,int)), Qt::QueuedConnection);
	connect(AMDatabase::userdb(), SIGNAL(created(QString,int)), this, SLOT(onDatabaseCreated(QString,int)), Qt::QueuedConnection);
	connect(AMDatabase::userdb(), SIGNAL(removed(QString,int)), this, SLOT(onDatabaseRemoved(QString,int)), Qt::QueuedConnection);

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

	QSqlQuery q2 = AMDatabase::userdb()->query();
	q2.prepare(QString("SELECT id,name,dateTime FROM %1 ORDER BY dateTime ASC").arg(samplePlateTableName_));
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
	if(newPlate.storeToDb(AMDatabase::userdb()))
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
		p.loadFromDb(AMDatabase::userdb(), id);
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
	sampleSelector_ = new AMSampleEditor(AMDatabase::userdb());
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

	connect(samplePlateSelector_, SIGNAL(samplePlateChanged()), this, SIGNAL(newSamplePlateSelected()));

	addSampleButton_->setIcon(QIcon(":/add.png"));

}

void AMSamplePlateView::onAddSampleButtonClicked() {


	if(manipulator_)
		samplePlate_->append(
					AMSamplePosition(
						sampleSelector_->currentSample(),
						manipulator_->toInfoList(),
						0/*manipulator_->facilityId()*/));
	else
		samplePlate_->append(
					AMSamplePosition(
						sampleSelector_->currentSample(),
						AMControlInfoList(),
						0));

	// save the sample plate, because it's been modified.
	samplePlate_->storeToDb(AMDatabase::userdb());

}

#include "util/AMErrorMonitor.h"

// called by the delegate when the editor buttons (Mark, Move To, Remove) are clicked
void AMSamplePlateView::onRowMarkPressed(int row) {
	if(manipulator_)
		(*samplePlate_)[row].setPosition( manipulator_->toInfoList() );
	else {
		(*samplePlate_)[row].setPosition( AMControlInfoList() );
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -3, "We couldn't mark that sample position, because we don't know how to move or measure the sample manipulator."));
	}

	// save the sample plate, because it's been modified.
	samplePlate_->storeToDb(AMDatabase::userdb());
}


// #include "actions/AMBeamlineControlSetMoveAction.h"

// called by the delegate when the editor buttons (Mark, Move To, Remove) are clicked
void AMSamplePlateView::onRowMoveToPressed(int row) {
	/// \todo Move this to a manipulator/controller's job, rather than do ourselves? Use an action instead of direct move?
	if(manipulator_)
		manipulator_->setFromInfoList(samplePlate_->at(row).position());
	else {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -3, "We couldn't move that sample into position, because we don't know how to move the sample manipulator."));
	}
}

// called by the delegate when the editor buttons (Mark, Move To, Remove) are clicked
void AMSamplePlateView::onRowRemovePressed(int row) {
	samplePlate_->remove(row);

	// save the sample plate, because it's been modified.
	samplePlate_->storeToDb(AMDatabase::userdb());
}

void AMSamplePlateItemModel::onSamplePositionChanged(int r)
{
	if(r<plate_->count()) {
		QModelIndex i = index(r,0);
		emit dataChanged(i,i);
	}
}
