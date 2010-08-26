#include "AMDetailedItemDelegate.h"

AMDetailedItemDelegate::AMDetailedItemDelegate(QObject *parent) :
	QStyledItemDelegate(parent)
{
}

#include <QApplication>
#include <QStyle>
#include <QFontMetrics>

#include "acquaman.h"


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

	painter->drawText(textRect, index.data(Qt::DisplayRole).toString());

	QVariant description = index.data(AM::DescriptionRole);
	if(!description.isNull())
		painter->drawText(textRect.translated(QPoint(0,20)), opt.fontMetrics.elidedText(description.toString(), Qt::ElideRight, textRect.width() ));

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
