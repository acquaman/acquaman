#include "BioXASMainMonochromatorCrystalChangeControlView.h"
#include <QDebug>

BioXASMainMonochromatorCrystalChangeControlView::BioXASMainMonochromatorCrystalChangeControlView(BioXASMainMonochromatorCrystalChangeControl *control, QWidget *parent) :
    QWidget(parent)
{
    Q_UNUSED(control)

    // Initialize variables.

    control_ = 0;

	QString introMessage = "Begin crystal change procedure?";
	QString selectMessage = "Please select desired region:";
	QString successMessage = "Crystal change completed successfully!";
	QString failMessage = "Unable to complete crystal change procedure.";

    // Create UI elements.

    initializedDisplay_ = new QWidget();
	QLabel *initializedDisplayText = new QLabel(introMessage);
	QDialogButtonBox *initializedDisplayButtons = new QDialogButtonBox(QDialogButtonBox::Ok |
																	   QDialogButtonBox::Cancel);

	selectionDisplay_ = new QWidget();
	QLabel *selectionDisplayText = new QLabel(selectMessage);
	selectionDisplayOptions_ = new QComboBox();
	selectionDisplayOptions_->addItem("A");
	selectionDisplayOptions_->addItem("B");
	QDialogButtonBox *selectionDisplayButtons = new QDialogButtonBox(QDialogButtonBox::Ok |
																	 QDialogButtonBox::Cancel);

    runningDisplay_ = new QWidget();
    runningDisplayProgress_ = new QProgressBar();
    runningDisplayProgress_->setMinimum(0);
    runningDisplayProgress_->setMaximum(10);
    runningDisplayProgress_->setValue(0);
    runningDisplayDescription_ = new QLabel();
    runningDisplayInstruction_ = new QTextEdit();
    runningDisplayInstruction_->setEnabled(false);

    completeSuccessDisplay_ = new QWidget(this);
	QLabel *completeSuccessDisplayText = new QLabel(successMessage);
    QDialogButtonBox *completeSuccessDisplayButtons = new QDialogButtonBox(QDialogButtonBox::Ok);

    completeFailDisplay_ = new QWidget(this);
	QLabel *completeFailDisplayText = new QLabel(failMessage);
    QDialogButtonBox *completeFailDisplayButtons = new QDialogButtonBox(QDialogButtonBox::Ok);

    // Create and set layouts.

    QVBoxLayout *initializedDisplayLayout = new QVBoxLayout();
    initializedDisplayLayout->addWidget(initializedDisplayText);
    initializedDisplayLayout->addWidget(initializedDisplayButtons);

    initializedDisplay_->setLayout(initializedDisplayLayout);

	QHBoxLayout *selectionDisplayWidgetsLayout = new QHBoxLayout();
	selectionDisplayWidgetsLayout->addWidget(selectionDisplayText);
	selectionDisplayWidgetsLayout->addWidget(selectionDisplayOptions_);
	QVBoxLayout *selectionDisplayLayout = new QVBoxLayout();
	selectionDisplayLayout->addLayout(selectionDisplayWidgetsLayout);
	selectionDisplayLayout->addWidget(selectionDisplayButtons);

	selectionDisplay_->setLayout(selectionDisplayLayout);

    QVBoxLayout *runningDisplayLayout = new QVBoxLayout();
    runningDisplayLayout->addWidget(runningDisplayProgress_);
    runningDisplayLayout->addWidget(runningDisplayDescription_);
    runningDisplayLayout->addWidget(runningDisplayInstruction_);

    runningDisplay_->setLayout(runningDisplayLayout);

    QVBoxLayout *completeSuccessDisplayLayout = new QVBoxLayout();
    completeSuccessDisplayLayout->addWidget(completeSuccessDisplayText);
    completeSuccessDisplayLayout->addWidget(completeSuccessDisplayButtons);

    completeSuccessDisplay_->setLayout(completeSuccessDisplayLayout);

    QVBoxLayout *completeFailDisplayLayout = new QVBoxLayout();
    completeFailDisplayLayout->addWidget(completeFailDisplayText);
    completeFailDisplayLayout->addWidget(completeFailDisplayButtons);

    completeFailDisplay_->setLayout(completeFailDisplayLayout);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->addWidget(initializedDisplay_);
    layout->addWidget(runningDisplay_);
    layout->addWidget(completeSuccessDisplay_);
    layout->addWidget(completeFailDisplay_);

    setLayout(layout);

    // Initial settings.

    showCompleteFailDisplay();

    // Make connections.

	connect( initializedDisplayButtons, SIGNAL(accepted()), this, SLOT(onInitializedDisplayAccepted()) );
    connect( initializedDisplayButtons, SIGNAL(rejected()), this, SLOT(close()) );
	connect( selectionDisplayButtons, SIGNAL(accepted()), this, SLOT(onSelectionDisplayAccepted()) );
	connect( selectionDisplayButtons, SIGNAL(rejected()), this, SLOT(close()) );
    connect( completeSuccessDisplayButtons, SIGNAL(accepted()), this, SLOT(close()) );
    connect( completeFailDisplayButtons, SIGNAL(accepted()), this, SLOT(close()) );

    // Current settings.

    setControl(control);
}

