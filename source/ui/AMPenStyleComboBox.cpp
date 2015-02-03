#include "AMPenStyleComboBox.h"

AMPenStyleComboBox::AMPenStyleComboBox(QWidget *parent) :
	QComboBox(parent)
{
	setToolTip(QString("Line Style"));



	for(int iPenStyle = Qt::NoPen, lastPenStyle = Qt::CustomDashLine;
		iPenStyle < lastPenStyle; ++iPenStyle)
	{
		addItem(QString(), QVariant(iPenStyle));
	}

	setItemDelegate(new AMPenStyleDelegate(this));
	setCurrentIndex(1);
	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));

}

Qt::PenStyle AMPenStyleComboBox::selectedPenStyle() const
{
	int currentPenStyleIndex = itemData(currentIndex()).toInt();

	return (Qt::PenStyle)currentPenStyleIndex;
}

void AMPenStyleComboBox::setSelectedPenStyle(Qt::PenStyle penStyle)
{
	for (int iCurrent = 0, itemListSize = count();
		 iCurrent < itemListSize; ++iCurrent)
	{
		int penStyleInteger = itemData(iCurrent).toInt();

		if((Qt::PenStyle)penStyleInteger == penStyle)
		{
			setCurrentIndex(iCurrent);
		}
	}
}


void AMPenStyleComboBox::onCurrentIndexChanged(int /*index*/)
{
	emit currentPenStyleChanged(selectedPenStyle());
}


void AMPenStyleComboBox::paintEvent(QPaintEvent*)
{
	QStylePainter stylePainter(this);
	stylePainter.setPen(palette().color(QPalette::Text));

	QStyleOptionComboBox option;
	initStyleOption(&option);
	stylePainter.drawComplexControl(QStyle::CC_ComboBox, option);

	QPainter painter(this);
	painter.save();

	QRect itemRectangle =
			stylePainter.style()->subElementRect(QStyle::SE_ComboBoxFocusRect, &option, this);

	itemRectangle.adjust(5,0,-5,0);

	QPen pen;


	if(option.state.testFlag(QStyle::State_HasFocus))
		pen.setColor(palette().color(QPalette::Active, QPalette::HighlightedText));
	else
		pen.setColor(palette().color(QPalette::Normal, QPalette::WindowText));
	pen.setWidth(2);
	pen.setStyle(selectedPenStyle());
	painter.setPen(pen);

	int middleOfRectangle = (itemRectangle.bottom() + itemRectangle.top()) /2;

	painter.drawLine(itemRectangle.left(), middleOfRectangle,
					  itemRectangle.right(), middleOfRectangle);
	painter.restore();

}


AMPenStyleDelegate::AMPenStyleDelegate(QObject *parent)
	:QItemDelegate(parent)
{
}

void AMPenStyleDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if(!index.isValid())
		return;

	QVariant variantData = index.data(Qt::UserRole);

	if(!variantData.isValid())
		return;

	bool isInt = false;

	int integerData = variantData.toInt(&isInt);

	if(!isInt)
		return;

	QRect itemRectangle = option.rect;

	itemRectangle.adjust(5,0,-5,0);

	QPen pen;

	if(option.state.testFlag(QStyle::State_HasFocus))
	{
		pen.setColor(option.palette.color(QPalette::Active, QPalette::HighlightedText));
		painter->fillRect(option.rect, option.palette.highlight());
	}
	else
	{
		pen.setColor(option.palette.color(QPalette::Normal, QPalette::WindowText));
	}
	pen.setWidth(2);
	pen.setStyle((Qt::PenStyle)integerData);

	painter->save();
	painter->setPen(pen);

	int middleOfRectangle = (itemRectangle.bottom() + itemRectangle.top()) /2;

	painter->drawLine(itemRectangle.left(), middleOfRectangle,
					  itemRectangle.right(), middleOfRectangle);

	painter->restore();
}
