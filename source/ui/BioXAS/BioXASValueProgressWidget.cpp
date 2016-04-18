#include "BioXASValueProgressWidget.h"
#include <QPainter>
#include <QPaintEvent>

BioXASValueProgressWidget::BioXASValueProgressWidget(QWidget *parent) :
	QLabel(parent)
{
	displayProgress_ = true;
	value_ = 0;
	valueMin_ = 0;
	valueMax_ = 100;
}

BioXASValueProgressWidget::~BioXASValueProgressWidget()
{

}

void BioXASValueProgressWidget::setDisplayProgress(bool showProgress)
{
	if (displayProgress_ != showProgress) {
		displayProgress_ = showProgress;
		repaint();
	}
}

void BioXASValueProgressWidget::setProgressValue(double newValue)
{
	if (value_ != newValue) {
		value_ = newValue;
		repaint();
	}
}

void BioXASValueProgressWidget::setProgressValueMinimum(double newValue)
{
	if (valueMin_ != newValue) {
		valueMin_ = newValue;
		repaint();
	}
}

void BioXASValueProgressWidget::setProgressValueMaximum(double newValue)
{
	if (valueMax_ != newValue) {
		valueMax_ = newValue;
		repaint();
	}
}

void BioXASValueProgressWidget::paintEvent(QPaintEvent *event)
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

double BioXASValueProgressWidget::progressPercentage(double min, double value, double max) const
{
	return abs(value - min) / abs(max - min);
}

double BioXASValueProgressWidget::abs(double arg) const
{
	return (arg >= 0) ? arg : -1 * arg;
}
