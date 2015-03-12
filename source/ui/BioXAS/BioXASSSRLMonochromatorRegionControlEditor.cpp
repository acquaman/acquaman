#include "BioXASSSRLMonochromatorRegionControlEditor.h"
#include "beamline/BioXAS/BioXASMainBeamline.h"

BioXASSSRLMonochromatorRegionControlEditor::BioXASSSRLMonochromatorRegionControlEditor(BioXASSSRLMonochromatorRegionControl *regionControl, QWidget *parent) :
	AMExtendedControlEditor(regionControl, 0, false, false, parent)
{
	if (control_->isEnum())
		qDebug() << "\n\nHey, control is an enum:" << control_->enumNames() << "\n\n";

	connect( control_, SIGNAL(moveStarted()), this, SLOT(onRegionControlMoveStarted()) );
}

BioXASSSRLMonochromatorRegionControlEditor::~BioXASSSRLMonochromatorRegionControlEditor()
{

}

void BioXASSSRLMonochromatorRegionControlEditor::onControlEnumChanged()
{
	if (control_->isEnum())
		qDebug() << "\n\nHey, control is an enum:" << control_->enumNames() << "\n\n";
	else
		qDebug() << "\n\nHey, control is not an enum.\n\n";

	AMExtendedControlEditor::onControlEnumChanged();
}

void BioXASSSRLMonochromatorRegionControlEditor::onRegionControlMoveStarted()
{
	BioXASSSRLMonochromatorRegionControlMovingView *movingView = new BioXASSSRLMonochromatorRegionControlMovingView(BioXASMainBeamline::bioXAS()->mono()->regionControl(), this);
	movingView->setWindowFlags(Qt::Sheet);
	movingView->setWindowModality(Qt::WindowModal);
	movingView->show();
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
	movingInstruction_->setWordWrap(true);

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
			connect( regionControl_, SIGNAL(moveStepChanged(QString, QString)), this, SLOT(onMoveStepChanged(QString, QString)) );
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
	movingProgress_->setValue(numerator);
	movingProgress_->setMinimum(0);
	movingProgress_->setMaximum(denominator);
}

void BioXASSSRLMonochromatorRegionControlMovingView::onMoveStepChanged(const QString &newDescription, const QString &newInstruction)
{
	movingDescription_->setText(newDescription);

	// we only display the instruction label if there is an instruction associated
	// with this step.

	if (newInstruction != "") {
		movingInstruction_->setText(newInstruction);
		movingInstruction_->updateGeometry();
		movingInstruction_->show();

	} else {
		movingInstruction_->hide();
		movingInstruction_->updateGeometry();
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
