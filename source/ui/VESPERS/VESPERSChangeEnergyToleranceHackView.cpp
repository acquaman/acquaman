#include "VESPERSChangeEnergyToleranceHackView.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include <QTimer>
#include <QHBoxLayout>
#include <QString>

VESPERSChangeEnergyToleranceHackView::VESPERSChangeEnergyToleranceHackView(QWidget *parent) :
	QWidget(parent)
{
	setpoint_ = new QDoubleSpinBox;
	setpoint_->setSuffix(" eV");
	setpoint_->setRange(0, 30000);
	setpoint_->setValue(VESPERSBeamline::vespers()->mono()->tolerance());
	connect(setpoint_, SIGNAL(editingFinished()), this, SLOT(setTolerance()));

	feedback_ = new QLabel(QString("%1 eV").arg(VESPERSBeamline::vespers()->mono()->tolerance(), 0, 'f', 2));

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(new QLabel("Energy Tolerance:"));
	layout->addWidget(setpoint_);
	layout->addWidget(feedback_);

	setLayout(layout);
}

void VESPERSChangeEnergyToleranceHackView::setTolerance()
{
	VESPERSBeamline::vespers()->mono()->setTolerance(setpoint_->value());
	QTimer::singleShot(500, this, SLOT(updateFeedback()));
}

void VESPERSChangeEnergyToleranceHackView::updateFeedback()
{
	feedback_->setText(QString("%1 eV").arg(VESPERSBeamline::vespers()->mono()->tolerance(), 0, 'f', 2));
}
