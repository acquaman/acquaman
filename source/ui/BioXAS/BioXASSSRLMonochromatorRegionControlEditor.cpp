#include "BioXASSSRLMonochromatorRegionControlEditor.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorRegionControl.h"

BioXASSSRLMonochromatorRegionControlEditor::BioXASSSRLMonochromatorRegionControlEditor(BioXASSSRLMonochromatorRegionControl *regionControl, QWidget *parent) :
	CLSControlEditor(regionControl, parent)
{

}

BioXASSSRLMonochromatorRegionControlEditor::~BioXASSSRLMonochromatorRegionControlEditor()
{

}

void BioXASSSRLMonochromatorRegionControlEditor::setControl(AMControl *newControl)
{
	if (control_) {
		disconnect( control_, 0, this, 0 );
	}

	CLSControlEditor::setControl(newControl);

	if (control_) {
		connect( control_, SIGNAL(moveStarted()), this, SLOT(onRegionControlMoveStarted()) );
	}
}

void BioXASSSRLMonochromatorRegionControlEditor::onRegionControlMoveStarted()
{
	if (control_ && control_->isConnected()) {
		BioXASSSRLMonochromatorRegionControlMovingView *movingView = new BioXASSSRLMonochromatorRegionControlMovingView(qobject_cast<BioXASSSRLMonochromatorRegionControl*>(control_), this);
		movingView->setWindowModality(Qt::WindowModal);
		movingView->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
		movingView->show();
	}
}










BioXASSSRLMonochromatorRegionControlMovingView::BioXASSSRLMonochromatorRegionControlMovingView(BioXASSSRLMonochromatorRegionControl *regionControl, QWidget *parent) :
	QWidget(parent)
{
	// Initialize variables.

	regionControl_ = 0;

	// Create UI elements.

	movingView_ = new QWidget();
	movingProgress_ = new QProgressBar();
	movingDescription_ = new QLabel();
	movingInstruction_ = new QLabel();
	movingNotes_ = new QLabel();

	failedView_ = new QWidget();
	failedDescription_ = new QLabel();
	QDialogButtonBox *failedButtons = new QDialogButtonBox(QDialogButtonBox::Ok);

	succeededView_ = new QWidget();
	QLabel *succeededMessage = new QLabel("Region change completed successfully!");
	QDialogButtonBox *succeededButtons = new QDialogButtonBox(QDialogButtonBox::Ok);

	// Create and set layouts.

	QVBoxLayout *movingLayout = new QVBoxLayout();
	movingLayout->setMargin(0);
	movingLayout->addWidget(movingProgress_);
	movingLayout->addWidget(movingDescription_);
	movingLayout->addWidget(movingInstruction_);
	movingLayout->addWidget(movingNotes_);

	movingView_->setLayout(movingLayout);

	QVBoxLayout *failedLayout = new QVBoxLayout();
	failedLayout->setMargin(0);
	failedLayout->addWidget(failedDescription_);
	failedLayout->addWidget(failedButtons);

	failedView_->setLayout(failedLayout);

	QVBoxLayout *succeededLayout = new QVBoxLayout();
	succeededLayout->setMargin(0);
	succeededLayout->addWidget(succeededMessage);
	succeededLayout->addWidget(succeededButtons);

	succeededView_->setLayout(succeededLayout);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(movingView_);
	layout->addWidget(failedView_);
	layout->addWidget(succeededView_);

	setLayout(layout);

	// Initial settings.

	movingView_->hide();
	failedView_->hide();
	succeededView_->hide();

	// Make connections.

	connect( failedButtons, SIGNAL(accepted()), this, SLOT(close()) );
	connect( succeededButtons, SIGNAL(accepted()), this, SLOT(close()) );

	// Current settings.

	setRegionControl(regionControl);
}

BioXASSSRLMonochromatorRegionControlMovingView::~BioXASSSRLMonochromatorRegionControlMovingView()
{

}

void BioXASSSRLMonochromatorRegionControlMovingView::setRegionControl(BioXASSSRLMonochromatorRegionControl *newControl)
{
	if (regionControl_ != newControl) {

		if (regionControl_)
			disconnect( regionControl_, 0, this, 0 );

		regionControl_ = newControl;

		if (regionControl_) {
			connect( regionControl_, SIGNAL(moveStarted()), this, SLOT(onMoveStarted()) );
			connect( regionControl_, SIGNAL(moveFailed(int)), this, SLOT(onMoveFailed(int)) );
			connect( regionControl_, SIGNAL(moveSucceeded()), this, SLOT(onMoveSucceeded()) );
			connect( regionControl_, SIGNAL(moveProgressChanged(double,double)), this, SLOT(onMoveProgressChanged(double, double)) );
			connect( regionControl_, SIGNAL(moveStepChanged(QString, QString, QString)), this, SLOT(onMoveStepChanged(QString, QString, QString)) );
		}

		onMoveStarted();

		emit regionControlChanged(regionControl_);
	}
}

void BioXASSSRLMonochromatorRegionControlMovingView::onMoveStarted()
{
	if (regionControl_ && regionControl_->moveInProgress())
		showMovingView();
}

void BioXASSSRLMonochromatorRegionControlMovingView::onMoveFailed(int failureCode)
{
	showFailedView(AMControl::failureExplanation(failureCode));
}

void BioXASSSRLMonochromatorRegionControlMovingView::onMoveSucceeded()
{
	showSucceededView();
}

void BioXASSSRLMonochromatorRegionControlMovingView::onMoveProgressChanged(double numerator, double denominator)
{
	movingProgress_->setValue((int)numerator);
	movingProgress_->setMinimum(0);
	movingProgress_->setMaximum((int)denominator);
}

void BioXASSSRLMonochromatorRegionControlMovingView::onMoveStepChanged(const QString &newDescription, const QString &newInstruction, const QString &newNotes)
{
	movingDescription_->setText(newDescription);

	// we only display the instructions if there is an instruction associated with this step.

	if (newInstruction != "") {
		movingInstruction_->setText(newInstruction);
		movingInstruction_->show();

	} else {
		movingInstruction_->hide();
	}

	// we only display the notes if there are notes associated with this step.

	if (newNotes != "") {
		movingNotes_->setText(newNotes);
		movingNotes_->show();
	} else {
		movingNotes_->hide();
	}
}

void BioXASSSRLMonochromatorRegionControlMovingView::showMovingView()
{
	// Hide other views.

	failedView_->hide();
	succeededView_->hide();

	// Show moving view.

	movingView_->show();
}

void BioXASSSRLMonochromatorRegionControlMovingView::showFailedView(const QString &message)
{
	// Hide other views.

	movingView_->hide();
	succeededView_->hide();

	// Set failure message.

	failedDescription_->setText(message);

	// Show failed view.

	failedView_->show();
}

void BioXASSSRLMonochromatorRegionControlMovingView::showSucceededView()
{
	// Hide other views.

	movingView_->hide();
	failedView_->hide();

	// Show succeeded view.

	succeededView_->show();
}
