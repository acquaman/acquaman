#include "SGMFastScanConfigurationView.h"

#include "SGMSidebar.h"

SGMFastScanConfigurationView::SGMFastScanConfigurationView(SGMFastScanConfiguration *sfsc, QWidget *parent) :
		AMScanConfigurationView(parent)
{
	cfg_ = NULL;
	/* NTBA March 14, 2011 David Chevrier
	   Part of exporter package
	autoSavePath_ = "";
	autoSaveDialog_ = 0; //NULL
	*/
	if(SGMBeamline::sgm()->isConnected()){
		cfg_ = sfsc;

		presetsComboBox_ = new QComboBox();
		presetsComboBox_->addItems(sfsc->presets());
		connect(presetsComboBox_, SIGNAL(currentIndexChanged(int)), sfsc, SLOT(setParametersFromPreset(int)));

		elementLabel_ = new QLabel("Element");
		runTimeLabel_ = new QLabel("Run Time");
		startEnergyLabel_ = new QLabel("Start Energy");
		energyMidpointLabel_ = new QLabel("Energy Midpoint");
		endEnergyLabel_ = new QLabel("End Energy");
		motorSettingsLabel_ = new QLabel("Motor Settings");
		baseLineLabel_ = new QLabel("Baseline Counts");
		warningsLabel_ = new QLabel("");
		QFont warningsFont;
		warningsFont.setPointSize(48);
		warningsLabel_->setFont(warningsFont);
		warningsLabel_->setStyleSheet( "QLabel{ color: red }" );

		elementEdit_ = new QLineEdit();
		elementEdit_->setText(sfsc->element());
		runTimeDSB_ = new QDoubleSpinBox();
		runTimeDSB_->setMinimum(0);
		runTimeDSB_->setMaximum(100);
		runTimeDSB_->setValue(sfsc->runTime());
		startEnergyDSB_ = new QDoubleSpinBox();
		startEnergyDSB_->setMinimum(200);
		startEnergyDSB_->setMaximum(2000);
		startEnergyDSB_->setValue(sfsc->energyStart());
		energyMidpointDSB_ = new QDoubleSpinBox();
		energyMidpointDSB_->setMinimum(200);
		energyMidpointDSB_->setMaximum(2000);
		energyMidpointDSB_->setValue(sfsc->energyMidpoint());
		endEnergyDSB_ = new QDoubleSpinBox();
		endEnergyDSB_->setMinimum(200);
		endEnergyDSB_->setMaximum(2000);
		endEnergyDSB_->setValue(sfsc->energyEnd());
		motorSettingsSB_ = new QSpinBox();
		motorSettingsSB_->setMinimum(100);
		motorSettingsSB_->setMaximum(50000);
		motorSettingsSB_->setValue(sfsc->velocity());
		baseLineSB_ = new QSpinBox();
		baseLineSB_->setMinimum(0);
		baseLineSB_->setMaximum(50000);
		baseLineSB_->setValue(sfsc->baseLine());

		connect(sfsc, SIGNAL(onElementChanged(QString)), elementEdit_, SLOT(setText(QString)));
		connect(sfsc, SIGNAL(onRunSecondsChanged(double)), runTimeDSB_, SLOT(setValue(double)));
		connect(sfsc, SIGNAL(onEnergyStartChanged(double)), startEnergyDSB_, SLOT(setValue(double)));
		connect(sfsc, SIGNAL(onEnergyMidpointChanged(double)), energyMidpointDSB_, SLOT(setValue(double)));
		connect(sfsc, SIGNAL(onEnergyEndChanged(double)), endEnergyDSB_, SLOT(setValue(double)));
		connect(sfsc, SIGNAL(onVelocityChanged(int)), motorSettingsSB_, SLOT(setValue(int)));
		connect(sfsc, SIGNAL(onBaseLineChanged(int)), baseLineSB_, SLOT(setValue(int)));

		connect(elementEdit_, SIGNAL(textEdited(QString)), sfsc, SLOT(setElement(QString)));
		connect(runTimeDSB_, SIGNAL(valueChanged(double)), sfsc, SLOT(setRunSeconds(double)));
		connect(runTimeDSB_, SIGNAL(valueChanged(double)), sfsc, SLOT(setScalerTime(double)));
		connect(startEnergyDSB_, SIGNAL(valueChanged(double)), sfsc, SLOT(setEnergyStart(double)));
		connect(energyMidpointDSB_, SIGNAL(valueChanged(double)), sfsc, SLOT(setEnergyMidpoint(double)));
		connect(endEnergyDSB_, SIGNAL(valueChanged(double)), sfsc, SLOT(setEnergyEnd(double)));
		connect(motorSettingsSB_, SIGNAL(valueChanged(int)), sfsc, SLOT(setVelocity(int)));
		connect(motorSettingsSB_, SIGNAL(valueChanged(int)), sfsc, SLOT(setVelocityBase(int)));
		connect(motorSettingsSB_, SIGNAL(valueChanged(int)), sfsc, SLOT(setAcceleration(int)));
		connect(baseLineSB_, SIGNAL(valueChanged(int)), sfsc, SLOT(setBaseLine(int)));

		fl_ = new QFormLayout();
		fl_->addRow(elementLabel_, elementEdit_);
		fl_->addRow(runTimeLabel_, runTimeDSB_);
		fl_->addRow(startEnergyLabel_, startEnergyDSB_);
		fl_->addRow(energyMidpointLabel_, energyMidpointDSB_);
		fl_->addRow(endEnergyLabel_, endEnergyDSB_);
		fl_->addRow(motorSettingsLabel_, motorSettingsSB_);
		fl_->addRow(baseLineLabel_, baseLineSB_);

		/* NTBA March 14, 2011 David Chevrier
		   Part of exporter package
		saveLabel_ = new QLabel("Save a copy to:");
		saveEdit_ = new QLineEdit();
		saveEdit_->setMinimumWidth(400);
		saveEdit_->setText(sfsc->sensibleFileSavePath());
		autoSaveDialog_ = new QFileDialog(0, "Choose a Directory for Auto Save");
		autoSaveDialog_->setFileMode(QFileDialog::Directory);
		autoSaveDialogButton_ = new QPushButton();
		autoSaveDialogButton_->setText("Choose");
		connect(saveEdit_, SIGNAL(editingFinished()), this, SLOT(onSavePathEditingFinished()));
		connect(autoSaveDialogButton_, SIGNAL(clicked()), autoSaveDialog_, SLOT(show()));
		connect(autoSaveDialog_, SIGNAL(fileSelected(QString)), this, SLOT(onSaveDialogDirectoryChosen(QString)));
		if(!sfsc->finalizedSavePath().isEmpty())
			saveFbkLabel_ = new QLabel("\tFile will be saved as "+sfsc->finalizedSavePath().section('/', -1));
		else
			saveFbkLabel_ = new QLabel("\tEnter a path and a file root");
		fl2_ = new QFormLayout();
		fl2_->addRow(saveLabel_, saveEdit_);
		QHBoxLayout *hl = new QHBoxLayout();
		hl->addWidget(saveFbkLabel_);
		hl->addStretch(1);
		hl->addWidget(autoSaveDialogButton_);
		fl2_->addRow(hl);

		connect(sfsc, SIGNAL(onNewFinalizedSavePath(QString)), this, SLOT(onNewFinalizedSavePath(QString)));
		*/

		gl_.addWidget(presetsComboBox_,		0, 0, 1, 1, Qt::AlignCenter);
		gl_.addLayout(fl_,			0, 1, 1, 1, Qt::AlignCenter);
		gl_.addWidget(warningsLabel_,		0, 0, 1, 2, Qt::AlignCenter);
		gl_.setRowStretch(1, 10);
		this->setLayout(&gl_);
		this->setMaximumSize(700, 800);

		connect(SGMBeamline::sgm(), SIGNAL(criticalControlsConnectionsChanged()), this, SLOT(onSGMBeamlineCriticalControlsConnectedChanged()));
		onSGMBeamlineCriticalControlsConnectedChanged();
	}
}

