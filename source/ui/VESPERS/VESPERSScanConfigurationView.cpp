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
	ccdComboBox_->setCurrentIndex(ccdComboBox_->findData(detector));
}

void VESPERSScanConfigurationView::updateMotorSelectionComboBox(int choice)
{
	motorSelectionComboBox_->setCurrentIndex(motorSelectionComboBox_->findData(choice));
}

QComboBox *VESPERSScanConfigurationView::createFluorescenceComboBox()
{
	QComboBox *newComboBox = new QComboBox;
	newComboBox->insertItem(0, "None");
	newComboBox->insertItem(1, "1E Vortex");
	newComboBox->insertItem(2, "4E Vortex");
	newComboBox->insertItem(3, "1E & 4E");

	return newComboBox;
}

QComboBox *VESPERSScanConfigurationView::createCCDComboBox()
{
	QComboBox *newComboBox = new QComboBox;
	newComboBox->addItem("None", 0);
	newComboBox->addItem("Roper", 1);
	newComboBox->addItem("Mar", 2);
	newComboBox->addItem("Pilatus", 4);

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
		newComboBox->addItem(labels.at(i), ids.at(i));

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
	dwellTime->setMinimumWidth(100);

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

void VESPERSScanConfigurationView::disableStandardFluorescenceOptions()
{
	QStandardItemModel *model = qobject_cast<QStandardItemModel *>(fluorescenceDetectorComboBox_->model());

	for (int i = 0; i < fluorescenceDetectorComboBox_->count(); i++)
		if (model->item(i)->text() == "None")
			model->item(i)->setFlags(Qt::NoItemFlags);
}

void VESPERSScanConfigurationView::disableStandardXRDOptions()
{
	QStandardItemModel *model = qobject_cast<QStandardItemModel *>(ccdComboBox_->model());

	for (int i = 0; i < ccdComboBox_->count(); i++)
		if (model->item(i)->text() == "Roper" || model->item(i)->text() == "Mar")
			model->item(i)->setFlags(Qt::NoItemFlags);
}

void VESPERSScanConfigurationView::disableStandardI0Options()
{
	QStandardItemModel *model = qobject_cast<QStandardItemModel *>(i0ComboBox_->model());

	for (int i = 0; i < i0ComboBox_->count(); i++)
		if (model->item(i)->text() == "Ipost")
			model->item(i)->setFlags(Qt::NoItemFlags);
}

void VESPERSScanConfigurationView::disableStandardItOptions()
{
	QStandardItemModel *model = qobject_cast<QStandardItemModel *>(itComboBox_->model());

	for (int i = 0; i < itComboBox_->count(); i++)
		if (model->item(i)->text() == "Isplit")
			model->item(i)->setFlags(Qt::NoItemFlags);
}

QGroupBox * VESPERSScanConfigurationView::createAfterScanOptionsBox(bool autoCloseShutter, bool gotoOriginalPosition, bool cleanupScaler)
{
	QGroupBox *box = new QGroupBox("After Scan");
	QVBoxLayout *layout = new QVBoxLayout;

	closeFastShutterCheckBox_ = new QCheckBox("Close Shutter");
	closeFastShutterCheckBox_->setChecked(autoCloseShutter);

	goToPositionCheckBox_ = new QCheckBox("Go to original position");
	goToPositionCheckBox_->setChecked(gotoOriginalPosition);

	cleanupScalerCheckBox_ = new QCheckBox("Cleanup Scaler");
	cleanupScalerCheckBox_->setChecked(cleanupScaler);

	layout->addWidget(closeFastShutterCheckBox_);
	layout->addWidget(goToPositionCheckBox_);
	layout->addWidget(cleanupScalerCheckBox_);

	box->setLayout(layout);

	return box;
}
