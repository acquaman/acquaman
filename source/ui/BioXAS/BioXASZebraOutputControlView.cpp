#include "BioXASZebraOutputControlView.h"

BioXASZebraOutputControlView::BioXASZebraOutputControlView(BioXASZebraOutputControl *outputControl, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	outputControl_ = 0;

	// Create UI elements.

	nameLabel_ = new QLabel();

	outputValueBox_ = new QSpinBox();

	outputValueLabel_ = new QLabel();

	outputStatusLabel_ = new QLabel();

	// Create and set main layout.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(nameLabel_);
	layout->addWidget(outputValueBox_);
	layout->addWidget(outputValueLabel_);
	layout->addWidget(outputStatusLabel_);

	setLayout(layout);

	// Current settings.

	setOutputControl(outputControl);

	refresh();
}

BioXASZebraOutputControlView::~BioXASZebraOutputControlView()
{

}

void BioXASZebraOutputControlView::refresh()
{
	// Clear UI elements.

	nameLabel_->clear();
	outputValueBox_->clear();
	outputValueLabel_->clear();
	outputStatusLabel_->setPixmap(QPixmap(":/32x32/greenLEDOff.png"));

	// Update elements.

	if (outputControl_) {
		nameLabel_->setText(outputControl_->name());
		outputValueBox_->setMinimum(0);
		outputValueBox_->setMaximum(63);
		outputValueBox_->setValue(outputControl_->outputValue());
		outputValueLabel_->setText(outputControl_->outputValueString());
		outputStatusLabel_->setPixmap(outputControl_->outputStatusValue() ? QPixmap(":/32x32/greenLEDOn.png") : QPixmap(":/32x32/greenLEDOff.png"));
	}
}

void BioXASZebraOutputControlView::setOutputControl(BioXASZebraOutputControl *newControl)
{
	if (outputControl_ != newControl) {

		if (outputControl_)
			disconnect( outputControl_, 0, this, 0 );

		outputControl_ = newControl;

		if (outputControl_) {
			connect( outputControl_, SIGNAL(outputValueChanged(int)), this, SLOT(refresh()) );
			connect( outputControl_, SIGNAL(outputStatusChanged(bool)), this, SLOT(refresh()) );
		}

		refresh();

		emit outputControlChanged(outputControl_);
	}
}

