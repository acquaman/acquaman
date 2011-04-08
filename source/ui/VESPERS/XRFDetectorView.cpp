#include "XRFDetectorView.h"
#include "DeadTimeButton.h"
#include "acquaman/VESPERS/VESPERSXRFScanController.h"
#include "ui/AMPeriodicTableView.h"

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

	AMPeriodicTableView *table = new AMPeriodicTableView;

	QHBoxLayout *viewLayout = new QHBoxLayout;
	viewLayout->addLayout(setupPlot());
	viewLayout->addLayout(setupControls());

	QVBoxLayout *tableLayout = new QVBoxLayout;
	tableLayout->addLayout(viewLayout);
	tableLayout->addWidget(table);

	setLayout(tableLayout);

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

void XRFDetailedDetectorView::onDeadTimeUpdate()
{
	deadTime_->setText(QString::number(detector_->deadTime()) + " %");
}

void XRFDetailedDetectorView::onElapsedTimeUpdate(double time)
{
	elapsedTime_->setText(QString::number(time, 'f', 2) + " s");
}

void XRFDetailedDetectorView::applyLog(bool apply)
{
	/// \todo implement the applyLog method
}

void XRFDetailedDetectorView::onStart()
{

}

void XRFDetailedDetectorView::onStop()
{

}


QLayout *XRFDetailedDetectorView::setupControls()
{
	QToolButton *start = new QToolButton;
	start->setIcon(QIcon(":/play_button_green.png"));
	connect(start, SIGNAL(clicked()), this, SLOT(onStart()));

	QToolButton *stop = new QToolButton;
	stop->setIcon(QIcon(":/red-stop-button.png"));
	connect(stop, SIGNAL(clicked()), this, SLOT(onStop()));

	integrationTime_ = new QDoubleSpinBox;
	integrationTime_->setSuffix(" s");
	integrationTime_->setSingleStep(0.1);
	connect(detector_->integrationTimeControl(), SIGNAL(valueChanged(double)), integrationTime_, SLOT(setValue(double)));

	elapsedTime_ = new QLabel(tr(" s"));
	connect(detector_->elapsedTimeControl(), SIGNAL(valueChanged(double)), this, SLOT(onElapsedTimeUpdate(double)));

	deadTime_ = new QLabel(tr(" %"));
	connect(detector_->deadTimeControl(), SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(onDeadTimeUpdate()));

	// Using a button group so I know which element I need to disable.
	DeadTimeButton *temp;
	deadTimeGroup_ = new QButtonGroup(this);
	deadTimeGroup_->setExclusive(false);
	QHBoxLayout *deadTimeLayout = new QHBoxLayout;
	deadTimeLayout->setSpacing(0);

	for (int i = 0; i < detector_->elements(); i++){

		temp = new DeadTimeButton(15.0, 30.0);
		temp->setCheckable(true);
		temp->setFixedSize(20, 20);
		connect(detector_->deadTimeControl()->at(i), SIGNAL(valueChanged(double)), temp, SLOT(setCurrent(double)));
		deadTimeLayout->addWidget(temp);
		deadTimeGroup_->addButton(temp, i);
	}

	if (detector_->elements() == 1)
		deadTimeGroup_->button(0)->setCheckable(false);
	else
		connect(deadTimeGroup_, SIGNAL(buttonClicked(int)), this, SLOT(elementClicked(int)));

	QGroupBox *deadTimeBox = new QGroupBox;
	deadTimeBox->setFlat(true);
	deadTimeBox->setLayout(deadTimeLayout);

	QToolButton *log = new QToolButton;
	log->setCheckable(true);
	log->setText("Log");
	connect(log, SIGNAL(toggled(bool)), this, SLOT(applyLog(bool)));

	QGridLayout *controlsLayout = new QGridLayout;
	controlsLayout->addWidget(start, 0, 0);
	controlsLayout->addWidget(stop, 0, 1);
	controlsLayout->addWidget(integrationTime_, 1, 0);
	controlsLayout->addWidget(elapsedTime_, 1, 1);
	controlsLayout->addWidget(deadTime_, 2, 0);
	controlsLayout->addWidget(deadTimeBox, 2, 1);
	controlsLayout->addWidget(log, 3, 0);

	QGroupBox *controls = new QGroupBox;
	controls->setLayout(controlsLayout);
	controls->setFlat(true);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(controls);
	layout->addStretch();

	return layout;
}

QLayout *XRFDetailedDetectorView::setupPlot()
{
	view_ = new AMScanView();

	// Put it in a layout for viewing.
	QHBoxLayout *plotLayout = new QHBoxLayout;
	plotLayout->addWidget(view_);

	return plotLayout;
}

// End detailed detector view
/////////////////////////////////////////////////////
