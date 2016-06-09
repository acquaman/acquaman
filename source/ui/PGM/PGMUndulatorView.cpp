#include "PGMUndulatorView.h"

#include <QVBoxLayout>

#include "ui/beamline/AMExtendedControlEditor.h"
#include "beamline/PGM/PGMBeamline.h"

PGMUndulatorView::PGMUndulatorView(QWidget *parent) : QWidget(parent)
{

	QVBoxLayout* mainLayout = new QVBoxLayout();

	undulatorGapControlEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->undulatorGap());
	mainLayout->addWidget(undulatorGapControlEditor_);

	undulatorTrackingControlEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->undulatorTracking());
	mainLayout->addWidget(undulatorTrackingControlEditor_);

	setLayout(mainLayout);
}

