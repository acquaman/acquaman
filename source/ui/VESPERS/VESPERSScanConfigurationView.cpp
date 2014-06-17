#include "VESPERSScanConfigurationView.h"

#include "beamline/VESPERS/VESPERSBeamline.h"

VESPERSScanConfigurationView::~VESPERSScanConfigurationView(){}

VESPERSScanConfigurationView::VESPERSScanConfigurationView(QWidget *parent)
	: AMScanConfigurationView(parent)
{
	// Setting up the steps to show the time offset for scan time estimation.
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
	setContextMenuPolicy(Qt::CustomContextMenu);
}

void VESPERSScanConfigurationView::onCustomContextMenuRequested(QPoint pos)
{
	QMenu popup(this);

	QAction *temp = popup.addAction("Set time offset");
	temp = popup.exec(mapToGlobal(pos));

	// If a valid action was selected.
	if (temp && (temp->text() == "Set time offset")){

		timeOffsetLabel_->setVisible(!timeOffsetLabel_->isVisible());
		timeOffset_->setVisible(!timeOffset_->isVisible());
	}
}

QString VESPERSScanConfigurationView::fluorescenceDetectorIdToString(int id)
{
	QString string = QString();

	if (id == VESPERS::SingleElement)
		string = "Single Element";

	else if (id == VESPERS::FourElement)
		string = "Four Element";

	else if (id == int(VESPERS::SingleElement | VESPERS::FourElement)){

		QMenu menu(this);
		menu.addAction("Single Element");
		menu.addAction("Four Element");
		QAction *action = menu.exec(QCursor::pos());

		if (action && (action->text() == "Single Element" || action->text() == "Four Element"))
			string = action->text();
	}

	return string;
}

QString VESPERSScanConfigurationView::ccdDetectorIdToString(int id)
{
	QString string = QString();

	switch(id){

	case VESPERS::NoCCD:
		break;

	case VESPERS::Roper:
		string = "Roper CCD";
		break;

	case VESPERS::Mar:
		string = "Mar CCD";
		break;

	case VESPERS::Pilatus:
		string = "Pilatus";
		break;
	}

	return string;
}

void VESPERSScanConfigurationView::updateItComboBox(int It)
{
	itComboBox_->setCurrentIndex(It);
}

void VESPERSScanConfigurationView::updateI0ComboBox(int I0)
{
	i0ComboBox_->setCurrentIndex(I0);
}

void VESPERSScanConfigurationView::updateFluorescenceDetectorComboBox(int detector)
{
	fluorescenceDetectorComboBox_->setCurrentIndex(detector);
}

void VESPERSScanConfigurationView::updateCCDDetectorComboBox(int detector)
{
	ccdComboBox_->setCurrentIndex(detector);
}

void VESPERSScanConfigurationView::updateMotorSelectionComboBox(int choice)
{
	motorSelectionComboBox_->setCurrentIndex(choice);
}

