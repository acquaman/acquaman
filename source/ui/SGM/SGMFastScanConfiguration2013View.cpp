#include "SGMFastScanConfiguration2013View.h"

#include <QGridLayout>
#include <QComboBox>
#include <QLabel>
#include <QFileDialog>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QFormLayout>

#include "ui/AMTopFrame.h"

SGMFastScanConfiguration2013View::SGMFastScanConfiguration2013View(SGMFastScanConfiguration2013 *sfsc, QWidget *parent) :
		AMScanConfigurationView(parent)
{
	cfg_ = NULL;

	if(SGMBeamline::sgm()->isConnected()){
		cfg_ = sfsc;

		topFrame_ = new AMTopFrame("Configure a Fast Scan to Run Later");
		topFrame_->setIcon(QIcon(":/utilities-system-monitor.png"));

		presetsComboBox_ = new QComboBox();
		presetsComboBox_->addItems(sfsc->presets());
		connect(presetsComboBox_, SIGNAL(currentIndexChanged(int)), sfsc, SLOT(setParametersFromPreset(int)));

		connect(sfsc, SIGNAL(startPositionChanged()), this, SLOT(onParametersStartPositionChanged()));
		connect(sfsc, SIGNAL(middlePositionChanged()), this, SLOT(onParametersMiddlePositionChanged()));
		connect(sfsc, SIGNAL(endPositionChanged()), this, SLOT(onParametersEndPositionChanged()));
		connect(sfsc, SIGNAL(fastScanSettingsChanged()), this, SLOT(onFastScanSettingsChanged()));

		warningsLabel_ = new QLabel("");
		QFont warningsFont;
		warningsFont.setPointSize(48);
		warningsLabel_->setFont(warningsFont);
		warningsLabel_->setStyleSheet( "QLabel{ color: red }" );

		scanNameLabel_ = new QLabel("Scan Name");
		scanNameEdit_ = new QLineEdit(this);

		connect(scanNameEdit_, SIGNAL(textEdited(QString)), this, SLOT(onScanNameEditChanged(QString)));

		startPositionView_ = 0;
		middlePositionView_ = 0;
		endPositionView_ = 0;
		fastScanSettingsView_ = 0;

		fastDetectorSelector_ = 0; //NULL
		fastDetectorSelectorView_ = 0; //NULL

		onParametersStartPositionChanged();
		onParametersMiddlePositionChanged();
		onParametersEndPositionChanged();
		onFastScanSettingsChanged();

		startPositionView_ = new SGMEnergyPositionWBeamlineView(&startPositionCopy_, SGMEnergyPositionView::ViewModeStartOrEnd);
		middlePositionView_ = new SGMEnergyPositionWBeamlineView(&middlePositionCopy_, SGMEnergyPositionView::ViewModeMiddle);
		endPositionView_ = new SGMEnergyPositionWBeamlineView(&endPositionCopy_, SGMEnergyPositionView::ViewModeStartOrEnd);
		fastScanSettingsView_ = new SGMFastScanSettingsView(&fastScanSettingsCopy_);

		QHBoxLayout *presetsLayout = new QHBoxLayout();
		presetsLayout->addWidget(presetsComboBox_);
		presetsLayout->addStretch(10);

		settingsLayout_ = new QVBoxLayout();
		settingsLayout_->addWidget(fastScanSettingsView_);
		settingsLayout_->addStretch(8);
		settingsLayout_->setContentsMargins(5,0,5,0);

		QVBoxLayout *positionsLayout = new QVBoxLayout();
		positionsLayout->addWidget(startPositionView_);
		positionsLayout->addWidget(middlePositionView_);
		positionsLayout->addWidget(endPositionView_);
		positionsLayout->addStretch(8);
		positionsLayout->setContentsMargins(5,0,5,0);

		QHBoxLayout *hl = new QHBoxLayout();
		hl->addLayout(settingsLayout_);
		hl->addLayout(positionsLayout);

		QHBoxLayout *nameHL = new QHBoxLayout();
		nameHL->addWidget(scanNameLabel_);
		nameHL->addWidget(scanNameEdit_);

		QVBoxLayout *mainLayout = new QVBoxLayout();
		mainLayout->addLayout(presetsLayout);
		mainLayout->addLayout(hl);
		mainLayout->addWidget(warningsLabel_);
		mainLayout->addStretch(10);
		mainLayout->addLayout(nameHL);

		QVBoxLayout *vl = new QVBoxLayout();
		vl->addWidget(topFrame_);
		vl->addLayout(mainLayout);
		vl->setContentsMargins(0,0,0,0);
		vl->setSpacing(1);
		mainLayout->setContentsMargins(10, 0, 10, 0);
		this->setLayout(vl);
		this->setMaximumSize(700, 800);

		connect(SGMBeamline::sgm(), SIGNAL(criticalControlsConnectionsChanged()), this, SLOT(onSGMBeamlineCriticalControlsConnectedChanged()));
		onSGMBeamlineCriticalControlsConnectedChanged();
	}
}

