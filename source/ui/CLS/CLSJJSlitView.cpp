#include "CLSJJSlitView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QLabel>

#include "beamline/CLS/CLSJJSlit.h"
#include "beamline/BioXAS/BioXASSideBeamline.h"
#include "ui/beamline/AMExtendedControlEditor.h"

CLSJJSlitView::CLSJJSlitView(CLSJJSlits *jjSlits, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	jjSlits_ = 0;

	// Create UI elements.

	verticalGapEditor_ = new AMExtendedControlEditor(0);
	verticalGapEditor_->setControlFormat('f', 3);
	verticalGapEditor_->hideBorder();

	verticalCenterEditor_ = new AMExtendedControlEditor(0);
	verticalCenterEditor_->setControlFormat('f', 3);
	verticalCenterEditor_->hideBorder();

	horizontalGapEditor_ = new AMExtendedControlEditor(0);
	horizontalGapEditor_->setControlFormat('f', 3);
	horizontalGapEditor_->hideBorder();

	horizontalCenterEditor_ = new AMExtendedControlEditor(0);
	horizontalCenterEditor_->setControlFormat('f', 3);
	horizontalCenterEditor_->hideBorder();

	// Create and set layouts.

	QGridLayout *contentGridLayout = new QGridLayout;
	contentGridLayout->addWidget(new QLabel("Direction"),  0, 0, 1, 1);
	contentGridLayout->addWidget(new QLabel("Gap"),        0, 1, 1, 1);
	contentGridLayout->addWidget(new QLabel("Center"),     0, 2, 1, 1);

	contentGridLayout->addWidget(new QLabel("Vertical"),   1, 0, 1, 1);
	contentGridLayout->addWidget(verticalGapEditor_,       1, 1, 1, 1);
	contentGridLayout->addWidget(verticalCenterEditor_,    1, 2, 1, 1);

	contentGridLayout->addWidget(new QLabel("Horizontal"), 2, 0, 1, 1);
	contentGridLayout->addWidget(horizontalGapEditor_,     2, 1, 1, 1);
	contentGridLayout->addWidget(horizontalCenterEditor_,  2, 2, 1, 1);

	setLayout(contentGridLayout);

	// Current settings.

	setJJSlits(jjSlits);
}

CLSJJSlitView::~CLSJJSlitView()
{
}

void CLSJJSlitView::setJJSlits(CLSJJSlits *newSlits)
{
	if (jjSlits_ != newSlits) {

		if (jjSlits_) {
			verticalGapEditor_->setControl(0);
			verticalCenterEditor_->setControl(0);
			horizontalGapEditor_->setControl(0);
			horizontalCenterEditor_->setControl(0);
		}

		jjSlits_ = newSlits;

		if (jjSlits_) {
			verticalGapEditor_->setControl(jjSlits_->verticalGapControl());
			verticalCenterEditor_->setControl(jjSlits_->verticalCenterControl());
			horizontalGapEditor_->setControl(jjSlits_->horizontalGapControl());
			horizontalCenterEditor_->setControl(jjSlits_->horizontalCenterControl());
		}

		emit jjSlitsChanged(jjSlits_);
	}
}
