#include "AMHeaderButton.h"

#define AMHEADERBUTTON_STYLE_SHEET "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(233, 233, 233, 255), stop:0.502513 rgba(199, 198, 198, 255), stop:1 rgba(163, 162, 162, 255)); border: 1px solid black; border-top-color: rgb(180, 180, 180); border-bottom-color: rgb(121, 121, 121); border-left-color: rgb(180, 180, 180); border-right-color: rgb(180, 180, 180);"

AMHeaderButton::AMHeaderButton(QWidget *parent) :
	QToolButton(parent)
{
	setStyleSheet(AMHEADERBUTTON_STYLE_SHEET);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	setMaximumHeight(16);
}


#include <QPaintEvent>
#include <QPainter>

#include <QDebug>

void AMHeaderButton::paintEvent(QPaintEvent * event) {

	QToolButton::paintEvent(event);

	QRect dest = rect();
	dest.setLeft(dest.left()+4);
	dest.setHeight(16);
	dest.setWidth(16);

	QPainter p(this);
	p.drawPixmap(dest, arrowPix);
	p.end();
}

void AMHeaderButton::setArrowType(Qt::ArrowType type) {
	arrowType_ = type;
	switch(type) {
	case Qt::NoArrow:
		arrowPix = QPixmap();
		break;

	case Qt::LeftArrow:
		arrowPix = QPixmap(":/22x22/arrow-white-right.png").scaledToHeight(16, Qt::SmoothTransformation);
		break;
	case Qt::RightArrow:
		arrowPix = QPixmap(":/22x22/arrow-white-right.png").scaledToHeight(16, Qt::SmoothTransformation);
		break;
	case Qt::UpArrow:
		arrowPix = QPixmap(":/22x22/arrow-white-down.png").scaledToHeight(16, Qt::SmoothTransformation);
		break;
	case Qt::DownArrow:
		arrowPix = QPixmap(":/22x22/arrow-white-down.png").scaledToHeight(16, Qt::SmoothTransformation);
		break;
	}

	QToolButton::setArrowType(Qt::NoArrow);
	update();
}
