#include "BioXASZebraOutputControlView.h"

BioXASZebraOutputControlView::BioXASZebraOutputControlView(BioXASZebraOutputControl *outputControl, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	outputControl_ = 0;

	// Create UI elements.

	nameLabel_ = new QLabel();
	nameLabel_->setMinimumWidth(100);

	outputValueBox_ = new QSpinBox();
	outputValueBox_->setMinimumWidth(100);
	connect( outputValueBox_, SIGNAL(valueChanged(int)), this, SLOT(onOutputValueBoxChanged()) );

	outputValueLabel_ = new QLabel();
	outputValueLabel_->setMinimumWidth(100);

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
	outputValueBox_->setEnabled(false);
	outputValueLabel_->clear();
	outputStatusLabel_->setPixmap(QIcon(":/22x22/greenLEDOff.png").pixmap(22));

	// Update elements.

	if (outputControl_) {
		nameLabel_->setText(outputControl_->name());
		outputValueBox_->setEnabled(true);
		outputValueBox_->setMinimum(0);
		outputValueBox_->setMaximum(63);
		outputValueBox_->setValue(outputControl_->outputValue());
		outputValueLabel_->setText(outputControl_->outputValueString());
		outputStatusLabel_->setPixmap(QIcon(outputControl_->isOutputStateHigh() ? ":/22x22/greenLEDOn.png" : ":/32x32/greenLEDOff.png").pixmap(22));
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
			connect( outputControl_, SIGNAL(outputStatusChanged(double)), this, SLOT(refresh()) );
		}

		refresh();

		emit outputControlChanged(outputControl_);
	}
}

void BioXASZebraOutputControlView::onOutputValueBoxChanged()
{
	if (outputControl_)
		outputControl_->setOutputValue(outputValueBox_->value());
}

