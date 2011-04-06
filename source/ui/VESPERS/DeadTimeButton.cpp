#include "DeadTimeButton.h"
#include <QPainter>
#include <QStyleOption>

DeadTimeButton::DeadTimeButton(double good, double bad, QWidget *parent)
	: QToolButton(parent)
{
	good_ = good;
	bad_ = bad;
	current_ = 0;
}

void DeadTimeButton::paintEvent(QPaintEvent *e)
{
	Q_UNUSED(e)

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	// Main button box.  Uses the standand push button style for the basic aspects of the box.
	QStyleOptionButton option;
	option.initFrom(this);
	option.state = QStyle::State_Off;

	if (current() < good())
		setPalette(QPalette(Qt::black, QColor(20, 220, 20), Qt::gray, Qt::darkGray, QColor(170, 170, 170), Qt::black, Qt::red, Qt::green, QColor(0, 200, 0)));
	else if (current() >= good() && current() < bad())
		setPalette(QPalette(Qt::black, QColor(220, 220, 20), Qt::gray, Qt::darkGray, QColor(170, 170, 170), Qt::black, Qt::red, Qt::yellow, QColor(200, 200, 0)));
	else
		setPalette(QPalette(Qt::black, QColor(220, 20, 20), Qt::gray, Qt::darkGray, QColor(170, 170, 170), Qt::black, Qt::red, Qt::red, QColor(200, 0, 0)));

	if (!isChecked())
		option.palette = palette();
	else
		option.palette = QPalette(Qt::black, QColor(225, 225, 225, 100), Qt::gray, Qt::gray, QColor(225, 225, 225), Qt::gray, Qt::gray, Qt::gray, QColor(225, 225, 225));

	style()->drawControl(QStyle::CE_PushButton, &option, &painter, this);
}
