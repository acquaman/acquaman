#include "BioXASJJSlitsScanConfigurationView.h"

#include <QLineEdit>
#include <QLayout>
#include <QCheckBox>

#include "ui/AMTopFrame.h"

BioXASJJSlitsScanConfigurationView::BioXASJJSlitsScanConfigurationView(BioXASJJSlitsScanConfiguration *configuration, QWidget *parent) :
    AMScanConfigurationView(parent)
{
	configuration_ = configuration;

	AMTopFrame *frame = new AMTopFrame("BioXAS JJ Slits Calibration");

	scanName_ = new QLineEdit;
	scanName_->setText(configuration_->name());
	scanName_->setAlignment(Qt::AlignCenter);

	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	onScanNameEdited();

	connect(configuration_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));

	// Dwell time.

	dwellTime_ = new QDoubleSpinBox;
	dwellTime_->setRange(0, 1000000);
	dwellTime_->setSuffix(" s");
	dwellTime_->setAlignment(Qt::AlignCenter);
	dwellTime_->setDecimals(1);
	dwellTime_->setValue(configuration_->scanAxes().size() > 0 ? double(configuration_->scanAxisAt(0)->regionAt(0)->regionTime()) : 1.0);
	dwellTime_->setMinimumWidth(100);

	connect(dwellTime_, SIGNAL(editingFinished()), this, SLOT(onDwellTimeChanged()));
	onDwellTimeChanged();

	QHBoxLayout *timeLayout = new QHBoxLayout;
	timeLayout->addWidget(new QLabel("Dwell Time:"));
	timeLayout->addWidget(dwellTime_);

	// The estimated scan time.

	estimatedTime_ = new QLabel;

	connect(configuration_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged()));
	onEstimatedTimeChanged();

	scanInformation_ = new QLabel("Scan Size:");

	QVBoxLayout *timeBoxLayout = new QVBoxLayout;
	timeBoxLayout->addLayout(timeLayout);
	timeBoxLayout->addWidget(estimatedTime_);
	timeBoxLayout->addWidget(scanInformation_);

	QGroupBox *timeGroupBox = new QGroupBox("Time");
	timeGroupBox->setLayout(timeBoxLayout);

	// Setup the group box for setting the start and end points.

	QGroupBox *positionsBox = new QGroupBox("Positions");

	xStart_ = createPositionDoubleSpinBox("Start: ", " mm", double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()), 3);
	xStep_ = createPositionDoubleSpinBox("Step: ", " mm", double(configuration_->scanAxisAt(0)->regionAt(0)->regionStep()), 3);
	xEnd_ = createPositionDoubleSpinBox("End: ", " mm", double(configuration_->scanAxisAt(0)->regionAt(0)->regionEnd()), 3);
	connect(xStart_, SIGNAL(editingFinished()), this, SLOT(onXStartChanged()));
	connect(xStep_, SIGNAL(editingFinished()), this, SLOT(onXStepChanged()));
	connect(xEnd_, SIGNAL(editingFinished()), this, SLOT(onXEndChanged()));

	zStart_ = createPositionDoubleSpinBox("Start: ", " mm", double(configuration_->scanAxisAt(1)->regionAt(0)->regionStart()), 3);
	zStep_ = createPositionDoubleSpinBox("Step: ", " mm", double(configuration_->scanAxisAt(1)->regionAt(0)->regionStep()), 3);
	zEnd_ = createPositionDoubleSpinBox("End: ", " mm", double(configuration_->scanAxisAt(1)->regionAt(0)->regionEnd()), 3);
	connect(zStart_, SIGNAL(editingFinished()), this, SLOT(onZStartChanged()));
	connect(zStep_, SIGNAL(editingFinished()), this, SLOT(onZStepChanged()));
	connect(zEnd_, SIGNAL(editingFinished()), this, SLOT(onZEndChanged()));

	connect(configuration_->scanAxisAt(0)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(setXStart(AMNumber)));
	connect(configuration_->scanAxisAt(0)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(setXStep(AMNumber)));
	connect(configuration_->scanAxisAt(0)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(setXEnd(AMNumber)));
	connect(configuration_->scanAxisAt(1)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(setZStart(AMNumber)));
	connect(configuration_->scanAxisAt(1)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(setZStep(AMNumber)));
	connect(configuration_->scanAxisAt(1)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(setZEnd(AMNumber)));

	updateScanInformation();
	onEstimatedTimeChanged();

	QHBoxLayout *axis1Layout = new QHBoxLayout;
	axis1Layout->addWidget(xStart_);
	axis1Layout->addWidget(xStep_);
	axis1Layout->addWidget(xEnd_);

	QHBoxLayout *axis2Layout = new QHBoxLayout;
	axis2Layout->addWidget(zStart_);
	axis2Layout->addWidget(zStep_);
	axis2Layout->addWidget(zEnd_);

	QVBoxLayout *positionLayout = new QVBoxLayout;
	positionLayout->addLayout(axis1Layout);
	positionLayout->addLayout(axis2Layout);
	positionsBox->setLayout(positionLayout);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(positionsBox);
	layout->addWidget(timeGroupBox);
	layout->addWidget(scanName_);

	QHBoxLayout *moreLayout = new QHBoxLayout;
	moreLayout->addStretch();
	moreLayout->addLayout(layout);
	moreLayout->addStretch();

	QVBoxLayout *configViewLayout = new QVBoxLayout;
	configViewLayout->addWidget(frame);
	configViewLayout->addStretch();
	configViewLayout->addLayout(moreLayout);
	configViewLayout->addStretch();

	setLayout(configViewLayout);
}