const AMScanConfiguration* SGMFastScanConfiguration2013View::configuration() const{
	cfg_->setDetectorConfigurations(fastDetectorSelector_->selectedDetectorInfos());
	qDebug() << "Things been modified? " << cfg_->currentParameters()->modified() << cfg_->currentParameters()->fastScanSettings().modified();
	return cfg_;
}

void SGMFastScanConfiguration2013View::setDetectorSelector(AMDetectorSelector *fastDetectorSelector){
	fastDetectorSelector_ = fastDetectorSelector;
	if(!fastDetectorSelectorView_){
		fastDetectorSelectorView_ = new AMDetectorSelectorView(fastDetectorSelector_);
		settingsLayout_->addWidget(fastDetectorSelectorView_);
	}
}

void SGMFastScanConfiguration2013View::onSGMBeamlineCriticalControlsConnectedChanged(){
	if(SGMBeamline::sgm()->isConnected()){
		presetsComboBox_->setEnabled(true);
		startPositionView_->setEnabled(true);
		middlePositionView_->setEnabled(true);
		endPositionView_->setEnabled(true);
		fastScanSettingsView_->setEnabled(true);
		warningsLabel_->setText("");
	}
	else{
		presetsComboBox_->setEnabled(false);
		startPositionView_->setEnabled(false);
		middlePositionView_->setEnabled(false);
		endPositionView_->setEnabled(false);
		fastScanSettingsView_->setEnabled(false);
		warningsLabel_->setText("SGM Beamline Unavailable");
	}
}

void SGMFastScanConfiguration2013View::onScanNameEditChanged(const QString &scanName){
	cfg_->setUserScanName(scanName);
}

void SGMFastScanConfiguration2013View::onParametersStartPositionChanged(){
	disconnect(&startPositionCopy_, 0);
	startPositionCopy_ = cfg_->currentParameters()->scanInfo().start();
	if(startPositionView_)
		startPositionView_->setEnergyPosition(&startPositionCopy_);
	connect(&startPositionCopy_, SIGNAL(energyPositionChanged()), this, SLOT(onStartPositionCopyChanged()));
}

void SGMFastScanConfiguration2013View::onParametersMiddlePositionChanged(){
	disconnect(&middlePositionCopy_, 0);
	middlePositionCopy_ = cfg_->currentParameters()->scanInfo().middle();
	if(middlePositionView_)
		middlePositionView_->setEnergyPosition(&middlePositionCopy_);
	connect(&middlePositionCopy_, SIGNAL(energyPositionChanged()), this, SLOT(onMiddlePositionCopyChanged()));
}

void SGMFastScanConfiguration2013View::onParametersEndPositionChanged(){
	disconnect(&endPositionCopy_, 0);
	endPositionCopy_ = cfg_->currentParameters()->scanInfo().end();
	if(endPositionView_)
		endPositionView_->setEnergyPosition(&endPositionCopy_);
	connect(&endPositionCopy_, SIGNAL(energyPositionChanged()), this, SLOT(onEndPositionCopyChanged()));
}

void SGMFastScanConfiguration2013View::onFastScanSettingsChanged(){
	disconnect(&fastScanSettingsCopy_, 0);
	disconnect(&fastScanSettingsCopy_, SIGNAL(fastScanSettingsChanged()), this, SLOT(onFastScanSettingsCopyChanged()));

	fastScanSettingsCopy_ = cfg_->currentParameters()->fastScanSettings();
	if(fastScanSettingsView_)
		fastScanSettingsView_->setFastScanSettings(&fastScanSettingsCopy_);

	connect(&fastScanSettingsCopy_, SIGNAL(fastScanSettingsChanged()), this, SLOT(onFastScanSettingsCopyChanged()));
}

void SGMFastScanConfiguration2013View::onStartPositionCopyChanged(){
	cfg_->currentParameters()->setStartPosition(startPositionCopy_);
}

void SGMFastScanConfiguration2013View::onMiddlePositionCopyChanged(){
	cfg_->currentParameters()->setMiddlePosition(middlePositionCopy_);
}

void SGMFastScanConfiguration2013View::onEndPositionCopyChanged(){
	cfg_->currentParameters()->setEndPosition(endPositionCopy_);
}

void SGMFastScanConfiguration2013View::onFastScanSettingsCopyChanged(){
	cfg_->currentParameters()->setFastScanSettings(fastScanSettingsCopy_);
}
