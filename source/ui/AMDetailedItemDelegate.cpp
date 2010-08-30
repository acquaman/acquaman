#include "AMDetailedItemDelegate.h"
#include <QStyle>
#include <QApplication>
#include <QFontMetrics>
#include <QStyleOptionButton>
#include "acquaman.h"


AMDetailedItemDelegate::AMDetailedItemDelegate(QObject *parent) :
		AMCloseItemDelegate(parent)
{
	// apply defaults:
	setFont();
	setTextColor();
	setFontSize();
	setItemHeight();
}

void AMDetailedItemDelegate::setItemHeight(int height){
	height_=height;
}

void AMDetailedItemDelegate::setFont(const QFont& font){
	font_=font;
}

void AMDetailedItemDelegate::setTextColor(const QColor &color1, const QColor &color2){
	color1_ = color1;
	color2_ = color2;
}

void AMDetailedItemDelegate::setFontSize(int size1,int size2){
	size1_=size1;
	size2_=size2;
}


void AMDetailedItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const  {

	QStyleOptionViewItemV4 opt(option);
	initStyleOption(&opt, index);


	QStyle* sty = QApplication::style();

	// Draw the background: (this will handle selection for us. You can also probe selection directly with opt.state & QStyle::State_Selected)
	sty->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter);


	int textStartingPoint = horizontalMargin();
	// Do we have a pixmap available?
	QRect pmap = opt.rect;
	pmap.setLeft( pmap.left() + horizontalMargin() );
	pmap.setTop( pmap.top() + verticalMargin() );
	pmap.setBottom( pmap.bottom() - verticalMargin() );

	if(opt.features & QStyleOptionViewItemV2::HasDecoration) {
		QPixmap p = opt.icon.pixmap(pmap.height());
		sty->drawItemPixmap(painter, pmap, Qt::AlignLeft | Qt::AlignVCenter, p);
		textStartingPoint += horizontalMargin() + p.width() + horizontalSpacing();
	}

	QRect textRect = opt.rect;
	textRect.setLeft( textRect.left() + textStartingPoint);

	painter->setFont(font_);
	painter->setPen(color1_);

	if (AM::ModifiedRole){
		QFont fonti=font_;
		fonti.setItalic(true);
		painter->setFont(fonti);
		painter->drawText(textRect, index.data(Qt::DisplayRole).toString());
	}
	else painter->drawText(textRect, index.data(Qt::DisplayRole).toString());

	QVariant modifiedTime = index.data(AM::DateTimeRole);
	AMDateTimeUtils modifier;

	QVariant description = index.data(AM::DescriptionRole);
	if(!description.isNull()){
		painter->setFont(font_);
		painter->setPen(color2_);
		//	painter->drawText(textRect.translated(QPoint(0,20)), opt.fontMetrics.elidedText(description.toString(), Qt::ElideRight, textRect.width() ));
		painter->drawText(textRect.translated(QPoint(0,20)), opt.fontMetrics.elidedText(modifier.checkAndModifyDate(modifiedTime.toDateTime()), Qt::ElideRight, textRect.width() ));

	}

	/// call base class to draw close button:
	drawCloseButton(painter, opt, index);


	/* What info is available:
enum OptionType
enum Position
enum StyleOptionType
enum StyleOptionVersion
enum ViewItemFeature
flags ViewItemFeatures
enum ViewItemPosition
QStyleOptionViewItemV4 ()
QStyleOptionViewItemV4 ( const QStyleOptionViewItemV4 & )
QStyleOptionViewItemV4 ( const QStyleOptionViewItem & )
backgroundBrush : QBrush
checkState : Qt::CheckState
decorationAlignment : Qt::Alignment
decorationPosition : Position
decorationSize : QSize
direction : Qt::LayoutDirection
displayAlignment : Qt::Alignment
features : ViewItemFeatures
font : QFont
fontMetrics : QFontMetrics
icon : QIcon
index : QModelIndex
initFrom ( const QWidget * )
locale : QLocale
palette : QPalette
rect : QRect
showDecorationSelected : bool
state : QStyle::State
text : QString
textElideMode : Qt::TextElideMode
type : int
version : int
viewItemPosition : ViewItemPosition
widget : const QWidget *
operator= ( const QStyleOptionViewItem & ) : QStyleOption
*/
}



