#include "VESPERSScanConfigurationView.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "util/VESPERS/GeneralUtilities.h"

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

	switch(id){

	case VESPERS::NoXRF:
		break;

	case VESPERS::SingleElement:
		string = "Single Element";
		break;

	case VESPERS::FourElement:
		string = "Four Element";
		break;

	case VESPERS::SingleElement | VESPERS::FourElement:

		QMenu menu(this);
		menu.addAction("Single Element");
		menu.addAction("Four Element");
		QAction *action = menu.exec(QCursor::pos());

		if (action && (action->text() == "Single Element" || action->text() == "Four Element"))
			string = action->text();

		break;
	}

	return string;
}
void VESPERSScanConfigurationView::updateItButtons(int It)
{
	ItGroup_->button(It)->setChecked(true);
}

void VESPERSScanConfigurationView::updateI0Buttons(int I0)
{
	I0Group_->button(I0)->setChecked(true);
}

void VESPERSScanConfigurationView::updateFluorescenceDetector(int detector)
{
	fluorescenceButtonGroup_->button(detector)->setChecked(true);
}

void VESPERSScanConfigurationView::updateCCDDetectorButtons(int detector)
{
	ccdButtonGroup_->button(detector)->setChecked(true);
}

void VESPERSScanConfigurationView::updateMotor(int choice)
{
	motorButtonGroup_->button(choice)->setChecked(true);
}

void VESPERSScanConfigurationView::updateAndSetRoiTextBox(int xrfId)
{
	roiText_->clear();

	if (xrfId ==  (VESPERS::SingleElement | VESPERS::FourElement)){

		AMROIInfoList singleElList = *VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList();
		AMROIInfoList fourElList = *VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList();
		QList<QPair<int, int> > sameList = VESPERS::findRoiPairs(&singleElList, &fourElList);

		if (!sameList.isEmpty()){

			QList<int> singleRoiList;
			QList<int> fourRoiList;

			roiText_->insertPlainText("Same ROI's\n");
			roiText_->insertPlainText("Name\tLow (eV)\tHigh (eV)\n");

			for (int i = 0, count = sameList.size(); i < count; i++){

				QPair<int, int> temp = sameList.at(i);
				singleRoiList << temp.first;
				fourRoiList << temp.second;
				AMROIInfo info = singleElList.at(temp.first);
				roiText_->insertPlainText(GeneralUtilities::addGreek(info.name())+"\t" + QString::number(info.low()) + "\t" + QString::number(info.high()) +"\n");
			}

			if (singleRoiList.size() < singleElList.count() || fourRoiList.size() < fourElList.count()){

				roiText_->insertPlainText("\nDifferent ROI's\n");

				if (singleRoiList.size() < singleElList.count()){

					roiText_->insertPlainText("Single Element Vortex\n");
					roiText_->insertPlainText("Name\tLow (eV)\tHigh (eV)\n");

					for (int i = 0, count = singleElList.count(); i < count; i++){

						if (!singleRoiList.contains(i)){

							AMROIInfo info = singleElList.at(i);
							roiText_->insertPlainText(GeneralUtilities::addGreek(info.name())+"\t" + QString::number(info.low()) + "\t" + QString::number(info.high()) +"\n");
						}
					}

					roiText_->insertPlainText("\n");
				}

				if (fourRoiList.size() < fourElList.count()){

					roiText_->insertPlainText("Four Element Vortex\n");
					roiText_->insertPlainText("Name\tLow (eV)\tHigh (eV)\n");

					for (int i = 0, count = fourElList.count(); i < count; i++){

						if (!fourRoiList.contains(i)){

							AMROIInfo info = fourElList.at(i);
							roiText_->insertPlainText(GeneralUtilities::addGreek(info.name())+"\t" + QString::number(info.low()) + "\t" + QString::number(info.high()) +"\n");
						}
					}
				}
			}
		}

		else {

			roiText_->insertPlainText("Different ROI's\n");
			roiText_->insertPlainText("Single Element Vortex\n");

			for (int i = 0, count = singleElList.count(); i < count; i++){

				AMROIInfo info = singleElList.at(i);
				roiText_->insertPlainText(GeneralUtilities::addGreek(info.name())+"\t" + QString::number(info.low()) + "\t" + QString::number(info.high()) +"\n");
			}

			roiText_->insertPlainText("\nFour Element Vortex\n");

			for (int i = 0, count = fourElList.count(); i < count; i++){

				AMROIInfo info = fourElList.at(i);
				roiText_->insertPlainText(GeneralUtilities::addGreek(info.name())+"\t" + QString::number(info.low()) + "\t" + QString::number(info.high()) +"\n");
			}
		}
	}

	else {

		AMROIInfoList list = AMROIInfoList();

		if (xrfId == VESPERS::SingleElement)
			list = *VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList();

		else if (xrfId == VESPERS::FourElement)
			list = *VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList();

		roiText_->insertPlainText("Name\tLow (eV)\tHigh (eV)\n");

		for (int i = 0; i < list.count(); i++)
			roiText_->insertPlainText(GeneralUtilities::addGreek(list.at(i).name())+"\t" + QString::number(list.at(i).low()) + "\t" + QString::number(list.at(i).high()) +"\n");
	}
}