BioXASJJSlitsScanConfigurationView::~BioXASJJSlitsScanConfigurationView()
{

}

const AMScanConfiguration *BioXASJJSlitsScanConfigurationView::configuration() const
{
	return configuration_;
}

void BioXASJJSlitsScanConfigurationView::onScanNameEdited()
{
	QString name = scanName_->text();
	configuration_->setName(name);
	configuration_->setUserScanName(name);
}

void BioXASJJSlitsScanConfigurationView::onEstimatedTimeChanged()
{
	estimatedTime_->setText("Estimated time per scan:\t" + convertTimeToString(configuration_->totalTime()));
}

QString BioXASJJSlitsScanConfigurationView::convertTimeToString(double time)
{
	QString timeString;
	int days = int(time/3600.0/24.0);
	if (days > 0){
		time -= days*3600.0*24;
		timeString += QString::number(days) + "d:";
	}

	int hours = int(time/3600.0);
	if (hours > 0){
		time -= hours*3600;
		timeString += QString::number(hours) + "h:";
	}

	int minutes = int(time/60.0);

	if (minutes > 0){
		time -= minutes*60;
		timeString += QString::number(minutes) + "m:";
	}

	int seconds = ((int)time)%60;

	timeString += QString::number(seconds) + "s";

	return timeString;
}

QDoubleSpinBox * BioXASJJSlitsScanConfigurationView::createPositionDoubleSpinBox(const QString &prefix, const QString &suffix, double value, int decimals)
{
	QDoubleSpinBox *box = new QDoubleSpinBox;
	box->setPrefix(prefix);
	box->setRange(-100000, 100000);
	box->setSuffix(suffix);
	box->setDecimals(decimals);
	box->setValue(value);
	box->setAlignment(Qt::AlignCenter);
	return box;
}

void BioXASJJSlitsScanConfigurationView::onXStartChanged()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionStart(xStart_->value());
	updateScanInformation();
}

void BioXASJJSlitsScanConfigurationView::onXStepChanged()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionStep(xStep_->value());
	updateScanInformation();
}

void BioXASJJSlitsScanConfigurationView::onXEndChanged()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(xEnd_->value());
	updateScanInformation();
}

void BioXASJJSlitsScanConfigurationView::onZStartChanged()
{
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionStart(zStart_->value());
	updateScanInformation();
}

void BioXASJJSlitsScanConfigurationView::onZStepChanged()
{
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionStep(zStep_->value());
	updateScanInformation();
}

void BioXASJJSlitsScanConfigurationView::onZEndChanged()
{
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionEnd(zEnd_->value());
	updateScanInformation();
}

void BioXASJJSlitsScanConfigurationView::onDwellTimeChanged()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionTime(dwellTime_->value());
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionTime(dwellTime_->value());
}

void BioXASJJSlitsScanConfigurationView::updateScanInformation()
{
	double hSize = fabs(double(configuration_->scanAxisAt(0)->regionAt(0)->regionEnd())-double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()));
	double vSize = fabs(double(configuration_->scanAxisAt(1)->regionAt(0)->regionEnd())-double(configuration_->scanAxisAt(1)->regionAt(0)->regionStart()));

	int hPoints = int((hSize)/double(configuration_->scanAxisAt(0)->regionAt(0)->regionStep()));

	if ((hSize - (hPoints + 0.01)*double(configuration_->scanAxisAt(0)->regionAt(0)->regionStep())) < 0)
		hPoints += 1;
	else
		hPoints += 2;

	int vPoints = int((vSize)/double(configuration_->scanAxisAt(1)->regionAt(0)->regionStep()));

	if ((vSize - (vPoints + 0.01)*double(configuration_->scanAxisAt(1)->regionAt(0)->regionStep())) < 0)
		vPoints += 1;
	else
		vPoints += 2;

	scanInformation_->setText(QString("Scan Size: %1 mm x %2 mm\t Points: %3 x %4")
	.arg(QString::number(hSize, 'f', 1))
	.arg(QString::number(vSize, 'f', 1))
	.arg(hPoints)
	.arg(vPoints)
	);
}

void BioXASJJSlitsScanConfigurationView::setXStart(const AMNumber &value)
{
	xStart_->setValue(double(value));
}

void BioXASJJSlitsScanConfigurationView::setXStep(const AMNumber &value)
{
	xStep_->setValue(double(value));
}

void BioXASJJSlitsScanConfigurationView::setXEnd(const AMNumber &value)
{
	xEnd_->setValue(double(value));
}

void BioXASJJSlitsScanConfigurationView::setZStart(const AMNumber &value)
{
	zStart_->setValue(double(value));
}

void BioXASJJSlitsScanConfigurationView::setZStep(const AMNumber &value)
{
	zStep_->setValue(double(value));
}

void BioXASJJSlitsScanConfigurationView::setZEnd(const AMNumber &value)
{
	zEnd_->setValue(double(value));
}

void BioXASJJSlitsScanConfigurationView::setDwellTime(const AMNumber &value)
{
	dwellTime_->setValue(double(value));
}
