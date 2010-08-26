#include "AMDetailedItemDelegate.h"
#include <QDebug>

/*Give it setItemHeight(), setFont(), setTextColor(line1Color, line2Color), setFontSize(line1size, line2size).
Choose pretty default font colors and sizes ("Lucida Grande" is my standard application-wide pick so far;
line 1 bigger and darker than line 2). Can I add a request for AMDetailedDelegate?  If the AM::ModifiedRole
for the data is true, can you render the top line (Qt::DisplayRole data) in italic text?|*/

#include <QApplication>
#include <QStyle>
#include <QFontMetrics>
 #include <QGraphicsTextItem>
#include <QStyleOptionButton>
#include "acquaman.h"


AMDetailedItemDelegate::AMDetailedItemDelegate(QObject *parent) :
QStyledItemDelegate(parent)
{
	setFont(QFont("Lucida Grande", 10));
	setTextColor();
	setFontSize();
}

void AMDetailedItemDelegate::setItemHeight(int height){
	height_=height;
}

void AMDetailedItemDelegate::setFont(const QFont& font){
	font_=font;
	qDebug() << "Setting font:" << font.family() << font.pointSize();

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
	painter->drawText(textRect, index.data(Qt::DisplayRole).toString());
	qDebug() << painter->fontInfo().family() << painter->fontInfo().pointSize();
/*
	QStyleOptionButton option1;
		 //option1.initFrom(index);
		 option1.text = "x";

	sty->drawControl(QStyle::CE_PushButton,&option1,painter);
*/
	QVariant description = index.data(AM::DescriptionRole);
	if(!description.isNull()){
		painter->setPen(color2_);
		painter->drawText(textRect.translated(QPoint(0,20)), opt.fontMetrics.elidedText(description.toString(), Qt::ElideRight, textRect.width() ));
	}
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