const AMScanConfiguration* SGMFastScanConfigurationView::configuration() const{
	return cfg_;
}

void SGMFastScanConfigurationView::onSGMBeamlineCriticalControlsConnectedChanged(){
	if(SGMBeamline::sgm()->isConnected()){
		presetsComboBox_->setEnabled(true);
		elementLabel_->setEnabled(true);
		runTimeLabel_->setEnabled(true);
		startEnergyLabel_->setEnabled(true);
		energyMidpointLabel_->setEnabled(true);
		endEnergyLabel_->setEnabled(true);
		motorSettingsLabel_->setEnabled(true);
		baseLineLabel_->setEnabled(true);
		elementEdit_->setEnabled(true);
		runTimeDSB_->setEnabled(true);
		startEnergyDSB_->setEnabled(true);
		energyMidpointDSB_->setEnabled(true);
		endEnergyDSB_->setEnabled(true);
		motorSettingsSB_->setEnabled(true);
		baseLineSB_->setEnabled(true);
		warningsLabel_->setText("");
	}
	else{
		presetsComboBox_->setEnabled(false);
		elementLabel_->setEnabled(false);
		runTimeLabel_->setEnabled(false);
		startEnergyLabel_->setEnabled(false);
		energyMidpointLabel_->setEnabled(false);
		endEnergyLabel_->setEnabled(false);
		motorSettingsLabel_->setEnabled(false);
		baseLineLabel_->setEnabled(false);
		elementEdit_->setEnabled(false);
		runTimeDSB_->setEnabled(false);
		startEnergyDSB_->setEnabled(false);
		energyMidpointDSB_->setEnabled(false);
		endEnergyDSB_->setEnabled(false);
		motorSettingsSB_->setEnabled(false);
		baseLineSB_->setEnabled(false);
		warningsLabel_->setText("SGM Beamline Unavailable");
	}
}