void VESPERSScanConfigurationView::updateAndSetRoiTextBox(int xrfId)
{Q_UNUSED(xrfId);
//	roiText_->clear();

//	if (xrfId ==  (VESPERS::SingleElement | VESPERS::FourElement)){

//		AMROIInfoList singleElList = *VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList();
//		AMROIInfoList fourElList = *VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList();
//		QList<QPair<int, int> > sameList = VESPERS::findRoiPairs(&singleElList, &fourElList);

//		if (!sameList.isEmpty()){

//			QList<int> singleRoiList;
//			QList<int> fourRoiList;

//			roiText_->insertPlainText("Same ROI's\n");
//			roiText_->insertPlainText("Name\tLow (eV)\tHigh (eV)\n");

//			for (int i = 0, count = sameList.size(); i < count; i++){

//				QPair<int, int> temp = sameList.at(i);
//				singleRoiList << temp.first;
//				fourRoiList << temp.second;
//				AMROIInfo info = singleElList.at(temp.first);
//				roiText_->insertPlainText(GeneralUtilities::addGreek(info.name())+"\t" + QString::number(info.low()) + "\t" + QString::number(info.high()) +"\n");
//			}

//			if (singleRoiList.size() < singleElList.count() || fourRoiList.size() < fourElList.count()){

//				roiText_->insertPlainText("\nDifferent ROI's\n");

//				if (singleRoiList.size() < singleElList.count()){

//					roiText_->insertPlainText("Single Element Vortex\n");
//					roiText_->insertPlainText("Name\tLow (eV)\tHigh (eV)\n");

//					for (int i = 0, count = singleElList.count(); i < count; i++){

//						if (!singleRoiList.contains(i)){

//							AMROIInfo info = singleElList.at(i);
//							roiText_->insertPlainText(GeneralUtilities::addGreek(info.name())+"\t" + QString::number(info.low()) + "\t" + QString::number(info.high()) +"\n");
//						}
//					}

//					roiText_->insertPlainText("\n");
//				}

//				if (fourRoiList.size() < fourElList.count()){

//					roiText_->insertPlainText("Four Element Vortex\n");
//					roiText_->insertPlainText("Name\tLow (eV)\tHigh (eV)\n");

//					for (int i = 0, count = fourElList.count(); i < count; i++){

//						if (!fourRoiList.contains(i)){

//							AMROIInfo info = fourElList.at(i);
//							roiText_->insertPlainText(GeneralUtilities::addGreek(info.name())+"\t" + QString::number(info.low()) + "\t" + QString::number(info.high()) +"\n");
//						}
//					}
//				}
//			}
//		}

//		else {

//			roiText_->insertPlainText("Different ROI's\n");
//			roiText_->insertPlainText("Single Element Vortex\n");

//			for (int i = 0, count = singleElList.count(); i < count; i++){

//				AMROIInfo info = singleElList.at(i);
//				roiText_->insertPlainText(GeneralUtilities::addGreek(info.name())+"\t" + QString::number(info.low()) + "\t" + QString::number(info.high()) +"\n");
//			}

//			roiText_->insertPlainText("\nFour Element Vortex\n");

//			for (int i = 0, count = fourElList.count(); i < count; i++){

//				AMROIInfo info = fourElList.at(i);
//				roiText_->insertPlainText(GeneralUtilities::addGreek(info.name())+"\t" + QString::number(info.low()) + "\t" + QString::number(info.high()) +"\n");
//			}
//		}
//	}

//	else {

//		AMROIInfoList list = AMROIInfoList();

//		if (xrfId == VESPERS::SingleElement)
//			list = *VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList();

//		else if (xrfId == VESPERS::FourElement)
//			list = *VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList();

//		roiText_->insertPlainText("Name\tLow (eV)\tHigh (eV)\n");

//		for (int i = 0; i < list.count(); i++)
//			roiText_->insertPlainText(GeneralUtilities::addGreek(list.at(i).name())+"\t" + QString::number(list.at(i).low()) + "\t" + QString::number(list.at(i).high()) +"\n");
//	}
}

QComboBox *VESPERSScanConfigurationView::createFluorescenceComboBox()
{
	QComboBox *newComboBox = new QComboBox;
	newComboBox->insertItem(0, "None");
	newComboBox->insertItem(1, "Single Element Vortex");
	newComboBox->insertItem(2, "Four Element Vortex");
	newComboBox->insertItem(3, "Single & Four");

	return newComboBox;
}

QComboBox *VESPERSScanConfigurationView::createCCDComboBox()
{
	QComboBox *newComboBox = new QComboBox;
	newComboBox->insertItem(0, "None");
	newComboBox->insertItem(1, "Roper");
	newComboBox->insertItem(2, "Mar");
	newComboBox->insertItem(3, "Pilatus");

	return newComboBox;
}

QComboBox *VESPERSScanConfigurationView::createIonChamberComboBox()
{
	QComboBox *newComboBox = new QComboBox;
	newComboBox->insertItem(0, "Isplit");
	newComboBox->insertItem(1, "Iprekb");
	newComboBox->insertItem(2, "Imini");
	newComboBox->insertItem(3, "Ipost");

	return newComboBox;
}

QComboBox *VESPERSScanConfigurationView::createMotorSelectionComboBox(QStringList labels, QList<int> ids)
{
	QComboBox *newComboBox = new QComboBox;

	for (int i = 0, size = ids.size(); i < size; i++)
		newComboBox->insertItem(ids.at(i), labels.at(i));

	return newComboBox;
}

QLineEdit *VESPERSScanConfigurationView::createScanNameView(const QString &name)
{
	QLineEdit *scanName = new QLineEdit;
	scanName->setText(name);
	scanName->setAlignment(Qt::AlignCenter);

	return scanName;
}

