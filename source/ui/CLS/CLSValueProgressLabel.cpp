#include "CLSValueProgressLabel.h"

#include <QPainter>
#include <QPaintEvent>

CLSValueProgressLabel::CLSValueProgressLabel(QWidget *parent) :
	QLabel(parent)
{
	displayProgress_ = true;
	value_ = 0;
	valueMin_ = 0;
	valueMax_ = 100;
}

CLSValueProgressLabel::~CLSValueProgressLabel()
{

}

void CLSValueProgressLabel::setDisplayProgress(bool showProgress)
{
	if (displayProgress_ != showProgress) {
		displayProgress_ = showProgress;
		repaint();
	}
}

void CLSValueProgressLabel::setProgressValue(double newValue)
{
	if (value_ != newValue) {
		value_ = newValue;
		repaint();
	}
}

void CLSValueProgressLabel::setProgressValueMinimum(double newValue)
{
	if (valueMin_ != newValue) {
		valueMin_ = newValue;
		repaint();
	}
}

void CLSValueProgressLabel::setProgressValueMaximum(double newValue)
{
	if (valueMax_ != newValue) {
		valueMax_ = newValue;
		repaint();
	}
}

void CLSValueProgressLabel::paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);

	QRect labelRect = event->rect();
	painter.fillRect(labelRect, painter.background());

	if (displayProgress_) {
		QRect progressRect = QRect(labelRect.left(), labelRect.top(), progressPercentage(valueMin_, value_, valueMax_) * labelRect.width(), labelRect.height());
		painter.fillRect(progressRect, QBrush(QColor(224, 255, 255)));
	}

	painter.drawText(labelRect, Qt::AlignCenter, text());

	painter.end();
}

double CLSValueProgressLabel::progressPercentage(double min, double value, double max) const
{
	return qAbs(value - min) / qAbs(max - min);
}