/* NTBA March 14, 2011 David Chevrier
   Part of exporter package
void SGMFastScanConfigurationView::onSavePathEditingFinished(){
	SGMFastScanConfiguration *sfsc = qobject_cast<SGMFastScanConfiguration*>(cfg_);
	if(saveEdit_->text().isEmpty())
		return;
	if(sfsc && sfsc->setSensibleFileSavePath(saveEdit_->text())){
		saveFbkLabel_->setText("\tFile will be saved as "+sfsc->finalizedSavePath().section('/', -1));
	}
	else{
		saveEdit_->setText(sfsc->sensibleFileSavePath());
		saveFbkLabel_->setText("\t"+sfsc->sensibleFileSaveWarning());
	}
}
*/

/* NTBA March 14, 2011 David Chevrier
   Part of exporter package
void SGMFastScanConfigurationView::onNewFinalizedSavePath(const QString &savePath){
	saveFbkLabel_->setText("\tFile will be saved as "+savePath.section('/', -1));
}
*/

/* NTBA March 14, 2011 David Chevrier
   Part of exporter package
void SGMFastScanConfigurationView::onSaveDialogDirectoryChosen(const QString &savePath){
	SGMFastScanConfiguration *sfsc = qobject_cast<SGMFastScanConfiguration*>(cfg_);
	QString saveFile = savePath+"/default";
	if(saveFile.isEmpty())
		return;
	if(sfsc && sfsc->setSensibleFileSavePath(saveFile)){
		saveEdit_->setText(saveFile);
		saveFbkLabel_->setText("\tFile will be saved as "+sfsc->finalizedSavePath().section('/', -1));
	}
	else{
		saveEdit_->setText(sfsc->sensibleFileSavePath());
		saveFbkLabel_->setText("\t"+sfsc->sensibleFileSaveWarning());
	}
}
*/
