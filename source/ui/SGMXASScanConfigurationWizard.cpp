/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#include "SGMXASScanConfigurationWizard.h"

SGMXASScanConfigurationWizard::SGMXASScanConfigurationWizard(SGMXASScanConfiguration *sxsc, const AMDetectorInfoSet& cfgDetectorInfoSet, QWidget *parent) :
	QWizard(parent)
{
	cfg_ = NULL;
	if(SGMBeamline::sgm()->isConnected()){
		cfg_ = sxsc;

		introPage = new SGMXASScanConfigurationIntroWizardPage("Welcome to SGM's XAS Scan Wizard",
															   "This wizard will guide you through all of the steps required to configure an XAS on the SGM Beamline.");
		regionsPage = new AMXASRegionsWizardPage(cfg_->regions(),
												 "Configure Your Scan's Energy Regions",
												 "Using the table provided, enter as many regions as you require.");
		fluxResolutionPage = new SGMFluxResolutionWizardPage((AMControlOptimizationSet*)(cfg_->fluxResolutionSet()), cfg_->regions(),
															 "Pick the Desired Flux or Resolution for Your Scan",
															 "Based on the regions you've entered, the sliders below offer the optimum settings for flux and resolution. "
															 "If you choose a flux setting, the best possible resolution will be chosen and vice versa. "
															 "Given your selection, the beamline parameters will be set accordinly. "
															 "If you wish to set the applicable beamline parameters manually, click on the Details button.");
		trackingPage = new AMControlSetWizardPage(cfg_->trackingSet(),
												  "Enable or Disable Energy Tracking for Your Scan",
												  "By unchecking the boxes below, you will disable the energy tracking of each element. "
												  "Normally, all of these devices track energy; however, certain scan types and certain energy regions can operate without tracking for increased stability.");
		AMDetectorInfoSet detectorChoiceConfigurations = cfg_->detectorChoiceConfigurations();
		detectorsPage = new AMDetectorSetWizardPage(cfg_->detectorChoices(), &detectorChoiceConfigurations,
													"Choose which Detectors Your Scan will Collect Data From",
													"Checking the boxes below will enable each detector for this scan. "
													"Additional setup information can be found by clicking the appropriate Details button.");
		reviewPage = new SGMXASScanConfigurationReviewWizardPage("Review the Settings for Your Scan",
																 "Scan Configuration Complete.");
		QMap<int, QWizardPage*> widthMap, heightMap;
		widthMap.insert(introPage->sizeHint().width(), introPage);
		widthMap.insert(regionsPage->sizeHint().width(), regionsPage);
		widthMap.insert(fluxResolutionPage->sizeHint().width(), fluxResolutionPage);
		widthMap.insert(trackingPage->sizeHint().width(), trackingPage);
		widthMap.insert(detectorsPage->sizeHint().width(), detectorsPage);
		widthMap.insert(reviewPage->sizeHint().width(), reviewPage);
		heightMap.insert(introPage->sizeHint().height(), introPage);
		heightMap.insert(regionsPage->sizeHint().height(), regionsPage);
		heightMap.insert(fluxResolutionPage->sizeHint().height(), fluxResolutionPage);
		heightMap.insert(trackingPage->sizeHint().height(), trackingPage);
		heightMap.insert(detectorsPage->sizeHint().height(), detectorsPage);
		heightMap.insert(reviewPage->sizeHint().height(), reviewPage);
		QMap<int, QWizardPage*>::const_iterator i = widthMap.constEnd();
		QMap<int, QWizardPage*>::const_iterator j = heightMap.constEnd();
		--i;
		--j;
		int newMinWidth = i.key();
		i.value()->setMinimumWidth(newMinWidth);
		while(i != widthMap.constBegin()){
			--i;
			i.value()->setMinimumWidth(newMinWidth);
		}
		int newMinHeight = j.key();
		j.value()->setMinimumHeight(newMinHeight);
		while(j != heightMap.constBegin()){
			--j;
			j.value()->setMinimumHeight(newMinHeight);
		}

		addPage(introPage);
		addPage(regionsPage);
		addPage(fluxResolutionPage);
		addPage(trackingPage);
		addPage(detectorsPage);
		addPage(reviewPage);

		setWindowTitle(tr("SGM XAS Scan Wizard"));
		qDebug() << "End of constructor, cfg_ is " << cfg_;
	}
}

void SGMXASScanConfigurationWizard::accept(){
	qDebug() << "In accept, cfg_ is " << cfg_;
	SGMXASDacqScanController *xasCtrl = new SGMXASDacqScanController(cfg_, SGMBeamline::sgm());
	emit scanControllerReady((AMScanController*)xasCtrl);
	xasCtrl->initialize();
	QDialog::accept();
	emit scanStartRequested();
}

SGMXASScanConfigurationIntroWizardPage::SGMXASScanConfigurationIntroWizardPage(QString title, QString subTitle, QWidget *parent) :
		QWizardPage(parent)
{
	gl_ = new QGridLayout();
	gl_->setColumnMinimumWidth(0, gl_->sizeHint().width());
	setLayout(gl_);
	textLabel_ = new QLabel(subTitle);
	textLabel_->setAlignment(Qt::AlignJustify);
	textLabel_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	textLabel_->setWordWrap(true);
	gl_->addWidget(textLabel_, 0, 0, 1, 1, Qt::AlignLeft|Qt::AlignTop);
	setTitle(title);
}

void SGMXASScanConfigurationIntroWizardPage::resizeEvent(QResizeEvent *e){
	textLabel_->setMinimumWidth(e->size().width() - (int)(0.05*e->size().width()) );
	QWizardPage::resizeEvent(e);
}

