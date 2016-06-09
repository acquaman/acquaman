#include "PGMPicoAmmeterView.h"

#include <QHBoxLayout>

PGMPicoAmmeterView::PGMPicoAmmeterView(PGMPicoAmmeter *picoAmmeter, QWidget *parent)
	: QWidget(parent)
{
	picoAmmeter_ = picoAmmeter;

	description_ = new QLabel(QString("%1:").arg(picoAmmeter_->description()));
	value_ = new QLabel;

	connect(picoAmmeter_, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(description_, 0, Qt::AlignLeft);
	layout->addWidget(value_, 0, Qt::AlignRight);

	setLayout(layout);
}

void PGMPicoAmmeterView::onValueChanged()
{
	value_->setText(QString("%1 %2").arg(double(picoAmmeter_->singleReading())).arg(picoAmmeter_->units()));
}

