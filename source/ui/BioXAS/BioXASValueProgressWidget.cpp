#include "BioXASValueProgressWidget.h"
#include <QPainter>
#include <QPaintEvent>

BioXASValueProgressWidget::BioXASValueProgressWidget(QWidget *parent) :
	QLabel(parent)
{
	displayProgress_ = true;
	value_ = 5;
	valueMin_ = 0;
	valueMax_ = 10;
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

void BioXASValueProgressWidget::setValue(double newValue)
{
	if (value_ != newValue) {
		value_ = newValue;
		repaint();
	}
}

void BioXASValueProgressWidget::setValueMinimum(double newValue)
{
	if (valueMin_ != newValue) {
		valueMin_ = newValue;
		repaint();
	}
}

void BioXASValueProgressWidget::setValueMaximum(double newValue)
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
		QRect progressRect = QRect(labelRect.left(), labelRect.top(), labelRect.width() - ((valueMax_ - valueMin_ - value_)/(valueMax_ - valueMin_)) * labelRect.width(), labelRect.height());
		painter.fillRect(progressRect, QBrush(QColor(224, 255, 255)));
	}

	painter.setPen(QPen(Qt::black));
	painter.drawText(labelRect, Qt::AlignCenter, text());

	painter.end();
}
