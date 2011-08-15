#include "VESPERSBeamlineView.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "ui/VESPERS/VESPERSIntermediateSlitsView.h"
#include "ui/VESPERS/VESPERSIonChamberCalibrationView.h"

#include <QVBoxLayout>

VESPERSBeamlineView::VESPERSBeamlineView(QWidget *parent) :
	QWidget(parent)
{
	VESPERSIntermediateSlitsView *slitsView = new VESPERSIntermediateSlitsView(VESPERSBeamline::vespers()->intermediateSlits());
	VESPERSIonChamberCalibrationView *ionCalibrationView = new VESPERSIonChamberCalibrationView(VESPERSBeamline::vespers()->ionChamberCalibration());

	QVBoxLayout *current = new QVBoxLayout;
	current->addWidget(slitsView, 0, Qt::AlignCenter);
	current->addWidget(ionCalibrationView, 0, Qt::AlignCenter);

	setLayout(current);
}
