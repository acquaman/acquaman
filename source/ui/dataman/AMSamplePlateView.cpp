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


#include "AMSamplePlateView.h"

#include <QListView>
#include <QApplication>
#include <QPainter>
#include <QVBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QPushButton>
#include <QTimer>

#include "util/AMDateTimeUtils.h"
#include "ui/dataman/AMSampleView.h"

AMSamplePlateItemModel::AMSamplePlateItemModel(AMSamplePlate *samplePlate, QObject *parent) :
	QAbstractListModel(parent)
{
	samplePlate_ = samplePlate;

	if(samplePlate_){
		connect(samplePlate_, SIGNAL(sampleAboutToBeAdded(int)), this, SLOT(onSampleAboutToBeAdded(int)));
		connect(samplePlate_, SIGNAL(sampleAdded(int)), this, SLOT(onSampleAdded(int)));
		connect(samplePlate_, SIGNAL(sampleAboutToBeRemoved(int)), this, SLOT(onSampleAboutToBeRemoved(int)));
		connect(samplePlate_, SIGNAL(sampleRemoved(int)), this, SLOT(onSampleRemoved(int)));
		connect(samplePlate_, SIGNAL(sampleChanged(int)), this, SLOT(onSampleChanged(int)));
	}
}

int AMSamplePlateItemModel::rowCount(const QModelIndex &parent) const
{
	if(parent.isValid() || !samplePlate_)
		return 0;
	return samplePlate_->sampleCount();
}

QVariant AMSamplePlateItemModel::data(const QModelIndex &index, int role) const
{
	if(index.parent().isValid() || index.column() > 0 || !samplePlate_)
		return QVariant();
	switch(role){
	case Qt::DisplayRole:
	case Qt::EditRole:
		return samplePlate_->sampleAt(index.row())->name();
	case Qt::DecorationRole:
		return QApplication::style()->standardIcon(QStyle::SP_FileIcon);	/// \todo implement pictures
	case AM::DateTimeRole:
		return samplePlate_->sampleAt(index.row())->dateTime();
	case AM::DescriptionRole:
		if(samplePlate_->sampleAt(index.row())->modified())
			return QString("MODIFIED");
		else
			return QString("created ") +  AMDateTimeUtils::prettyDateTime(samplePlate_->sampleAt(index.row())->dateTime());

		/*
	case AM::UserRole:
		return sampleElements(index.row());
	case AM::UserRole + 1:
		return positionsString(index.row());
	case AM::UserRole + 2:
		return facilityName(index.row());

		/// Returns a pointer to the AMSamplePosition object here. Can access all the additional information directly.
	case AM::PointerRole:
		return QVariant::fromValue((QObject*)&(plate_->at(index.row())));
		*/

	default:
		return QVariant();
	}
}

Qt::ItemFlags AMSamplePlateItemModel::flags(const QModelIndex &index) const
{
	Q_UNUSED(index)

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant AMSamplePlateItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(section)

	if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return QString("Samples");
	else
		return QVariant();
}

void AMSamplePlateItemModel::onSampleAboutToBeAdded(int index){
	beginInsertRows(QModelIndex(), index, index);
}

void AMSamplePlateItemModel::onSampleAdded(int index){
	Q_UNUSED(index);
	endInsertRows();
}

void AMSamplePlateItemModel::onSampleAboutToBeRemoved(int index){
	beginRemoveRows(QModelIndex(), index, index);
}

void AMSamplePlateItemModel::onSampleRemoved(int index){
	Q_UNUSED(index)
	endRemoveRows();
}

void AMSamplePlateItemModel::onSampleChanged(int index){
	emit dataChanged(createIndex(index, 0, samplePlate_->sampleAt(index)), createIndex(index, 0, samplePlate_->sampleAt(index)));
}

