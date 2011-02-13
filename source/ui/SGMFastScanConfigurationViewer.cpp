#include "SGMFastScanConfigurationViewer.h"

#include "SGMSidebar.h"

SGMFastScanConfigurationViewer::SGMFastScanConfigurationViewer(SGMFastScanConfiguration *sfsc, QWidget *parent) :
		AMScanConfigurationViewer(sfsc, parent)
{
	cfg_ = NULL;
	autoSavePath_ = "";
	autoSaveDialog_ = 0; //NULL
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
		//scalerTimeLabel_ = new QLabel("Scaler Time");

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
		/*
		scalerTimeDSB_ = new QDoubleSpinBox();
		scalerTimeDSB_->setMinimum(1.0);
		scalerTimeDSB_->setMaximum(1000.0);
		scalerTimeDSB_->setValue(sfsc->scalerTime());
		*/

		connect(sfsc, SIGNAL(onElementChanged(QString)), elementEdit_, SLOT(setText(QString)));
		connect(sfsc, SIGNAL(onRunSecondsChanged(double)), runTimeDSB_, SLOT(setValue(double)));
		connect(sfsc, SIGNAL(onEnergyStartChanged(double)), startEnergyDSB_, SLOT(setValue(double)));
		connect(sfsc, SIGNAL(onEnergyMidpointChanged(double)), energyMidpointDSB_, SLOT(setValue(double)));
		connect(sfsc, SIGNAL(onEnergyEndChanged(double)), endEnergyDSB_, SLOT(setValue(double)));
		connect(sfsc, SIGNAL(onVelocityChanged(int)), motorSettingsSB_, SLOT(setValue(int)));
		//connect(sfsc, SIGNAL(onScalerTimeChanged(double)), scalerTimeDSB_, SLOT(setValue(double)));

		connect(elementEdit_, SIGNAL(textEdited(QString)), sfsc, SLOT(setElement(QString)));
		connect(runTimeDSB_, SIGNAL(valueChanged(double)), sfsc, SLOT(setRunSeconds(double)));
		connect(runTimeDSB_, SIGNAL(valueChanged(double)), sfsc, SLOT(setScalerTime(double)));
		connect(startEnergyDSB_, SIGNAL(valueChanged(double)), sfsc, SLOT(setEnergyStart(double)));
		connect(energyMidpointDSB_, SIGNAL(valueChanged(double)), sfsc, SLOT(setEnergyMidpoint(double)));
		connect(endEnergyDSB_, SIGNAL(valueChanged(double)), sfsc, SLOT(setEnergyEnd(double)));
		connect(motorSettingsSB_, SIGNAL(valueChanged(int)), sfsc, SLOT(setVelocity(int)));
		connect(motorSettingsSB_, SIGNAL(valueChanged(int)), sfsc, SLOT(setVelocityBase(int)));
		connect(motorSettingsSB_, SIGNAL(valueChanged(int)), sfsc, SLOT(setAcceleration(int)));
		//connect(scalerTimeDSB_, SIGNAL(valueChanged(double)), sfsc, SLOT(setScalerTime(double)));

		fl_ = new QFormLayout();
		fl_->addRow(elementLabel_, elementEdit_);
		fl_->addRow(runTimeLabel_, runTimeDSB_);
		fl_->addRow(startEnergyLabel_, startEnergyDSB_);
		fl_->addRow(energyMidpointLabel_, energyMidpointDSB_);
		fl_->addRow(endEnergyLabel_, endEnergyDSB_);
		fl_->addRow(motorSettingsLabel_, motorSettingsSB_);
		//fl_->addRow(scalerTimeLabel_, scalerTimeDSB_);

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
		//fl2_->addRow(saveFbkLabel_, autoSaveDialogButton_);
		fl2_->addRow(hl);

		connect(sfsc, SIGNAL(onNewFinalizedSavePath(QString)), this, SLOT(onNewFinalizedSavePath(QString)));

		startScanButton_ = new QPushButton();
		startScanButton_->setText("Start Scan");
		addToQueueButton_ = new QPushButton();
		addToQueueButton_->setText("Add Scan to Queue");
		queueDirectorButton_ = new QPushButton();
		queueDirectorButton_->setText("Queue Director");
		int buttonWidth = std::max(startScanButton_->sizeHint().width(), addToQueueButton_->sizeHint().width());
		buttonWidth = std::max(buttonWidth, queueDirectorButton_->sizeHint().width());
		int buttonHeight = std::max(startScanButton_->sizeHint().height(), addToQueueButton_->sizeHint().height());
		buttonHeight = std::max(buttonHeight, queueDirectorButton_->sizeHint().height());
		startScanButton_->setMinimumSize(buttonWidth, (int)(1.75*buttonHeight));
		addToQueueButton_->setMinimumSize(buttonWidth, buttonHeight);
		queueDirectorButton_->setMinimumSize(buttonWidth, buttonHeight);
		connect(startScanButton_, SIGNAL(clicked()), this, SLOT(onStartScanClicked()));
		connect(addToQueueButton_, SIGNAL(clicked()), this, SLOT(onAddToQueueRequested()));
		connect(queueDirectorButton_, SIGNAL(clicked()), this, SIGNAL(queueDirectorRequested()));

		gl_.addWidget(presetsComboBox_,		0, 0, 1, 1, Qt::AlignCenter);
		gl_.addLayout(fl_,			0, 1, 1, 1, Qt::AlignCenter);
		gl_.addLayout(fl2_,			1, 0, 1, 2, Qt::AlignCenter);
		gl_.addWidget(startScanButton_,		5, 3, 1, 2, Qt::AlignRight);
		gl_.addWidget(addToQueueButton_,	6, 3, 1, 2, Qt::AlignRight);
		gl_.addWidget(queueDirectorButton_,	7, 3, 1, 2, Qt::AlignRight);
		this->setLayout(&gl_);
		this->setMaximumSize(700, 800);
	}
}

void SGMFastScanConfigurationViewer::onLockdowScanning(bool isLocked, QString reason){
	if(isLocked){
		startScanButton_->setEnabled(false);
		startScanButton_->setText("Start Scan\n"+reason);
	}
	else{
		startScanButton_->setEnabled(true);
		startScanButton_->setText("Start Scan");
	}
}

void SGMFastScanConfigurationViewer::onSavePathEditingFinished(){
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

void SGMFastScanConfigurationViewer::onNewFinalizedSavePath(const QString &savePath){
	saveFbkLabel_->setText("\tFile will be saved as "+savePath.section('/', -1));
}

void SGMFastScanConfigurationViewer::onSaveDialogDirectoryChosen(const QString &savePath){
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

void SGMFastScanConfigurationViewer::onStartScanClicked(){
	SGMFastScanConfiguration *sfsc = qobject_cast<SGMFastScanConfiguration*>(cfg_);
	emit lastSettings(sfsc->currentParameters());
	AMScanConfigurationViewer::onStartScanClicked();
}
