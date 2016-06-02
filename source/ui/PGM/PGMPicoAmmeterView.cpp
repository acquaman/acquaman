#include "PGMPicoAmmeterView.h"

#include <QHBoxLayout>

PGMPicoAmmeterView::PGMPicoAmmeterView(AMReadOnlyPVControl *control, QWidget *parent)
	: QWidget(parent)
{
	control_ = control;

	description_ = new QLabel(QString("%1:").arg(control_->description()));
	value_ = new QLabel;

	connect(control_, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
	connect(control_, SIGNAL(unitsChanged(QString)), this, SLOT(onValueChanged()));

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(description_, 0, Qt::AlignLeft);
	layout->addWidget(value_, 0, Qt::AlignRight);

	setLayout(layout);
}

void PGMPicoAmmeterView::onValueChanged()
{
	value_->setText(QString("%1 %2").arg(control_->value()).arg(control_->units()));
}

