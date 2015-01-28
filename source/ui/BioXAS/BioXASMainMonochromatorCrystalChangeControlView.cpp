#include "BioXASMainMonochromatorCrystalChangeControlView.h"

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

    introDisplay_ = new QDialog(this);
    introDisplay_->setWindowFlags(Qt::Widget);
    QLabel *introDisplayText = new QLabel(introMessage_);
    QDialogButtonBox *introDisplayButtons = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                                 QDialogButtonBox::Cancel);
//    actionsDisplay_ = new QWidget(this);
//    progress_ = new QProgressBar(this);
//    progress_->setMinimum(0);
//    progress_->setMaximum(10);
//    progress_->setValue(0);
//    description_ = new QLabel(this);
//    userInformation_ = new QLabel(this);

//    successDisplay_ = new QMessageBox(this);
//    successDisplay_->setText(successMessage_);
//    successDisplay_->setStandardButtons(QMessageBox::Ok);
//    successDisplay_->setDefaultButton(QMessageBox::Ok);
//    successDisplay_->setWindowFlags(Qt::Widget);

    failDisplay_ = new QDialog(this);
    failDisplay_->setWindowFlags(Qt::Widget);
    QLabel *failDisplayText = new QLabel(failMessage_);
    QDialogButtonBox *failDisplayButtons = new QDialogButtonBox(QDialogButtonBox::Ok);

    // Create and set layouts.

    QVBoxLayout *introDisplayLayout = new QVBoxLayout();
    introDisplayLayout->addWidget(introDisplayText);
    introDisplayLayout->addWidget(introDisplayButtons);
    introDisplay_->setLayout(introDisplayLayout);

    QVBoxLayout *failDisplayLayout = new QVBoxLayout();
    failDisplayLayout->addWidget(failDisplayText);
    failDisplayLayout->addWidget(failDisplayButtons);
    failDisplay_->setLayout(failDisplayLayout);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->addWidget(introDisplay_);
    layout->addWidget(failDisplay_);

    setLayout(layout);

    // Initial settings.

    showFailDisplay();

    // Make connections.

    connect( introDisplayButtons, SIGNAL(accepted()), this, SLOT(toBeginCrystalChange()) );
    connect( introDisplayButtons, SIGNAL(rejected()), this, SLOT(close()) );
    connect( failDisplayButtons, SIGNAL(accepted()), this, SLOT(close()) );

    // Current settings.

    setControl(0);
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
            connect( control_, SIGNAL(stateChanged(BioXASMainMonochromatorCrystalChangeControl::State)), this, SLOT(onControlStateChanged(BioXASMainMonochromatorCrystalChangeControl::State)) );
        }

        emit controlChanged(control_);
    }
}

void BioXASMainMonochromatorCrystalChangeControlView::toStartCrystalChange()
{
    if (control_)
        control_->startCrystalChange();
}

void BioXASMainMonochromatorCrystalChangeControlView::onControlStateChanged(BioXASMainMonochromatorCrystalChangeControl::State newState)
{
    switch (newState) {
    case BioXASMainMonochromatorCrystalChangeControl::Initialized:
        showIntroDisplay();
        break;
    case BioXASMainMonochromatorCrystalChangeControl::Running:
        break;
    case BioXASMainMonochromatorCrystalChangeControl::CompleteSuccess:
        break;
    default:
        showFailDisplay();
        break;
    }
}

void BioXASMainMonochromatorCrystalChangeControlView::onControlCurrentActionChanged(const QString &newDescription, const QString &newInformation)
{
    Q_UNUSED(newDescription)
    Q_UNUSED(newInformation)
}

void BioXASMainMonochromatorCrystalChangeControlView::onControlProgressChanged(double numerator, double denominator)
{
    Q_UNUSED(numerator)
    Q_UNUSED(denominator)
}

void BioXASMainMonochromatorCrystalChangeControlView::showIntroDisplay()
{
    // hide other displays.

    failDisplay_->hide();

    // show intro display.

    introDisplay_->show();
}

void BioXASMainMonochromatorCrystalChangeControlView::showFailDisplay()
{
    // hide other displays.

    introDisplay_->hide();

    // show fail display.

    failDisplay_->show();
}
