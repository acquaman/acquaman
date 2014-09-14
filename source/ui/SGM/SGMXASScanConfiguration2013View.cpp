/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ui/dataman/AMStepScanAxisView.h"
#include "SGMXASScanConfiguration2013View.h"

 SGMXASScanConfiguration2013View::~SGMXASScanConfiguration2013View(){}
SGMXASScanConfiguration2013View::SGMXASScanConfiguration2013View(SGMXASScanConfiguration2013 *configuration, QWidget *parent) :
	AMScanConfigurationView(parent)
{
	configuration_ = configuration;

	topFrame_ = new AMTopFrame("Configure an XAS Scan to Run Later");
	topFrame_->setIcon(QIcon(":/utilities-system-monitor.png"));

	// Regions setup
	AMStepScanAxisView *regionsView = new AMStepScanAxisView("", configuration_);

	QVBoxLayout *regionsViewLayout = new QVBoxLayout;
	regionsViewLayout->addWidget(regionsView);

	QGroupBox *regionsViewGroupBox = new QGroupBox("Regions Setup");
	regionsViewGroupBox->setLayout(regionsViewLayout);

	xasDetectorSelector_ = 0; //NULL
	xasDetectorSelectorView_ = 0; //NULL

	/* removed as temporary fix (see Issue579)
	trackingSet_ = 0; //NULL
	trackingSetView_ = 0; //NULL
	*/

	undulatorTrackingButton_ = new QPushButton(QString("Undulator"));
	undulatorTrackingButton_->setCheckable(true);
	updateTrackingButtonStatus(undulatorTrackingButton_, configuration_->undulatorTracking());

	gratingTrackingButton_ = new QPushButton(QString("Grating"));
	gratingTrackingButton_->setCheckable(true);
	updateTrackingButtonStatus(gratingTrackingButton_, configuration_->monoTracking());

	exitSlitTrackingButton_ = new QPushButton(QString("Exit Slit"));
	exitSlitTrackingButton_->setCheckable(true);
	updateTrackingButtonStatus(exitSlitTrackingButton_, configuration_->exitSlitTracking());

	// TODO: Need to find a way to get gratingSlitTracking()

	fluxResolutionView_ = new SGMFluxResolutionPickerView(configuration_->scanAxisAt(0), this);
	fluxResolutionView_->setFromInfoList(configuration_->fluxResolutionGroup());
	fluxResolutionView_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
	connect(fluxResolutionView_, SIGNAL(configValuesChanged(AMControlInfoList)), configuration_->dbObject(), SLOT(setFluxResolutionGroup(AMControlInfoList)));
	connect(configuration_->dbObject(), SIGNAL(fluxResolutionGroupChanged()), this, SLOT(onFluxResolutionGroupChanged()));
	connect(configuration_->dbObject(), SIGNAL(exitSlitTrackingChanged(bool)), this, SLOT(onExitSlitTrackingChanged(bool)));
	connect(configuration_->dbObject(), SIGNAL(undulatorTrackingChanged(bool)), this, SLOT(onUndulatorTrackingChanged(bool)));
	connect(configuration_->dbObject(), SIGNAL(monoTrackingChanged(bool)), this, SLOT(onGratingTrackingChanged(bool)));
	connect(undulatorTrackingButton_, SIGNAL(clicked()), this, SLOT(onUndulatorTrackingButtonClicked()));
	connect(gratingTrackingButton_, SIGNAL(clicked()), this, SLOT(onGratingTrackingButtonClicked()));
	connect(exitSlitTrackingButton_, SIGNAL(clicked()), this, SLOT(onExitSlitTrackingButtonClicked()));
	connect(configuration, SIGNAL(matchingBeamlineStatusChanged(bool)), this, SLOT(onMatchingBeamlineSettingsChanged(bool)));

	matchesBeamlineWarning_ = new QLabel("Warning: Current Config does not match the beamline settings. \nStarting a scan now will override the beamline settings with those of the config.");
	matchesBeamlineWarning_->setVisible(false);
	QPalette warningPalette = matchesBeamlineWarning_->palette();
	warningPalette.setColor(QPalette::WindowText, Qt::red);
	matchesBeamlineWarning_->setPalette(warningPalette);

	getBeamlineSettings_ = new QPushButton("Get Settings from Beamline");
	getBeamlineSettings_->setVisible(false);
	connect(getBeamlineSettings_, SIGNAL(clicked()), this, SLOT(onGetBeamlineSettingsClicked()));

	scanNameLabel_ = new QLabel("Scan Name");
	scanNameEdit_ = new AMRegExpLineEdit("/|;|@|#|<|>", Qt::CaseInsensitive, "/;#>@< characters are not allowed.");
	scanNameEdit_->setValidIfMatches(false);
	connect(scanNameEdit_, SIGNAL(textEdited(QString)), this, SLOT(onScanNameEditChanged(QString)));

	QGroupBox *trackingGroupBox = new QGroupBox("Tracking");
	QVBoxLayout* trackingButtonsLayout = new QVBoxLayout();
	trackingButtonsLayout->addWidget(undulatorTrackingButton_);
	trackingButtonsLayout->addWidget(gratingTrackingButton_);
	trackingButtonsLayout->addWidget(exitSlitTrackingButton_);
	trackingGroupBox->setLayout(trackingButtonsLayout);

	mainVL_ = new QVBoxLayout();
	mainVL_->addWidget(topFrame_);

	topRow_ = new QHBoxLayout();
	bottomRow_ = new QHBoxLayout();
	warningRow_ = new QHBoxLayout();
	explanationRow_ = new QHBoxLayout();

	topRow_->addWidget(regionsViewGroupBox);
	topRow_->addWidget(trackingGroupBox);

	bottomRow_->addWidget(fluxResolutionView_);

	warningRow_->addWidget(matchesBeamlineWarning_);

	explanationRow_->addSpacerItem(new QSpacerItem(450, 10));
	explanationRow_->addWidget(getBeamlineSettings_);

	mainVL_->addLayout(topRow_);
	mainVL_->addLayout(bottomRow_);
	mainVL_->addLayout(warningRow_);
	mainVL_->addLayout(explanationRow_);

	mainVL_->addStretch(8);

	QHBoxLayout *nameHL = new QHBoxLayout();
	nameHL->addWidget(scanNameLabel_);
	nameHL->addWidget(scanNameEdit_);
	nameHL->setContentsMargins(10,0,0,0);
	mainVL_->addLayout(nameHL);

	mainVL_->setContentsMargins(0,0,0,0);
	mainVL_->setSpacing(1);
	setLayout(mainVL_);

	// on startup, check the current status of beamline so that we can it
	onMatchingBeamlineSettingsChanged(configuration->matchesCurrentBeamline());
}

