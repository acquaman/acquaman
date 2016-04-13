#include "AMToolButton.h"
#include <QStyleOption>
#include <QPainter>
#include <QStylePainter>

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
	if (color_ != newColor) {
		color_ = newColor;
		repaint();

		emit colorChanged(color_);
	}
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

void AMToolButton::paintEvent(QPaintEvent *e)
{
	Q_UNUSED(e)

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	// Main button box.  Uses the standand push button style for the basic aspects of the box.

	QStyleOptionButton option;
	option.initFrom(this);
	option.state = QStyle::State_Off;

	if (!isEnabled()) {
		option.palette = QPalette(Qt::black, QColor(170, 170, 170, 100), Qt::gray, Qt::gray, QColor(170, 170, 170), Qt::gray, Qt::gray, Qt::gray, QColor(170, 170, 170));

	} else if (!isChecked()) {
		option.palette = QPalette(Qt::black, QColor(20, 220, 20), Qt::gray, Qt::darkGray, QColor(170, 170, 170), Qt::black, Qt::red, color_, QColor(0, 200, 0));

	} else {
		option.palette = QPalette(Qt::black, QColor(225, 225, 225, 100), Qt::gray, Qt::gray, QColor(225, 225, 225), Qt::gray, Qt::gray, Qt::gray, QColor(225, 225, 225));
	}

	style()->drawControl(QStyle::CE_PushButton, &option, &painter, this);
}

