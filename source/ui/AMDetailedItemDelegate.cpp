#include "AMDetailedItemDelegate.h"
#include <QDebug>
#include <QApplication>
#include <QStyle>
#include <QFontMetrics>
#include <QGraphicsTextItem>
#include <QStyleOptionButton>
#include "acquaman.h"


AMDetailedItemDelegate::AMDetailedItemDelegate(QObject *parent) :
		QStyledItemDelegate(parent)
{
	// apply defaults:
	setFont();
	setTextColor();
	setFontSize();
	setItemHeight();
	setCloseButtonAction();
}

void AMDetailedItemDelegate::setItemHeight(int height){
	height_=height;
}

void AMDetailedItemDelegate::setFont(const QFont& font){
	font_=font;
	//qDebug() << "Setting font:" << font.family() << font.pointSize();
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

	/// Display close button?
	if(closeButtonEnabled_) {
		// "local" rectangle of the close button. Must add
		closeButtonRect_ = QRect(
				opt.rect.width() - 20,
				(opt.rect.height() - 13)/2,
				12,
				13);
		painter->drawPixmap(opt.rect.left() + closeButtonRect_.left(),
							opt.rect.top() + closeButtonRect_.top(),
							closeButton_);
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


#include <QEvent>
#include <QMouseEvent>

bool AMDetailedItemDelegate::editorEvent ( QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index ) {

	if(!closeButtonEnabled_)
		return QStyledItemDelegate::editorEvent(event, model, option, index);


	if(event->type() == QEvent::MouseButtonPress) {
		mouseDownPosition_ = static_cast<QMouseEvent*>(event)->pos();
	}

	if(event->type() == QEvent::MouseButtonRelease) {
		QMouseEvent* e = static_cast<QMouseEvent*>(event);
		// Normally, a "click" has to be released nearly in the same position as where it was pushed down. I should actually check that both the down and the up are within the confines of the same widget/button, but we don't really have buttons here now.
		// This is our criteria for a valid click. Now is it on a close button?
		if( (mouseDownPosition_ - e->pos()).manhattanLength() < QApplication::startDragDistance() ) {
			QPoint localPos = e->pos() - option.rect.topLeft();

			// we're on the button!
			if( closeButtonRect_.contains(localPos) ) {
				emit closeButtonClicked(index);

				// handle anything special the view might need to do on mouse-release?
				// QStyledItemDelegate::editorEvent(event, model, option, index);

				if(closeButtonAction_ == RemoveFromModel) {
					model->removeRow(index.row(), index.parent());
				}
				// given that we've acted on this, we want this event.
				return true;
			}
		}
	}

	return QStyledItemDelegate::editorEvent(event, model, option, index);
}
