#include "BioXASSideMonoCrystalChangeView.h"
#include <QDebug>

BioXASSideMonoCrystalChangeView::BioXASSideMonoCrystalChangeView(BioXASSideMonochromator *mono) :
    QDialog()
{
    // Set variables.

    mono_ = 0;
    steps_ = new BioXASSideMonoCrystalChangeSteps(0, this);

    // Create UI elements.

    stepsView_ = new BioXASSideMonoCrystalChangeStepsView(0, this);
    finalView_ = new BioXASSideMonoCrystalChangeFinalView("", this);

    // Create and set layouts.

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->addWidget(stepsView_);
    mainLayout->addWidget(finalView_);
    setLayout(mainLayout);

    // View settings.

    showStepsView();

    // Make connections.

    connect( steps_, SIGNAL(stepsSucceeded()), this, SLOT(onStepsFinished()) );
    connect( steps_, SIGNAL(stepsFailed()), this, SLOT(onStepsFailed()) );
    connect( finalView_, SIGNAL(buttonClicked()), this, SLOT(accept()) );

    // Apply mono settings.

    setMono(mono);
}

BioXASSideMonoCrystalChangeView::~BioXASSideMonoCrystalChangeView()
{

}

BioXASSideMonochromator* BioXASSideMonoCrystalChangeView::mono() const
{
    return mono_;
}

void BioXASSideMonoCrystalChangeView::setMono(BioXASSideMonochromator *newMono)
{
    if (mono_ != newMono) {

        if (mono_) {
            steps_->setActions(0);
        }

        mono_ = newMono;

        if (mono_ && mono_->isConnected()) {
            steps_->setActions(mono_->createCrystalChangeAction());
            showStepsView();

        } else if (mono_) {
            showFinalView("Mono is not connected.");
        }

        emit monoChanged(mono_);

    } else if (!mono_) {
        showFinalView("Mono not found.");
    }
}

void BioXASSideMonoCrystalChangeView::showStepsView()
{
    finalView_->hide();
    stepsView_->show();
}

void BioXASSideMonoCrystalChangeView::showFinalView(const QString &message)
{
    finalView_->setMessage(message);

    stepsView_->hide();
    finalView_->show();
}

void BioXASSideMonoCrystalChangeView::onStepsFinished()
{
    showFinalView("Mono crystal change complete.");
}

void BioXASSideMonoCrystalChangeView::onStepsFailed()
{
    showFinalView("Crystal change could not be completed.");
}









BioXASSideMonoCrystalChangeStepsView::BioXASSideMonoCrystalChangeStepsView(BioXASSideMonoCrystalChangeSteps *toView, QWidget *parent) :
    QWidget(parent)
{
    // Set variables.

    steps_ = 0;

    // Create UI elements.

    progressBar_ = new QProgressBar(this);
    stepView_ = new BioXASSideMonoCrystalChangeStepView(0, this);

    // Create and set layouts.

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(progressBar_);
    mainLayout->addWidget(stepView_);
    setLayout(mainLayout);

    // Make connections.

    connect( steps_, SIGNAL(currentStepChanged(BioXASSideMonoCrystalChangeStep*)), this, SLOT(onCurrentStepChanged(BioXASSideMonoCrystalChangeStep*)) );
    connect( steps_, SIGNAL(progressChanged(double, double)), this, SLOT(setProgress(double, double)) );

    // View settings.

    progressBar_->setMinimum(0);
    progressBar_->setMaximum(0);

    // Apply steps settings.

    setSteps(toView);
}

BioXASSideMonoCrystalChangeStepsView::~BioXASSideMonoCrystalChangeStepsView()
{

}

void BioXASSideMonoCrystalChangeStepsView::setSteps(BioXASSideMonoCrystalChangeSteps *steps)
{
    if (steps_ != steps) {

        if (steps_)
            disconnect( steps_, 0, this, 0 );

        steps_ = steps;

        if (steps_) {
            connect( steps_, SIGNAL(currentStepChanged(BioXASSideMonoCrystalChangeStep*)), this, SLOT(onCurrentStepChanged(BioXASSideMonoCrystalChangeStep*)) );
            connect( steps_, SIGNAL(progressChanged(double,double)), this, SLOT(onProgressUpdate(double, double)) );
        }

        emit stepsChanged(steps_);
    }
}

