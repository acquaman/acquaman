#include "AMToolButton.h"
#include <QToolTip>
#include "math.h"

AMToolButton::AMToolButton(QWidget *parent) :
	QToolButton(parent)
{
	color_ = QColor();
}

AMToolButton::~AMToolButton()
{

}

void AMToolButton::setColor(const QColor &newColor)
{
	setStyleSheet(QString("* { color: black; background: qlineargradient( x1:0 y1:0, x2:0 y2:1, stop:0 %1, stop:1 %2); border-style: outset; border-radius: 5px; border-width: 2px; border-color: white; min width: 10em; padding: 6px; }").arg(newColor.name()).arg(newColor.darker().name()));
}

void AMToolButton::setColorToYellow()
{
	setColor(Qt::yellow);
}

void AMToolButton::setColorToRed()
{
	setColor(Qt::red);
}

void AMToolButton::setColorToGreen()
{
	setColor(Qt::green);
}