QGroupBox *VESPERSScanConfigurationView::addFluorescenceDetectorSelectionView()
{
	fluorescenceButtonGroup_ = new QButtonGroup;
	QRadioButton *tempButton;
	QVBoxLayout *fluorescenceDetectorLayout = new QVBoxLayout;

	tempButton = new QRadioButton("None");
	fluorescenceButtonGroup_->addButton(tempButton, 0);
	fluorescenceDetectorLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Single Element Vortex");
	fluorescenceButtonGroup_->addButton(tempButton, 1);
	fluorescenceDetectorLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Four Element Vortex");
	fluorescenceButtonGroup_->addButton(tempButton, 2);
	fluorescenceDetectorLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Single && Four");
	fluorescenceButtonGroup_->addButton(tempButton, 3);	// 3 is SingleElement | FourElement
	fluorescenceDetectorLayout->addWidget(tempButton);

	QGroupBox *fluorescenceDetectorGroupBox = new QGroupBox("Fluorescence Detector");
	fluorescenceDetectorGroupBox->setLayout(fluorescenceDetectorLayout);

	return fluorescenceDetectorGroupBox;
}

QGroupBox *VESPERSScanConfigurationView::addCCDDetectorSelectionView()
{
	ccdButtonGroup_ = new QButtonGroup;
	QRadioButton *tempButton;
	QVBoxLayout *ccdDetectorLayout = new QVBoxLayout;

	tempButton = new QRadioButton("None");
	ccdButtonGroup_->addButton(tempButton, 0);
	ccdDetectorLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Roper");
	ccdButtonGroup_->addButton(tempButton, 1);
	ccdDetectorLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Mar");
	ccdButtonGroup_->addButton(tempButton, 2);
	ccdDetectorLayout->addWidget(tempButton);

	QGroupBox *ccdDetectorGroupBox = new QGroupBox("CCD Detector");
	ccdDetectorGroupBox->setLayout(ccdDetectorLayout);

	return ccdDetectorGroupBox;
}

QGroupBox *VESPERSScanConfigurationView::addI0SelectionView()
{
	QRadioButton *tempButton;
	QVBoxLayout *I0GroupLayout = new QVBoxLayout;

	I0Group_ = new QButtonGroup;
	tempButton = new QRadioButton("Isplit");
	I0Group_->addButton(tempButton, 0);
	I0GroupLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Iprekb");
	I0Group_->addButton(tempButton, 1);
	I0GroupLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Imini");
	tempButton->setChecked(true);
	I0Group_->addButton(tempButton, 2);
	I0GroupLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Ipost");
	I0Group_->addButton(tempButton, 3);
	I0GroupLayout->addWidget(tempButton);

	QGroupBox *I0GroupBox = new QGroupBox("I0");
	I0GroupBox->setLayout(I0GroupLayout);

	return I0GroupBox;
}

