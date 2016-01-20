#include "AMToolButton.h"
#include <QStyleOption>
#include <QPainter>
#include <QStylePainter>

AMToolButton::AMToolButton(QWidget *parent) :
	QToolButton(parent)
{
	setAutoFillBackground(true);

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

//	setStyleSheet(QString("background-color: %1; "
//						  "border: 1px; "
//						  "border-color: black; "
//						  "border-radius: 3px; "
//						  "border-style: outset;").arg(newColor.name())
//				  );
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

void AMToolButton::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event)

	QStylePainter painter(this);

	QPalette palette = this->palette();
	palette.setColor(QPalette::Background, QColor(Qt::red));

	QStyleOptionToolButton buttonOption;
	initStyleOption(&buttonOption);
	buttonOption.palette = palette;

	painter.drawComplexControl(QStyle::CC_ToolButton, buttonOption);
}

