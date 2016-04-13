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

	ensureProgressValues();

	if (displayProgress_ && progressValuesOkay()) {
		QRect progressRect = QRect(labelRect.left(), labelRect.top(), labelRect.width() - (1 - double(value_)/double(valueMax_ - valueMin_)) * labelRect.width(), labelRect.height());
		painter.fillRect(progressRect, QBrush(QColor(224, 255, 255)));
	}

	painter.drawText(labelRect, Qt::AlignCenter, text());

	painter.end();
}

void BioXASValueProgressWidget::ensureProgressValues()
{
	// Swap the valueMin and valueMax if they are reversed.

	if (valueMin_ > valueMax_) {
		double temp = valueMin_;
		valueMin_ = valueMax_;
		valueMax_ = temp;
	}

	// Changes the min to be the same as the value, if the value is less.

	if (value_ < valueMin_)
		valueMin_ = value_;

	// Changes the max to be the same as the value, if the value is greater.

	if (value_ > valueMax_)
		valueMax_ = value_;
}

bool BioXASValueProgressWidget::progressValuesOkay() const
{
	return (valueMin_ < valueMax_ && value_ >= valueMin_ && value_ <= valueMax_);
}
