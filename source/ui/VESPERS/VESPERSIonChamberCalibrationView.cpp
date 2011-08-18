#include "VESPERSIonChamberCalibrationView.h"

#include "ui/VESPERS/VESPERSIonChamberView.h"

#include <QGridLayout>
#include <QLabel>

VESPERSIonChamberCalibrationView::VESPERSIonChamberCalibrationView(VESPERSIonChamberCalibration *calibration, QWidget *parent)
	: QWidget(parent)
{
	calibration_ = calibration;

	QGridLayout *calibrationLayout = new QGridLayout;
	calibrationLayout->addWidget(new QLabel("Name"), 0, 0, Qt::AlignCenter);
	calibrationLayout->addWidget(new QLabel("Sensitivity Value and Units"), 0, 1, 1, 2, Qt::AlignCenter);
	calibrationLayout->addWidget(new QLabel("Voltage (V)"), 0, 3, Qt::AlignCenter);

	calibrationLayout->addWidget(new VESPERSSplitIonChamberView(calibration_->splitIonChamber()), 1, 0, 1, 4);

	VESPERSIonChamberView *temp;

	for (int i = 0; i < calibration_->ionChamberCount(); i++){

		temp = new VESPERSIonChamberView(calibration_->ionChamberAt(i));
		calibrationLayout->addWidget(temp, i+2, 0, 1, 4);
	}

	setLayout(calibrationLayout);
	setMaximumSize(400, 300);
}