QGroupBox *VESPERSScanConfigurationView::addItSelectionView()
{
	QRadioButton *tempButton;
	QVBoxLayout *ItGroupLayout = new QVBoxLayout;

	ItGroup_ = new QButtonGroup;
	tempButton = new QRadioButton("Isplit");
	tempButton->setEnabled(false);
	ItGroup_->addButton(tempButton, 0);
	ItGroupLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Iprekb");
	ItGroup_->addButton(tempButton, 1);
	ItGroupLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Imini");
	ItGroup_->addButton(tempButton, 2);
	ItGroupLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Ipost");
	ItGroup_->addButton(tempButton, 3);
	ItGroupLayout->addWidget(tempButton);

	QGroupBox *ItGroupBox = new QGroupBox("It");
	ItGroupBox->setLayout(ItGroupLayout);

	return ItGroupBox;
}

QGroupBox *VESPERSScanConfigurationView::addMotorSelectionView(QStringList labels, QList<int> ids)
{
	QRadioButton *tempButton;
	QVBoxLayout *motorChoiceLayout = new QVBoxLayout;

	motorButtonGroup_ = new QButtonGroup;

	for (int i = 0, iSize = labels.size(); i < iSize; i++){

		tempButton = new QRadioButton(labels.at(i));
		motorButtonGroup_->addButton(tempButton, ids.at(i));
		motorChoiceLayout->addWidget(tempButton);
	}

	QGroupBox *motorSelectionBox = new QGroupBox("Motors");
	motorSelectionBox->setLayout(motorChoiceLayout);

	return motorSelectionBox;
}

QLineEdit *VESPERSScanConfigurationView::addScanNameView(const QString &name)
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

	xPosition_ = new QDoubleSpinBox;
	xPosition_->setEnabled(goToPositionCheckBox_->isChecked());
	xPosition_->setDecimals(3);
	xPosition_->setRange(-100, 100);
	xPosition_->setValue(goToPosition ? x : 0);
	xPosition_->setSuffix(" mm");

	QHBoxLayout *xLayout = new QHBoxLayout;
	xLayout->addWidget(xPosition_);
	xLayout->addWidget(savedXPosition_);

	yPosition_ = new QDoubleSpinBox;
	yPosition_->setEnabled(goToPositionCheckBox_->isChecked());
	yPosition_->setDecimals(3);
	yPosition_->setRange(-100, 100);
	yPosition_->setValue(goToPosition ? y : 0);
	yPosition_->setSuffix(" mm");

	QHBoxLayout *yLayout = new QHBoxLayout;
	yLayout->addWidget(yPosition_);
	yLayout->addWidget(savedYPosition_);

	QFormLayout *positionLayout = new QFormLayout;
	positionLayout->addRow(goToPositionCheckBox_);
	positionLayout->addRow(saveLayout);
	positionLayout->addRow("x:", xLayout);
	positionLayout->addRow("y:", yLayout);

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

QGroupBox *VESPERSScanConfigurationView::addExporterOptionsView(QStringList list, bool exportSpectra)
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

	QGroupBox *autoExportGroupBox = new QGroupBox("Export Options");
	autoExportGroupBox->setLayout(autoExportLayout);

	return autoExportGroupBox;
}

QDoubleSpinBox *VESPERSScanConfigurationView::addDwellTimeWidget(double time)
{
	QDoubleSpinBox *dwellTime = new QDoubleSpinBox;
	dwellTime->setRange(0, 1000000);
	dwellTime->setValue(time);
	dwellTime->setSuffix(" s");
	dwellTime->setAlignment(Qt::AlignCenter);
	dwellTime->setDecimals(1);

	return dwellTime;
}

QDoubleSpinBox *VESPERSScanConfigurationView::buildPositionDoubleSpinBox(const QString &prefix, const QString &suffix, double value, int decimals)
{
	QDoubleSpinBox *box = new QDoubleSpinBox;
	box->setPrefix(prefix);
	box->setRange(-100000, 100000);
	box->setSuffix(suffix);
	box->setValue(value);
	box->setDecimals(decimals);
	box->setAlignment(Qt::AlignCenter);

	return box;
}
