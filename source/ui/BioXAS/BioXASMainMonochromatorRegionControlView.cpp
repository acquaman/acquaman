#include "BioXASMainMonochromatorRegionControlView.h"

BioXASMainMonochromatorRegionControlView::BioXASMainMonochromatorRegionControlView(BioXASMainMonochromatorRegionControl *regionControl, QWidget *parent) :
	QWidget(parent)
{
	// Initialize variables.

	regionControl_ = 0;

	// Create UI elements.

	movingView_ = new QWidget();
	movingDescription_ = new QLabel();

	failedView_ = new QWidget();
	failedDescription_ = new QLabel();
	QDialogButtonBox *failedButtons = new QDialogButtonBox(QDialogButtonBox::Ok);

	succeededView_ = new QWidget();
	QLabel *succeededMessage = new QLabel("Region change completed successfully!");
	QDialogButtonBox *succeededButtons = new QDialogButtonBox(QDialogButtonBox::Ok);

	// Create and set layouts.

	QVBoxLayout *movingLayout = new QVBoxLayout();
	movingLayout->addWidget(movingDescription_);

	movingView_->setLayout(movingLayout);

	QVBoxLayout *failedLayout = new QVBoxLayout();
	failedLayout->addWidget(failedDescription_);
	failedLayout->addWidget(failedButtons);

	failedView_->setLayout(failedLayout);

	QVBoxLayout *succeededLayout = new QVBoxLayout();
	succeededLayout->addWidget(succeededMessage);
	succeededLayout->addWidget(succeededButtons);

	succeededView_->setLayout(succeededLayout);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(movingView_);
	layout->addWidget(failedView_);
	layout->addWidget(succeededView_);

	setLayout(layout);

	// Make connections.

	connect( failedButtons, SIGNAL(accepted()), this, SLOT(close()) );
	connect( succeededButtons, SIGNAL(accepted()), this, SLOT(close()) );

	// Current settings.

	movingView_->hide();
	failedView_->hide();
	succeededView_->hide();

	setRegionControl(regionControl);
}

BioXASMainMonochromatorRegionControlView::~BioXASMainMonochromatorRegionControlView()
{

}

void BioXASMainMonochromatorRegionControlView::setRegionControl(BioXASMainMonochromatorRegionControl *newControl)
{
	if (regionControl_ != newControl) {

		if (regionControl_)
			disconnect( regionControl_, 0, this, 0 );

		regionControl_ = newControl;

		if (regionControl_) {
			connect( regionControl_, SIGNAL(moveStarted()), this, SLOT(onMoveStarted()) );
			connect( regionControl_, SIGNAL(moveFailed(int)), this, SLOT(onMoveFailed(int)) );
			connect( regionControl_, SIGNAL(moveSucceeded()), this, SLOT(onMoveSucceeded()) );
		}

		emit regionControlChanged(regionControl_);
	}
}

void BioXASMainMonochromatorRegionControlView::onMoveStarted()
{
	showMovingView();
}

void BioXASMainMonochromatorRegionControlView::onMoveFailed(int failureCode)
{
	showFailedView(AMControl::failureExplanation(failureCode));
}

void BioXASMainMonochromatorRegionControlView::onMoveSucceeded()
{
	showSucceededView();
}

void BioXASMainMonochromatorRegionControlView::showMovingView()
{
	// Hide other views.

	failedView_->hide();
	succeededView_->hide();

	// Show moving view.

	movingView_->show();
}

void BioXASMainMonochromatorRegionControlView::showFailedView(const QString &message)
{
	// Hide other views.

	movingView_->hide();
	succeededView_->hide();

	// Set failure message.

	failedDescription_->setText(message);

	// Show failed view.

	failedView_->show();
}

void BioXASMainMonochromatorRegionControlView::showSucceededView()
{
	// Hide other views.

	movingView_->hide();
	failedView_->hide();

	// Show succeeded view.

	succeededView_->show();
}