void BioXASSideMonoCrystalChangeStepsView::onCurrentStepChanged(BioXASSideMonoCrystalChangeStep *newStep)
{
    stepView_->setStep(newStep);
}

void BioXASSideMonoCrystalChangeStepsView::onProgressUpdate(double numerator, double denominator)
{
    progressBar_->setMaximum(denominator);
    progressBar_->setValue(numerator);

}









BioXASSideMonoCrystalChangeStepView::BioXASSideMonoCrystalChangeStepView(BioXASSideMonoCrystalChangeStep *toView, QWidget *parent) :
    QWidget(parent)
{
    // Set variables.

    step_ = 0;
    extensionShown_ = false;

    // Create UI elements.

    description_ = new QLabel(this);
    userInstructions_ = new QLabel(this);
    extension_ = new QGroupBox(this);

    // Create and set layouts.

    QHBoxLayout *basicLayout = new QHBoxLayout();
    basicLayout->setMargin(0);
    basicLayout->addWidget(description_);
    basicLayout->addStretch();

    QHBoxLayout *extensionLayout = new QHBoxLayout();
    extensionLayout->setMargin(0);
    extensionLayout->addWidget(userInstructions_);

    extension_->setLayout(extensionLayout);
    extension_->hide();

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(basicLayout);
    mainLayout->addWidget(extension_);

    setLayout(mainLayout);

    // Apply view settings.

    userInstructions_->setWordWrap(true);

    // Apply step settings.

    setStep(toView);
}

BioXASSideMonoCrystalChangeStepView::~BioXASSideMonoCrystalChangeStepView()
{

}

BioXASSideMonoCrystalChangeStep* BioXASSideMonoCrystalChangeStepView::step() const
{
    return step_;
}

bool BioXASSideMonoCrystalChangeStepView::extensionShown() const
{
    return extensionShown_;
}

void BioXASSideMonoCrystalChangeStepView::setStep(BioXASSideMonoCrystalChangeStep *newStep)
{
    if (step_ != newStep) {

        if (step_)
            disconnect( step_, 0, this, 0 );

        step_ = newStep;

        if (step_) {
            setDescription(step_->description());
            setUserInstructions(step_->userInstructions());

            if (userInstructions_->text() != "")
                setExtensionShown(true);

            else
                setExtensionShown(false);

            connect( step_, SIGNAL(descriptionChanged(QString)), this, SLOT(onStepDescriptionChanged(QString)) );
            connect( step_, SIGNAL(userInstructionsChanged(QString)), this, SLOT(onStepUserInstructionsChanged(QString)) );

        } else {
            setDescription("");
            setUserInstructions("");
        }

        emit stepChanged(step_);
    }
}

void BioXASSideMonoCrystalChangeStepView::setDescription(const QString &description)
{
    description_->setText(description);
}

void BioXASSideMonoCrystalChangeStepView::setUserInstructions(const QString &instructions)
{
    userInstructions_->setText(instructions);

    if (instructions == "")
        setExtensionShown(false);
    else
        setExtensionShown(true);
}

void BioXASSideMonoCrystalChangeStepView::setExtensionShown(bool isShown)
{
    if (extensionShown_ != isShown) {
        extensionShown_ = isShown;

        if (extensionShown_)
            extension_->show();

        else
            extension_->hide();
    }
}

void BioXASSideMonoCrystalChangeStepView::onStepDescriptionChanged(const QString &newDescription)
{
    setDescription(newDescription);
}

void BioXASSideMonoCrystalChangeStepView::onStepUserInstructionsChanged(const QString &newInstructions)
{
    setUserInstructions(newInstructions);
}









BioXASSideMonoCrystalChangeFinalView::BioXASSideMonoCrystalChangeFinalView(const QString &message, QWidget *parent) :
    QWidget(parent)
{
    // Create UI elements.

    message_ = new QLabel(message, this);
    button_ = new QPushButton("Ok", this);

    // Create and set layouts.

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(button_);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(message_);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    // View settings.

    message_->setWordWrap(true);
    button_->setDefault(true);

    // Make connections.

    connect( button_, SIGNAL(clicked()), this, SIGNAL(buttonClicked()) );
}

BioXASSideMonoCrystalChangeFinalView::~BioXASSideMonoCrystalChangeFinalView()
{

}

void BioXASSideMonoCrystalChangeFinalView::setMessage(const QString &message)
{
    if (message_->text() != message) {
        message_->setText(message);
        emit messageChanged(message_->text());
    }
}

