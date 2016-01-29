#include "AMToolButton.h"

AMToolButton::AMToolButton(QWidget *parent) :
	QToolButton(parent)
{

}

AMToolButton::~AMToolButton()
{

}

void AMToolButton::setColor(const QColor &newColor)
{
	setPalette(QPalette(newColor));
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

