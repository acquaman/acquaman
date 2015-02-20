#include "BioXASMainMonochromatorCrystalChangeControlView.h"
#include <QDebug>

BioXASMainMonochromatorCrystalChangeControlView::BioXASMainMonochromatorCrystalChangeControlView(BioXASMainMonochromatorCrystalChangeControl *control, QWidget *parent) :
    QWidget(parent)
{
    Q_UNUSED(control)

    // Initialize variables.

    control_ = 0;
    introMessage_ = "Begin crystal change procedure?";
    successMessage_ = "Crystal change completed successfully!";
    failMessage_ = "Unable to complete crystal change procedure.";

    // Create UI elements.

    initializedDisplay_ = new QWidget();
    QLabel *initializedDisplayText = new QLabel(introMessage_);
    QDialogButtonBox *initializedDisplayButtons = new QDialogButtonBox(QDialogButtonBox::Ok |
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
    QLabel *completeSuccessDisplayText = new QLabel(successMessage_);
    QDialogButtonBox *completeSuccessDisplayButtons = new QDialogButtonBox(QDialogButtonBox::Ok);

    completeFailDisplay_ = new QWidget(this);
    QLabel *completeFailDisplayText = new QLabel(failMessage_);
    QDialogButtonBox *completeFailDisplayButtons = new QDialogButtonBox(QDialogButtonBox::Ok);

    // Create and set layouts.

    QVBoxLayout *initializedDisplayLayout = new QVBoxLayout();
    initializedDisplayLayout->addWidget(initializedDisplayText);
    initializedDisplayLayout->addWidget(initializedDisplayButtons);
    initializedDisplayLayout->addStretch();
    initializedDisplay_->setLayout(initializedDisplayLayout);

    QVBoxLayout *runningDisplayLayout = new QVBoxLayout();
    runningDisplayLayout->addWidget(runningDisplayProgress_);
    runningDisplayLayout->addWidget(runningDisplayDescription_);
    runningDisplayLayout->addWidget(runningDisplayInstruction_);
    runningDisplayLayout->addStretch();
    runningDisplay_->setLayout(runningDisplayLayout);

    QVBoxLayout *completeSuccessDisplayLayout = new QVBoxLayout();
    completeSuccessDisplayLayout->addWidget(completeSuccessDisplayText);
    completeSuccessDisplayLayout->addWidget(completeSuccessDisplayButtons);
    completeSuccessDisplayLayout->addStretch();
    completeSuccessDisplay_->setLayout(completeSuccessDisplayLayout);

    QVBoxLayout *completeFailDisplayLayout = new QVBoxLayout();
    completeFailDisplayLayout->addWidget(completeFailDisplayText);
    completeFailDisplayLayout->addWidget(completeFailDisplayButtons);
    completeFailDisplayLayout->addStretch();
    completeFailDisplay_->setLayout(completeFailDisplayLayout);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->addWidget(initializedDisplay_);
    layout->addWidget(runningDisplay_);
    layout->addWidget(completeSuccessDisplay_);
    layout->addWidget(completeFailDisplay_);
    setLayout(layout);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Initial settings.

    showCompleteFailDisplay();

    // Make connections.

    connect( initializedDisplayButtons, SIGNAL(accepted()), this, SLOT(toStartCrystalChange()) );
    connect( initializedDisplayButtons, SIGNAL(rejected()), this, SLOT(close()) );
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

			onControlStepChanged();
        }


        emit controlChanged(control_);
    }
}

void BioXASMainMonochromatorCrystalChangeControlView::toStartCrystalChange()
{
    if (control_)
        control_->startCrystalChange();
}

void BioXASMainMonochromatorCrystalChangeControlView::onControlStepChanged()
{

}

void BioXASMainMonochromatorCrystalChangeControlView::onControlProgressChanged(double numerator, double denominator)
{
    runningDisplayProgress_->setValue(numerator);
    runningDisplayProgress_->setMaximum(denominator);
}

void BioXASMainMonochromatorCrystalChangeControlView::onControlCrystalChangeComplete(bool success)
{
	Q_UNUSED(success)
}

void BioXASMainMonochromatorCrystalChangeControlView::showInitializedDisplay()
{
    // hide other displays.

    runningDisplay_->hide();
    completeSuccessDisplay_->hide();
    completeFailDisplay_->hide();

    // show initialized display.

    initializedDisplay_->show();
}

void BioXASMainMonochromatorCrystalChangeControlView::showRunningDisplay()
{
    // hide other displays.

    initializedDisplay_->hide();
    completeSuccessDisplay_->hide();
    completeFailDisplay_->hide();

    // Show running display.

    runningDisplay_->show();
}

void BioXASMainMonochromatorCrystalChangeControlView::showCompleteSuccessDisplay()
{
    // hide other displays.

    initializedDisplay_->hide();
    runningDisplay_->hide();
    completeFailDisplay_->hide();

    // Show complete success display.

    completeSuccessDisplay_->show();
}

void BioXASMainMonochromatorCrystalChangeControlView::showCompleteFailDisplay()
{
    // hide other displays.

    initializedDisplay_->hide();
    runningDisplay_->hide();
    completeSuccessDisplay_->hide();

    // show fail display.

    completeFailDisplay_->show();
}
