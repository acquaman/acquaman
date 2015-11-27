#include "SGMSampleChamberView.h"

SGMSampleChamberView::SGMSampleChamberView(SGMSampleChamber *sampleChamber, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	sampleChamber_ = 0;

	// Create UI elements.

	sampleChangeButton_ = new QPushButton("Change samples");

	vacuumView_ = new SGMSampleChamberVacuumView(0);

	// Create and set layouts.

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->setMargin(0);
	buttonLayout->addStretch();
	buttonLayout->addWidget(sampleChangeButton_);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addLayout(buttonLayout);
	layout->addWidget(vacuumView_);

	setLayout(layout);

	// Make connections.

	connect( sampleChangeButton_, SIGNAL(clicked()), this, SLOT(onSampleChangeButtonClicked()) );

	// Current settings.

	setSampleChamber(sampleChamber);

	refresh();
}

SGMSampleChamberView::~SGMSampleChamberView()
{

}

void SGMSampleChamberView::clear()
{
	vacuumView_->setVacuum(0);
}

void SGMSampleChamberView::refresh()
{
	// Clears the view.

	clear();

	// Update view elements.

	updateSampleChangeButton();
	updateVacuumView();
}

void SGMSampleChamberView::setSampleChamber(SGMSampleChamber *newSampleChamber)
{
	if (sampleChamber_ != newSampleChamber) {
		sampleChamber_ = newSampleChamber;

		refresh();

		emit sampleChamberChanged(sampleChamber_);
	}
}

void SGMSampleChamberView::updateSampleChangeButton()
{
	sampleChangeButton_->setEnabled(false);
}

void SGMSampleChamberView::updateVacuumView()
{
	SGMSampleChamberVacuum *vacuumControl = 0;

	if (sampleChamber_) {
		vacuumControl = sampleChamber_->vacuum();
	}

	vacuumView_->setVacuum(vacuumControl);
}

void SGMSampleChamberView::onSampleChangeButtonClicked()
{

}

