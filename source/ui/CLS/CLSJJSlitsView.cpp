#include "CLSJJSlitsView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QLabel>

#include "beamline/CLS/CLSJJSlits.h"
#include "ui/beamline/AMExtendedControlEditor.h"

CLSJJSlitsView::CLSJJSlitsView(CLSJJSlits *jjSlits, QWidget *parent) :
	QWidget(parent)
{
	// Initialize member variables.

	jjSlits_ = 0;

	// Create UI elements.

	verticalGapEditor_ = new AMExtendedControlEditor(0);
	verticalGapEditor_->setTitle("");
	verticalGapEditor_->setControlFormat('f', 3);
	verticalGapEditor_->hideBorder();

	verticalCenterEditor_ = new AMExtendedControlEditor(0);
	verticalCenterEditor_->setTitle("");
	verticalCenterEditor_->setControlFormat('f', 3);
	verticalCenterEditor_->hideBorder();

	horizontalGapEditor_ = new AMExtendedControlEditor(0);
	horizontalGapEditor_->setTitle("");
	horizontalGapEditor_->setControlFormat('f', 3);
	horizontalGapEditor_->hideBorder();

	horizontalCenterEditor_ = new AMExtendedControlEditor(0);
	horizontalCenterEditor_->setTitle("");
	horizontalCenterEditor_->setControlFormat('f', 3);
	horizontalCenterEditor_->hideBorder();

	// Create and set layouts.

	QGridLayout *contentGridLayout = new QGridLayout;
	contentGridLayout->addWidget(new QLabel("Direction"),  0, 0, 1, 1, Qt::AlignRight);
	contentGridLayout->addWidget(new QLabel("Gap"),        0, 1, 1, 1, Qt::AlignCenter);
	contentGridLayout->addWidget(new QLabel("Center"),     0, 2, 1, 1, Qt::AlignCenter);

	contentGridLayout->addWidget(new QLabel("Vertical"),   1, 0, 1, 1, Qt::AlignRight);
	contentGridLayout->addWidget(verticalGapEditor_,       1, 1, 1, 1);
	contentGridLayout->addWidget(verticalCenterEditor_,    1, 2, 1, 1);

	contentGridLayout->addWidget(new QLabel("Horizontal"), 2, 0, 1, 1, Qt::AlignRight);
	contentGridLayout->addWidget(horizontalGapEditor_,     2, 1, 1, 1);
	contentGridLayout->addWidget(horizontalCenterEditor_,  2, 2, 1, 1);

	setLayout(contentGridLayout);

	// Current settings.

	setControl(jjSlits);
}

CLSJJSlitsView::~CLSJJSlitsView()
{

}

void CLSJJSlitsView::refresh()
{
	// Clear the view.

	verticalGapEditor_->setControl(0);
	verticalCenterEditor_->setControl(0);
	horizontalGapEditor_->setControl(0);
	horizontalCenterEditor_->setControl(0);

	// Update view elements.

	if (jjSlits_) {
		verticalGapEditor_->setControl(jjSlits_->verticalGap());
		verticalCenterEditor_->setControl(jjSlits_->verticalCenter());
		horizontalGapEditor_->setControl(jjSlits_->horizontalGap());
		horizontalCenterEditor_->setControl(jjSlits_->horizontalCenter());
	}
}

void CLSJJSlitsView::setControl(CLSJJSlits *newSlits)
{
	if (jjSlits_ != newSlits) {
		jjSlits_ = newSlits;
		refresh();

		emit controlChanged(jjSlits_);
	}
}

void CLSJJSlitsView::setDataRange(double maxValue, double minValue)
{
	verticalGapEditor_->setRange(maxValue, minValue);
	verticalCenterEditor_->setRange(maxValue, minValue);
	horizontalGapEditor_->setRange(maxValue, minValue);
	horizontalCenterEditor_->setRange(maxValue, minValue);
}
