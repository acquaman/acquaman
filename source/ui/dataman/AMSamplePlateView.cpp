#include "AMSamplePlateView.h"

#include <QApplication>
#include <QPainter>
#include <QVBoxLayout>
#include <QLabel>

#include "util/AMDateTimeUtils.h"

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

	//return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
	return Qt::ItemIsEnabled;
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
	qDebug() << "Sample at index " << index << " changed";
	emit dataChanged(createIndex(index, 0, samplePlate_->sampleAt(index)), createIndex(index, 0, samplePlate_->sampleAt(index)));
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

/*
// create an editor widget with buttons to mark, move to, and remove this sample.
QWidget* AMSamplePlateItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {

	Q_UNUSED(option)

	AMSamplePositionPre2013ViewActionsWidget* editor = new AMSamplePositionPre2013ViewActionsWidget(index.row(), parent);

	connect(editor, SIGNAL(rowMarkPressed(int)), this, SIGNAL(rowMarkPressed(int)));
	connect(editor, SIGNAL(rowMoveToPressed(int)), this, SIGNAL(rowMoveToPressed(int)));
	connect(editor, SIGNAL(rowRemovePressed(int)), this, SIGNAL(rowRemovePressed(int)));

	connect(editor, SIGNAL(additionalInformationRequested(int)), this, SIGNAL(additionalInformationRequested(int)));

	return editor;
}
*/

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
	QWidget(parent)
{
	samplePlate_ = 0; //NULL
	samplePlateItemModel_ = 0; //NULL
	sampleListView_ = 0; //NULL

	emptyModel_ = new AMSamplePlateItemModel(0);
	noSamplePlateLabel_ = new QLabel("No Sample Plate Selected");

	vl_ = new QVBoxLayout();
	vl_->addWidget(noSamplePlateLabel_);
	setLayout(vl_);

	setSamplePlate(samplePlate);
	/*
	samplePlate_ = samplePlate;
	samplePlateItemModel_ = new AMSamplePlateItemModel(samplePlate_);
	sampleListView_ = new QListView();
	sampleListView_->setModel(samplePlateItemModel_);
	sampleListView_->setAlternatingRowColors(true);
	AMSamplePlateItemDelegate *listViewDelegate = new AMSamplePlateItemDelegate();
	sampleListView_->setItemDelegate(listViewDelegate);

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(sampleListView_);

	setLayout(vl);
	*/
}

void AMSamplePlateView::setSamplePlate(AMSamplePlate *samplePlate){

	samplePlate_ = samplePlate;
	AMSamplePlateItemModel *oldSamplePlateItemModel = samplePlateItemModel_;
	if(samplePlate_)
		samplePlateItemModel_ = new AMSamplePlateItemModel(samplePlate_);
	else
		samplePlateItemModel_ = 0; //NULL

	if(!sampleListView_ && samplePlateItemModel_){
		sampleListView_ = new QListView();
		sampleListView_->setAlternatingRowColors(true);
		AMSamplePlateItemDelegate *listViewDelegate = new AMSamplePlateItemDelegate();
		sampleListView_->setItemDelegate(listViewDelegate);

		vl_->addWidget(sampleListView_);
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