const AMScanConfiguration* SGMXASScanConfiguration2013View::configuration() const{
	configuration_->setDetectorConfigurations(xasDetectorSelector_->selectedDetectorInfos());
	return configuration_;
}

void SGMXASScanConfiguration2013View::setDetectorSelector(AMDetectorSelector *xasDetectorSelector){
	xasDetectorSelector_ = xasDetectorSelector;
	if(!xasDetectorSelectorView_){
		xasDetectorSelectorView_ = new AMDetectorSelectorView(xasDetectorSelector_);
		bottomRow_->addWidget(xasDetectorSelectorView_);
	}
}

/* removed as temporary fix (see Issue579)
void SGMXASScanConfiguration2013View::setTrackingSet(AMControlSet *trackingSet){
	trackingSet_ = trackingSet;
	if(!trackingSetView_){
		trackingSetView_ = new AMControlSetView(trackingSet_, true);
		bottomGL_->addWidget(trackingSetView_, 0, 2);

		connect(trackingSetView_, SIGNAL(configValuesChanged(AMControlInfoList)), configuration_->dbObject(), SLOT(setTrackingGroup(AMControlInfoList)));
		connect(configuration_->dbObject(), SIGNAL(trackingGroupChanged()), this, SLOT(onTrackingGroupChanged()));
	}
}

void SGMXASScanConfiguration2013View::onTrackingGroupChanged(){
	trackingSetView_->setFromInfoList(configuration_->trackingGroup());
}
*/

void SGMXASScanConfiguration2013View::onFluxResolutionGroupChanged(){
	fluxResolutionView_->setFromInfoList(configuration_->fluxResolutionGroup());
}

void SGMXASScanConfiguration2013View::onScanNameEditChanged(const QString &scanName){
	configuration_->setUserScanName(scanName);

	/* removed as temporary fix (see Issue579)
	for(int x = 0, size = trackingSet_->count(); x < size; x++)
		qDebug() << "Tracking set at " << x << trackingSet_->at(x)->name() << trackingSet_->controlNamed(trackingSet_->at(x)->name())->value();

	for(int x = 0, size = configuration_->trackingGroup().count(); x < size; x++)
		qDebug() << "Tracking group at " << x << configuration_->trackingGroup().at(x).name() << configuration_->trackingGroup().controlNamed(configuration_->trackingGroup().at(x).name()).value();
	*/
}

void SGMXASScanConfiguration2013View::onUndulatorTrackingChanged(bool isTracking)
{
	updateTrackingButtonStatus(undulatorTrackingButton_, isTracking);
}

void SGMXASScanConfiguration2013View::onUndulatorTrackingButtonClicked()
{
	configuration_->dbObject()->setUndulatorTracking(!configuration_->dbObject()->undulatorTracking());
}

void SGMXASScanConfiguration2013View::onExitSlitTrackingChanged(bool isTracking)
{
	updateTrackingButtonStatus(exitSlitTrackingButton_, isTracking);
}

void SGMXASScanConfiguration2013View::onExitSlitTrackingButtonClicked()
{
	configuration_->dbObject()->setExitSlitTracking(!configuration_->dbObject()->exitSlitTracking());
}

void SGMXASScanConfiguration2013View::onGratingTrackingChanged(bool isTracking)
{
	updateTrackingButtonStatus(gratingTrackingButton_, isTracking);
}

void SGMXASScanConfiguration2013View::onGratingTrackingButtonClicked()
{
	configuration_->dbObject()->setMonoTracking(!configuration_->dbObject()->monoTracking());
}

void SGMXASScanConfiguration2013View::onMatchingBeamlineSettingsChanged(bool matchedBeamline)
{
	matchesBeamlineWarning_->setVisible(!matchedBeamline);
	getBeamlineSettings_->setVisible(!matchedBeamline);
}

void SGMXASScanConfiguration2013View::onGetBeamlineSettingsClicked()
{
	configuration_->getSettingsFromBeamline();
}

void SGMXASScanConfiguration2013View::updateTrackingButtonStatus(QPushButton *button, bool isTracking)
{
	QColor buttonTextColor;


	buttonTextColor = (isTracking) ? (Qt::black) : (Qt::red);
	button->setChecked(isTracking);

	QPalette palette = button->palette();
	palette.setColor(QPalette::ButtonText, buttonTextColor);
	button->setPalette(palette);
}


