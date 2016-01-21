#include "BioXASZebraLogicBlockView.h"

BioXASZebraLogicBlockView::BioXASZebraLogicBlockView(BioXASZebraLogicBlock *control, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;

	// Create UI elements.

	inputViewsLayout_ = new QVBoxLayout();

	QGroupBox *inputViewsBox = new QGroupBox();
	inputViewsBox->setTitle("Inputs");
	inputViewsBox->setLayout(inputViewsLayout_);

	outputStatusLabel_ = new QLabel();

	QHBoxLayout *outputStatusLayout = new QHBoxLayout();
	outputStatusLayout->addWidget(outputStatusLabel_);

	QGroupBox *outputStatusBox = new QGroupBox();
	outputStatusBox->setTitle("Output");
	outputStatusBox->setLayout(outputStatusLayout);

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(inputViewsBox);
	layout->addWidget(outputStatusBox);

	setLayout(layout);

	// Current settings.

	setControl(control);

	refresh();
}

BioXASZebraLogicBlockView::~BioXASZebraLogicBlockView()
{

}

void BioXASZebraLogicBlockView::refresh()
{
	updateInputViews();
	updateOutputStatusLabel();
}

void BioXASZebraLogicBlockView::setControl(BioXASZebraLogicBlock *newControl)
{
	if (control_ != newControl) {
		
		if (control_) {
			disconnect( control_, 0, this, 0 );
		}
		
		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(inputControlsSetChanged(AMControlSet*)), this, SLOT(updateInputViews()) );
			connect( control_, SIGNAL(outputStatusChanged(bool)), this, SLOT(updateOutputStatusLabel()) );
		}

		refresh();

		emit controlChanged(control_);
	}
}

void BioXASZebraLogicBlockView::updateInputViews()
{
	// Clear the input views.

	foreach (BioXASZebraLogicBlockInputView *inputView, inputViews_) {
		if (inputView) {
			inputViewsLayout_->removeWidget(inputView);
			inputViews_.removeOne(inputView);

			inputView->deleteLater();
		}
	}

	// Create new views.

	if (control_) {

		AMControlSet *inputsSet = control_->inputControlsSet();

		if (inputsSet) {

			for (int i = 0, count = inputsSet->count(); i < count; i++) {
				BioXASZebraLogicBlockInput *inputControl = qobject_cast<BioXASZebraLogicBlockInput*>(inputsSet->at(i));

				if (inputControl) {
					BioXASZebraLogicBlockInputView *inputView = new BioXASZebraLogicBlockInputView(inputControl);

					inputViewsLayout_->addWidget(inputView);
					inputViews_.append(inputView);
				}
			}
		}
	}

}

void BioXASZebraLogicBlockView::updateOutputStatusLabel()
{
	bool status = false;

	if (control_)
		status = control_->outputStatus();

	outputStatusLabel_->setPixmap(QIcon(status ? ":/22x22/greenLEDOn.png" : ":/22x22/greenLEDOff.png").pixmap(22));
}