AMSamplePlateItemEditor::AMSamplePlateItemEditor(int row, QWidget *parent) :
	QWidget(parent)
{
	row_ = row;

	moveToButton_ = new QToolButton();
	moveToButton_->setText("Move To");
	moreInfoButton_ = new QToolButton();
	moreInfoButton_->setText("More Info");
	closeButton_ = new QToolButton();
	closeButton_->setIcon(QApplication::style()->standardIcon(QStyle::SP_DockWidgetCloseButton));

	QHBoxLayout *hl = new QHBoxLayout();
	hl->addWidget(moveToButton_);
	hl->addWidget(moreInfoButton_);
	hl->addWidget(closeButton_);
	hl->setContentsMargins(0,0,0,0);
	setLayout(hl);

	connect(moveToButton_, SIGNAL(pressed()), this, SLOT(onMoveToButtonClicked()));
	connect(moreInfoButton_, SIGNAL(pressed()), this, SLOT(onMoreInfoButtonClicked()));
	connect(closeButton_, SIGNAL(pressed()), this, SLOT(onCloseButtonPressed()));
}

void AMSamplePlateItemEditor::onMoveToButtonClicked(){
	emit rowMoveToPressed(row_);
}

void AMSamplePlateItemEditor::onMoreInfoButtonClicked(){
	emit rowMoreInfoPressed(row_);
}

void AMSamplePlateItemEditor::onCloseButtonPressed(){
	emit rowClosedPressed(row_);
}

AMSamplePlateItemDelegate::AMSamplePlateItemDelegate(QObject *parent) :
	QStyledItemDelegate(parent)
{
}

