#include "AMColoredTextToolButton.h"

AMColoredTextToolButton::AMColoredTextToolButton(const QColor& color, QWidget *parent) :
	QToolButton(parent)
{
	setTextColor(color);
}

void AMColoredTextToolButton::setTextColor(const QColor &color) {
	textColor_ = color;
	setStyleSheet(QString("AMColoredTextToolButton { color: rgba(%1, %2, %3, %4); }")
								.arg(color.red())
								.arg(color.green())
								.arg(color.blue())
								.arg(color.alpha()));
}
