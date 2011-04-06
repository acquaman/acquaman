#include "XRFDetectorView.h"
#include "DeadTimeButton.h"

#include <QString>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QButtonGroup>

// Brief detector view
/////////////////////////////////////////////

XRFBriefDetectorView::XRFBriefDetectorView(XRFDetector *detector, bool configureOnly, QWidget *parent)
	: AMBriefDetectorView(configureOnly, parent)
{
	// Setting this to zero even if detector is valid.  This is because I'll do a cast to the right detector in setDetector.
	detector_ = 0;
	setDetector(detector, configureOnly);
}

bool XRFBriefDetectorView::setDetector(AMDetector *detector, bool configureOnly)
{
	//I don't have a configure only view for these.  It doesn't make quite as much sense for the stand alone spectra to have configure only views.
	Q_UNUSED(configureOnly)

	// If there is no valid detector pointer, then return false.
	if (!detector)
		return false;

	detector_ = static_cast<XRFDetector *>(detector);

	QLabel *name = new QLabel(detector_->name());
	deadTime_ = new QLabel(QString::number(detector_->deadTime()) + " %");
	QToolButton *details = new QToolButton;
	details->setText(tr("Detailed"));

	connect(detector_, SIGNAL(connected(bool)), this, SLOT(setEnabled(bool)));
	connect(detector_, SIGNAL(nameChanged(QString)), name, SLOT(setText(QString)));
	connect(detector_, SIGNAL(readingsChanged(AMControlInfoList)), this, SLOT(onDeadTimeUpdate()));
	connect(details, SIGNAL(clicked()), this, SIGNAL(detailedViewClicked()));

	QHBoxLayout *briefLayout = new QHBoxLayout;
	briefLayout->addWidget(name);
	briefLayout->addWidget(deadTime_);
	briefLayout->addWidget(details);

	setLayout(briefLayout);

	return true;
}

void XRFBriefDetectorView::onDeadTimeUpdate()
{
	deadTime_->setText(QString::number(detector_->deadTime()) + " %");
}

// End brief detector view
////////////////////////////////////////////////////

// Detailed detector view
///////////////////////////////////////////////////
XRFDetailedDetectorView::XRFDetailedDetectorView(XRFDetector *detector, bool configureOnly, QWidget *parent)
	: AMDetailedDetectorView(configureOnly, parent)
{
	// Setting this to zero even if detector is valid.  This is because I'll do a cast to the right detector in setDetector.
	detector_ = 0;
	setDetector(detector, configureOnly);
}

bool XRFDetailedDetectorView::setDetector(AMDetector *detector, bool configureOnly)
{
	//I don't have a configure only view for these.  It doesn't make quite as much sense for the stand alone spectra to have configure only views.
	Q_UNUSED(configureOnly)

	// If there is no valid detector pointer, then return false.
	if (!detector)
		return false;

	detector_ = static_cast<XRFDetector *>(detector);

	QToolButton *start = new QToolButton;
	start->setIcon(QIcon(":/play_button_green.png"));

	QToolButton *stop = new QToolButton;
	stop->setIcon(QIcon(":/red-stop-button.png"));

	integrationTime_ = new QDoubleSpinBox;
	integrationTime_->setSuffix(" s");
	integrationTime_->setSingleStep(0.1);

	elapsedTime_ = new QLabel(tr(" s"));

	deadTime_ = new QLabel(tr(" %"));

	// Using a button group so I know which element I need to disable.
	DeadTimeButton *temp;
	deadTimeGroup_ = new QButtonGroup(this);
	deadTimeGroup_->setExclusive(false);
	QHBoxLayout *deadTimeLayout = new QHBoxLayout;
	deadTimeLayout->setSpacing(0);

	for (int i = 0; i < detector_->elements(); i++){

		temp = new DeadTimeButton(15.0, 30.0);
		temp->setCheckable(true);
		temp->setCurrent(10);
		temp->setFixedSize(20, 20);
		deadTimeLayout->addWidget(temp);
		deadTimeGroup_->addButton(temp, i);
	}

	connect(deadTimeGroup_, SIGNAL(buttonClicked(int)), this, SLOT(elementClicked(int)));
	QGroupBox *deadTimeBox = new QGroupBox;
	deadTimeBox->setFlat(true);
	deadTimeBox->setLayout(deadTimeLayout);

	QToolButton *log = new QToolButton;
	log->setCheckable(true);
	log->setText("Log");

	QToolButton *channels = new QToolButton;
	channels->setCheckable(true);
	channels->setText("Channel #");

	QToolButton *waterfallButton = new QToolButton;
	waterfallButton->setCheckable(true);
	waterfallButton->setText("Raw Data");

	waterfall_ = new QDoubleSpinBox;
	waterfall_->setValue(0.1);
	waterfall_->setSingleStep(0.01);
	waterfall_->setAlignment(Qt::AlignCenter);
	waterfall_->setEnabled(false);

	connect(waterfallButton, SIGNAL(toggled(bool)), waterfall_, SLOT(setEnabled(bool)));

	QGridLayout *controlsLayout = new QGridLayout;
	controlsLayout->addWidget(start, 0, 0);
	controlsLayout->addWidget(stop, 0, 1);
	controlsLayout->addWidget(integrationTime_, 1, 0);
	controlsLayout->addWidget(elapsedTime_, 1, 1);
	controlsLayout->addWidget(deadTime_, 2, 0);
	controlsLayout->addWidget(deadTimeBox, 2, 1);
	controlsLayout->addWidget(log, 3, 0);
	controlsLayout->addWidget(channels, 3, 1);
	controlsLayout->addWidget(waterfallButton, 4, 0);
	controlsLayout->addWidget(waterfall_, 4, 1);

	QGroupBox *controls = new QGroupBox;
	controls->setLayout(controlsLayout);
	controls->setFlat(true);

	QVBoxLayout *tempStretch = new QVBoxLayout;
	tempStretch->addWidget(controls);
	tempStretch->addStretch();

	QHBoxLayout *viewLayout = new QHBoxLayout;
	viewLayout->addStretch();
	viewLayout->addLayout(tempStretch);

	setLayout(viewLayout);

	return true;
}

void XRFDetailedDetectorView::elementClicked(int elementId)
{
	// If the button is checked then it means the element should be disabled.
	if (deadTimeGroup_->button(elementId)->isChecked())
		detector_->disableElement(elementId);
	else
		detector_->enableElement(elementId);
}

// End detailed detector view
/////////////////////////////////////////////////////