AMSamplePlateItemDelegate::~AMSamplePlateItemDelegate(){
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
	AMSamplePlateItemEditor *editor = new AMSamplePlateItemEditor(index.row(), parent);

	connect(editor, SIGNAL(rowMoveToPressed(int)), this, SIGNAL(rowMoveToPressed(int)));
	connect(editor, SIGNAL(rowMoreInfoPressed(int)), this, SIGNAL(rowMoreInfoPressed(int)));
	connect(editor, SIGNAL(rowClosedPressed(int)), this, SIGNAL(rowClosedPressed(int)));

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

AMSamplePlateView::AMSamplePlateView(AMSamplePlate *samplePlate, QWidget *parent) :
	QGroupBox("No Sample Plate", parent)
{
	samplePlate_ = 0; //NULL
	samplePlateItemModel_ = 0; //NULL
	sampleListView_ = 0; //NULL


	emptyModel_ = new AMSamplePlateItemModel(0);
	noSamplePlateLabel_ = new QLabel("No Sample Plate Selected");
	samplePlateModifiedLabel_ = new QLabel("Sample Plate\nModified");
	QFont samplePlateModifiedLabelFont = samplePlateModifiedLabel_->font();
	samplePlateModifiedLabelFont.setBold(true);
	samplePlateModifiedLabelFont.setItalic(true);
	samplePlateModifiedLabelFont.setPointSize(samplePlateModifiedLabelFont.pointSize()+2);
	samplePlateModifiedLabel_->setFont(samplePlateModifiedLabelFont);
	saveSamplePlateButton_ = new QPushButton("Save Plate");
	connect(saveSamplePlateButton_, SIGNAL(clicked()), this, SLOT(onSaveSamplePlateButtonClicked()));
	samplePlateModifiedHL_ = new QHBoxLayout();
	samplePlateModifiedHL_->addWidget(samplePlateModifiedLabel_);
	samplePlateModifiedHL_->addWidget(saveSamplePlateButton_);

	vl_ = new QVBoxLayout();
	vl_->addWidget(noSamplePlateLabel_);
	setLayout(vl_);

	samplePlateModifiedTimer_ = new QTimer();
	samplePlateModifiedTimer_->setSingleShot(true);
	connect(samplePlateModifiedTimer_, SIGNAL(timeout()), samplePlateModifiedLabel_, SLOT(show()));
	connect(samplePlateModifiedTimer_, SIGNAL(timeout()), saveSamplePlateButton_, SLOT(show()));

	setSamplePlate(samplePlate);
}
#include "beamline/camera/AMSampleCamera.h"
void AMSamplePlateView::setSamplePlate(AMSamplePlate *samplePlate){

	if(samplePlate_)
	{
		disconnect(AMSampleCamera::set(), SIGNAL(motorCoordinateChanged(QVector3D)), samplePlate_, SLOT(setPlatePosition(QVector3D)));
		disconnect(AMSampleCamera::set(), SIGNAL(motorRotationChanged(double)), samplePlate_, SLOT(setPlateRotation(double)));
	}
	samplePlate_ = samplePlate;
	AMSamplePlateItemModel *oldSamplePlateItemModel = samplePlateItemModel_;
	if(samplePlate_){
		samplePlateItemModel_ = new AMSamplePlateItemModel(samplePlate_);
		setTitle(samplePlate_->name());
		connect(samplePlate_, SIGNAL(modifiedChanged(bool)), this, SLOT(onSamplePlateModifiedChanged(bool)));
		connect(AMSampleCamera::set(), SIGNAL(motorCoordinateChanged(QVector3D)), samplePlate_, SLOT(setPlatePosition(QVector3D)));
		connect(AMSampleCamera::set(), SIGNAL(motorRotationChanged(double)), samplePlate_, SLOT(setPlateRotation(double)));
	}
	else{
		samplePlateItemModel_ = 0; //NULL
		setTitle("No Sample Plate");
	}

	if(!sampleListView_ && samplePlateItemModel_){
		sampleListView_ = new QListView();
		sampleListView_->setAlternatingRowColors(true);
		AMSamplePlateItemDelegate *listViewDelegate = new AMSamplePlateItemDelegate();
		sampleListView_->setItemDelegate(listViewDelegate);

		connect(listViewDelegate, SIGNAL(rowMoveToPressed(int)), this, SLOT(onRowMoveToPressed(int)));
		connect(listViewDelegate, SIGNAL(rowMoreInfoPressed(int)), this, SLOT(onRowMoreInfoPressed(int)));
		connect(listViewDelegate, SIGNAL(rowClosedPressed(int)), this, SLOT(onRowClosedPressed(int)));

		vl_->addWidget(sampleListView_);
		vl_->addLayout(samplePlateModifiedHL_);
		samplePlateModifiedLabel_->hide();
		saveSamplePlateButton_->hide();
	}

	if(!samplePlate_ && sampleListView_){
		sampleListView_->hide();
		noSamplePlateLabel_->show();
	}
	if(samplePlate_ && !noSamplePlateLabel_->isHidden())
		noSamplePlateLabel_->hide();
	if(samplePlate_ && sampleListView_->isHidden())
		sampleListView_->show();

	if(samplePlateItemModel_)
		sampleListView_->setModel(samplePlateItemModel_);
	else if(sampleListView_)
		sampleListView_->setModel(emptyModel_);

	if(oldSamplePlateItemModel)
		oldSamplePlateItemModel->deleteLater();
}

void AMSamplePlateView::onRowMoveToPressed(int row){
	samplePlate_->requestSampleMoveTo(row);
}

void AMSamplePlateView::onRowMoreInfoPressed(int row){
	AMSampleView *sampleView = new AMSampleView(samplePlate_->sampleAt(row));
	connect(sampleView, SIGNAL(aboutToClose()), sampleView, SLOT(deleteLater()));
	sampleView->show();
}

void AMSamplePlateView::onRowClosedPressed(int row){
	emit sampleAboutToBeRemoved(row);
	samplePlate_->removeSample(samplePlate_->sampleAt(row));
}

void AMSamplePlateView::onSamplePlateModifiedChanged(bool isModified){
	if(isModified){
		//samplePlateModifiedLabel_->show();
		//saveSamplePlateButton_->show();
		samplePlateModifiedTimer_->start(500);
	}
	else if(samplePlateModifiedTimer_->isActive()){
		samplePlateModifiedTimer_->stop();
	}
	else{
		samplePlateModifiedLabel_->hide();
		saveSamplePlateButton_->hide();
	}
}

void AMSamplePlateView::onSaveSamplePlateButtonClicked(){
	if(samplePlate_)
		samplePlate_->storeToDb(samplePlate_->database());
}

