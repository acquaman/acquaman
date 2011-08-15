#include "VESPERSBeamlineView.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "ui/VESPERS/VESPERSIntermediateSlitsView.h"
#include "ui/VESPERS/VESPERSIonChamberCalibrationView.h"
#include "ui/CLS/CLSSynchronizedDwellTimeView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

VESPERSBeamlineView::VESPERSBeamlineView(QWidget *parent) :
	QWidget(parent)
{
	VESPERSIntermediateSlitsView *slitsView = new VESPERSIntermediateSlitsView(VESPERSBeamline::vespers()->intermediateSlits());
	VESPERSIonChamberCalibrationView *ionCalibrationView = new VESPERSIonChamberCalibrationView(VESPERSBeamline::vespers()->ionChamberCalibration());
	CLSSynchronizedDwellTimeView *dwellTimeView = new CLSSynchronizedDwellTimeView(VESPERSBeamline::vespers()->synchronizedDwellTime());

	QVBoxLayout *current = new QVBoxLayout;
	current->addWidget(slitsView, 0, Qt::AlignCenter);
	current->addWidget(ionCalibrationView, 0, Qt::AlignCenter);

	QHBoxLayout *next = new QHBoxLayout;
	next->addLayout(current);
	next->addWidget(dwellTimeView);

	setLayout(next);
}