QGroupBox *VESPERSScanConfigurationView::addGoToPositionView(bool goToPosition, double x, double y)
{
	goToPositionCheckBox_ = new QCheckBox("Choose Position");
	goToPositionCheckBox_->setChecked(goToPosition);

	setCurrentPositionButton_ = new QPushButton(QIcon(":/save.png"), "");
	setCurrentPositionButton_->setEnabled(goToPosition);

	savedXPosition_ = new QLabel(QString::number(goToPosition ? x : 0, 'g', 3) + " mm");
	savedXPosition_->setEnabled(goToPosition);
	savedYPosition_ = new QLabel(QString::number(goToPosition ? y : 0, 'g', 3) + " mm");
	savedYPosition_->setEnabled(goToPosition);

	positionsSaved_ = new QLabel;
	QPalette palette(this->palette());
	palette.setColor(QPalette::Disabled, QPalette::WindowText, Qt::darkGray);

	// Although not entirely valid, the chances that both x and y being identically 0 for a saved position is incredibly unlikely.
	if (!goToPosition || (x == 0.0 && y == 0.0)){

		positionsSaved_->setText("Unsaved");
		palette.setColor(QPalette::Active, QPalette::WindowText, Qt::red);
	}
	else{

		positionsSaved_->setText("Saved");
		palette.setColor(QPalette::Active, QPalette::WindowText, Qt::darkGreen);
	}

	positionsSaved_->setPalette(palette);

	QFont font(this->font());
	font.setBold(true);
	positionsSaved_->setFont(font);
	positionsSaved_->setEnabled(goToPosition);

	QHBoxLayout *saveLayout = new QHBoxLayout;
	saveLayout->addWidget(setCurrentPositionButton_);
	saveLayout->addWidget(positionsSaved_);

	QVBoxLayout *positionLayout = new QVBoxLayout;
	positionLayout->addWidget(goToPositionCheckBox_, 0, Qt::AlignLeft);
	positionLayout->addLayout(saveLayout);
	positionLayout->addWidget(savedXPosition_);
	positionLayout->addWidget(savedYPosition_);

	QGroupBox *positionGroupBox = new QGroupBox("Go To Position");
	positionGroupBox->setLayout(positionLayout);

	return positionGroupBox;
}

QLabel *VESPERSScanConfigurationView::addExportPathLabel()
{
	QString exportString =  AMUserSettings::userDataFolder;
	exportString.remove("/userData");

	return new QLabel(QString("Data exported to: %1exportData").arg(exportString));
}

QGroupBox *VESPERSScanConfigurationView::addTimeOffsetLabel(double time)
{
	timeOffsetLabel_ = new QLabel("Scan time offset:");
	timeOffsetLabel_->hide();

	timeOffset_ = new QDoubleSpinBox;
	timeOffset_->hide();
	timeOffset_->setRange(0, 100);
	timeOffset_->setDecimals(2);
	timeOffset_->setSingleStep(0.1);
	timeOffset_->setSuffix(" s");
	timeOffset_->setAlignment(Qt::AlignCenter);
	timeOffset_->setValue(time);

	QHBoxLayout *timeOffsetLayout = new QHBoxLayout;
	timeOffsetLayout->addWidget(timeOffsetLabel_);
	timeOffsetLayout->addWidget(timeOffset_);

	QGroupBox *timeOffsetBox = new QGroupBox;
	timeOffsetBox->setLayout(timeOffsetLayout);
	timeOffsetBox->setFlat(true);

	return timeOffsetBox;
}

QGroupBox *VESPERSScanConfigurationView::addExporterOptionsView(QStringList list, bool exportSpectra, bool exportSpectraInRows)
{
	QRadioButton *autoExportButton;
	QVBoxLayout *autoExportLayout = new QVBoxLayout;
	autoExportButtonGroup_ = new QButtonGroup;

	for (int i = 0, iSize = list.size(); i < iSize; i++){

		autoExportButton = new QRadioButton(list.at(i));
		autoExportButtonGroup_->addButton(autoExportButton, i);
		autoExportLayout->addWidget(autoExportButton);
	}

	autoExportSpectra_ = new QCheckBox("Export Spectra");
	autoExportSpectra_->setChecked(exportSpectra);
	autoExportLayout->addWidget(autoExportSpectra_);

	exportSpectraInRows_ = new QCheckBox("Export spectra in columns instead of rows.");
	exportSpectraInRows_->setChecked(!exportSpectraInRows);
	autoExportLayout->addWidget(exportSpectraInRows_);

	QGroupBox *autoExportGroupBox = new QGroupBox("Export Options");
	autoExportGroupBox->setLayout(autoExportLayout);

	return autoExportGroupBox;
}

QDoubleSpinBox *VESPERSScanConfigurationView::createDwellTimeSpinBox(double time)
{
	QDoubleSpinBox *dwellTime = new QDoubleSpinBox;
	dwellTime->setRange(0, 1000000);
	dwellTime->setSuffix(" s");
	dwellTime->setAlignment(Qt::AlignCenter);
	dwellTime->setDecimals(1);
	dwellTime->setValue(time);

	return dwellTime;
}

QDoubleSpinBox *VESPERSScanConfigurationView::createPositionDoubleSpinBox(const QString &prefix, const QString &suffix, double value, int decimals)
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