AMXASRegionsWizardPage::AMXASRegionsWizardPage(AMXASRegionsList *regionsList, QString title, QString subTitle, QWidget *parent) :
		QWizardPage(parent)
{
	regionsList_ = regionsList;
	regionsView_ = new AMXASRegionsView(regionsList_, this);
	regionsLineView_ = new AMRegionsLineView(regionsList_, this);
	gl_ = new QGridLayout();
	textLabel_ = new QLabel(subTitle);
	textLabel_->setAlignment(Qt::AlignJustify);
	textLabel_->setWordWrap(true);
	textLabel_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	gl_->addWidget(textLabel_, 0, 0, 1, 1, Qt::AlignLeft|Qt::AlignTop);
	gl_->addWidget(regionsLineView_, 1, 0, 1, 1, Qt::AlignLeft);
	gl_->addWidget(regionsView_, 2, 0, 1, 1, Qt::AlignLeft);
	setLayout(gl_);
	setTitle(title);
	textLabel_->resize(textLabel_->sizeHint());
}

void AMXASRegionsWizardPage::resizeEvent(QResizeEvent *e){
	textLabel_->setMinimumWidth(e->size().width() - (int)(0.05*e->size().width()) );
	QWizardPage::resizeEvent(e);
}

SGMFluxResolutionWizardPage::SGMFluxResolutionWizardPage(AMControlOptimizationSet *fluxResolutionSet, AMXASRegionsList* regionsList, QString title, QString subTitle, QWidget *parent) :
		QWizardPage(parent)
{
	fluxResolutionSet_ = fluxResolutionSet;
	regionsList_ = regionsList;
	fluxResolutionView_ = new AMCompactControlOptimizationSetView(fluxResolutionSet_, this);
	connect(regionsList_, SIGNAL(regionsChanged()), this, SLOT(onRegionsUpdate()));
	onRegionsUpdate();
	gl_ = new QGridLayout();
	textLabel_ = new QLabel(subTitle);
	textLabel_->setAlignment(Qt::AlignJustify);
	textLabel_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	textLabel_->setWordWrap(true);
	gl_->addWidget(textLabel_, 0, 0, 1, 1, Qt::AlignLeft|Qt::AlignTop);
	gl_->addWidget(fluxResolutionView_, 1, 0, 1, 1, Qt::AlignLeft);
	setLayout(gl_);
	setTitle(title);
}

void SGMFluxResolutionWizardPage::resizeEvent(QResizeEvent *e){
	textLabel_->setMinimumWidth(e->size().width() - (int)(0.05*e->size().width()) );
	QWizardPage::resizeEvent(e);
}

void SGMFluxResolutionWizardPage::onRegionsUpdate(){
	fluxResolutionView_->onRegionsUpdate( regionsList_ );
}

AMControlSetWizardPage::AMControlSetWizardPage(AMControlSet *trackingSet, QString title, QString subTitle, QWidget *parent) :
		QWizardPage(parent)
{
	trackingSet_ = trackingSet;
	trackingView_ = new AMControlSetView(trackingSet_, true);
	gl_ = new QGridLayout();
	textLabel_ = new QLabel(subTitle);
	textLabel_->setAlignment(Qt::AlignJustify);
	textLabel_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	textLabel_->setWordWrap(true);
	gl_->addWidget(textLabel_, 0, 0, 1, 1, Qt::AlignLeft|Qt::AlignTop);
	gl_->addWidget(trackingView_, 1, 0, 1, 1, Qt::AlignLeft);
	setLayout(gl_);
	setTitle(title);
}

void AMControlSetWizardPage::resizeEvent(QResizeEvent *e){
	textLabel_->setMinimumWidth(e->size().width() - (int)(0.05*e->size().width()) );
	QWizardPage::resizeEvent(e);
}

AMDetectorSetWizardPage::AMDetectorSetWizardPage(AMDetectorSet *detectorSet, AMDetectorInfoSet *cfgDetectorInfoSet, QString title, QString subTitle, QWidget *parent) :
		QWizardPage(parent)
{
	detectorSet_ = detectorSet;
	detectorView_ = new AMDetectorSetView(detectorSet, true);
	gl_ = new QGridLayout();
	textLabel_ = new QLabel(subTitle);
	textLabel_->setAlignment(Qt::AlignJustify);
	textLabel_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	textLabel_->setWordWrap(true);
	gl_->addWidget(textLabel_, 0, 0, 1, 1, Qt::AlignLeft|Qt::AlignTop);
	gl_->addWidget(detectorView_, 1, 0, 1, 1, Qt::AlignLeft);
	setLayout(gl_);
	setTitle(title);
}

void AMDetectorSetWizardPage::resizeEvent(QResizeEvent *e){
	textLabel_->setMinimumWidth(e->size().width() - (int)(0.05*e->size().width()) );
	QWizardPage::resizeEvent(e);
}

SGMXASScanConfigurationReviewWizardPage::SGMXASScanConfigurationReviewWizardPage(QString title, QString subTitle, QWidget *parent) :
		QWizardPage(parent)
{
	gl_ = new QGridLayout();
	setLayout(gl_);
	textLabel_ = new QLabel(subTitle);
	textLabel_->setAlignment(Qt::AlignJustify);
	textLabel_->setWordWrap(true);
	textLabel_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	gl_->addWidget(textLabel_, 0, 0, 1, 1, Qt::AlignLeft|Qt::AlignTop);
	setTitle(title);
}

void SGMXASScanConfigurationReviewWizardPage::resizeEvent(QResizeEvent *e){
	textLabel_->setMinimumWidth(e->size().width() - (int)(0.05*e->size().width()) );
	QWizardPage::resizeEvent(e);
}