BioXASMainMonochromatorCrystalChangeControlView::~BioXASMainMonochromatorCrystalChangeControlView()
{

}

void BioXASMainMonochromatorCrystalChangeControlView::setControl(BioXASMainMonochromatorCrystalChangeControl *newControl)
{
    if (control_ != newControl) {

        if (control_)
            disconnect( control_, 0, this, 0 );

        control_ = newControl;

        if (control_) {

			connect( control_, SIGNAL(stepChanged(BioXASMainMonochromatorCrystalChangeControl::Step)), this, SLOT(onControlStepChanged()) );
            connect( control_, SIGNAL(progressChanged(double, double)), this, SLOT(onControlProgressChanged(double, double)) );
			connect( control_, SIGNAL(crystalChangeComplete(bool)), this, SLOT(onControlCrystalChangeComplete(bool)) );

			onControlStepChanged();
        }


        emit controlChanged(control_);
    }
}

void BioXASMainMonochromatorCrystalChangeControlView::setRunningDisplayDescription(const QString &newDescription)
{
	runningDisplayDescription_->setText(newDescription);
}

void BioXASMainMonochromatorCrystalChangeControlView::setRunningDisplayInstruction(const QString &newInstruction)
{
	if (newInstruction != "") {
		runningDisplayInstruction_->setText(newInstruction);
		runningDisplayInstruction_->show();

	} else {
		runningDisplayInstruction_->hide();
	}
}

void BioXASMainMonochromatorCrystalChangeControlView::onInitializedDisplayAccepted()
{
	// If the mono is in a valid region (A or B), then set the crystal change destination to be
	// the other region and begin a crystal change. If the current region isn't valid (None),
	// bring up the selection display.

	if (control_) {
		BioXASMainMonochromator::Region currentRegion = control_->mono()->region();

		if (currentRegion != BioXASMainMonochromator::None) {
			BioXASMainMonochromator::Region nextRegion;

			if (currentRegion == BioXASMainMonochromator::A)
				nextRegion = BioXASMainMonochromator::B;
			else
				nextRegion = BioXASMainMonochromator::A;

			control_->setNextRegion(nextRegion);
			startCrystalChange();

		} else {
			showSelectionDisplay();
		}
	}
}

void BioXASMainMonochromatorCrystalChangeControlView::onSelectionDisplayAccepted()
{
	// The user's region selection has been confirmed. Set the control's next region and
	// start a crystal change.

	if (control_) {
		QString selection = selectionDisplayOptions_->currentText();

		if (selection == "A")
			control_->setNextRegion(BioXASMainMonochromator::A);

		else if (selection == "B")
			control_->setNextRegion(BioXASMainMonochromator::B);
	}
}

void BioXASMainMonochromatorCrystalChangeControlView::onControlStepChanged()
{
	if (control_) {
		BioXASMainMonochromatorCrystalChangeControl::Step currentStep = control_->step();

		runningDisplayDescription_->setText(control_->stepDescription(currentStep));
		runningDisplayInstruction_->setText(control_->stepInstruction(currentStep));
	}
}

void BioXASMainMonochromatorCrystalChangeControlView::onControlProgressChanged(double numerator, double denominator)
{
    runningDisplayProgress_->setValue(numerator);
    runningDisplayProgress_->setMaximum(denominator);
}

void BioXASMainMonochromatorCrystalChangeControlView::onControlCrystalChangeComplete(bool success)
{
	if (success)
		showCompleteSuccessDisplay();
	else
		showCompleteFailDisplay();
}

void BioXASMainMonochromatorCrystalChangeControlView::startCrystalChange()
{
	// to do.
}

void BioXASMainMonochromatorCrystalChangeControlView::showInitializedDisplay()
{
	// Hide other displays.

	selectionDisplay_->hide();
    runningDisplay_->hide();
    completeSuccessDisplay_->hide();
    completeFailDisplay_->hide();

	// Show initialized display.

    initializedDisplay_->show();
}

void BioXASMainMonochromatorCrystalChangeControlView::showSelectionDisplay()
{
	// Hide other displays.

	initializedDisplay_->hide();
	runningDisplay_->hide();
	completeSuccessDisplay_->hide();
	completeFailDisplay_->hide();

	// Show selection display.

	selectionDisplay_->show();
}

void BioXASMainMonochromatorCrystalChangeControlView::showRunningDisplay()
{
	// Hide other displays.

    initializedDisplay_->hide();
	selectionDisplay_->hide();
    completeSuccessDisplay_->hide();
    completeFailDisplay_->hide();

    // Show running display.

    runningDisplay_->show();
}

void BioXASMainMonochromatorCrystalChangeControlView::showCompleteSuccessDisplay()
{
	// Hide other displays.

    initializedDisplay_->hide();
	selectionDisplay_->hide();
    runningDisplay_->hide();
    completeFailDisplay_->hide();

    // Show complete success display.

    completeSuccessDisplay_->show();
}

void BioXASMainMonochromatorCrystalChangeControlView::showCompleteFailDisplay()
{
	// Hide other displays.

    initializedDisplay_->hide();
	selectionDisplay_->hide();
    runningDisplay_->hide();
    completeSuccessDisplay_->hide();

	// Show fail display.

    completeFailDisplay_->show();
}
