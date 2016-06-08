#include "PGMGratingView.h"

#include <QVBoxLayout>

#include "ui/beamline/AMExtendedControlEditor.h"
#include "beamline/PGM/PGMBeamline.h"

PGMGratingView::PGMGratingView(QWidget *parent) : QWidget(parent)
{

	QVBoxLayout* mainLayout = new QVBoxLayout();

	// TODO: Ensure with Lucia if they wish this to be present. It ain't on the EDM screen.
	gratingTrackingControlEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->gratingTracking());
	mainLayout->addWidget(gratingTrackingControlEditor_);

	setLayout(mainLayout);
}

