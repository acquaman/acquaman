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


#include "AMSampleCameraWizardSelector.h"

#include <QFrame>
#include <QLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QLineEdit>
#include <QApplication>
#include <QLabel>
#include <QMenu>
#include <QAction>

AMSampleCameraWizardSelector::AMSampleCameraWizardSelector(QWidget* parent)
	: QWidget(parent)
{

	beamWizardComplete_ = false;
	cameraWizardComplete_ = false;
	samplePlateWizardComplete_ = false;
	rotationWizardComplete_ = false;

	QVBoxLayout* sampleCameraWizardSelectorLayout = new QVBoxLayout();
	QFrame* beamWizardFrame = new QFrame();
	QHBoxLayout* beamWizardLayout = new QHBoxLayout();
	QFrame* cameraWizardFrame = new QFrame();
	QHBoxLayout* cameraWizardLayout = new QHBoxLayout();
	QFrame* samplePlateWizardFrame = new QFrame();
	QHBoxLayout* samplePlateWizardLayout = new QHBoxLayout();
	QFrame* rotationWizardFrame = new QFrame();
	QHBoxLayout* rotationWizardLayout = new QHBoxLayout();

	QFrame *reloadAllFrame = new QFrame();
	QHBoxLayout *reloadAllLayout = new QHBoxLayout();

	sampleCameraWizardSelectorLayout->setContentsMargins(0,0,0,0);
	beamWizardLayout->setContentsMargins(0,0,0,0);
	cameraWizardLayout->setContentsMargins(0,0,0,0);
	samplePlateWizardLayout->setContentsMargins(0,0,0,0);
	rotationWizardLayout->setContentsMargins(0,0,0,0);
	reloadAllLayout->setContentsMargins(0,0,0,0);

	cameraWizardButton_ = new QPushButton("Setup Camera");
	cameraWizardLayout->addWidget(cameraWizardButton_);
	cameraWizardLayout->addStretch();
	loadCameraConfigurationButton_ = new QPushButton("Load");
	cameraWizardLayout->addWidget(loadCameraConfigurationButton_);
	cameraWizardFrame->setLayout(cameraWizardLayout);
	loadCameraConfigurationButton_->setContextMenuPolicy(Qt::CustomContextMenu);

	rotationWizardButton_ = new QPushButton("Setup Rotation");
	rotationWizardLayout->addWidget(rotationWizardButton_);
	rotationWizardLayout->addStretch();
	loadRotationConfigurationButton_ = new QPushButton("Load");
	rotationWizardLayout->addWidget(loadRotationConfigurationButton_);
	rotationWizardFrame->setLayout(rotationWizardLayout);
	rotationWizardButton_->setEnabled(false);
	loadRotationConfigurationButton_->setEnabled(false);
	loadRotationConfigurationButton_->setContextMenuPolicy(Qt::CustomContextMenu);

	samplePlateWizardButton_ = new QPushButton("Setup Sample Plate");
	samplePlateWizardLayout->addWidget(samplePlateWizardButton_);
	samplePlateWizardLayout->addStretch();
	loadSamplePlateButton_ = new QPushButton("Load");
	samplePlateWizardLayout->addWidget(loadSamplePlateButton_);
	samplePlateWizardFrame->setLayout(samplePlateWizardLayout);
	samplePlateWizardButton_->setEnabled(false);
	loadSamplePlateButton_->setEnabled(false);
	loadSamplePlateButton_->setContextMenuPolicy(Qt::CustomContextMenu);

	beamWizardLayout->addWidget(beamWizardButton_ = new QPushButton("Setup Beam"));
	beamWizardLayout->addStretch();
	loadBeamConfigurationButton_ = new QPushButton("Load");
	beamWizardLayout->addWidget(loadBeamConfigurationButton_);
	beamWizardFrame->setLayout(beamWizardLayout);
	beamWizardButton_->setEnabled(false);
	loadBeamConfigurationButton_->setEnabled(false);
	loadBeamConfigurationButton_->setContextMenuPolicy(Qt::CustomContextMenu);

	reloadAllLayout->addStretch();
	loadAllConfigurationsButton_ = new QPushButton("Reload All");
	reloadAllLayout->addWidget(loadAllConfigurationsButton_);
	reloadAllFrame->setLayout(reloadAllLayout);

	sampleCameraWizardSelectorLayout->addWidget(cameraWizardFrame);
	sampleCameraWizardSelectorLayout->addWidget(rotationWizardFrame);
	sampleCameraWizardSelectorLayout->addWidget(samplePlateWizardFrame);
	sampleCameraWizardSelectorLayout->addWidget(beamWizardFrame);
	sampleCameraWizardSelectorLayout->addWidget(reloadAllFrame);
	sampleCameraWizardSelectorLayout->addStretch();
	setLayout(sampleCameraWizardSelectorLayout);

	beamWizardButton_->setIcon(QIcon(":/22x22/redLEDOn.png"));
	cameraWizardButton_->setIcon(QIcon(":/22x22/redLEDOn.png"));
	samplePlateWizardButton_->setIcon(QIcon(":/22x22/redLEDOn.png"));
	rotationWizardButton_->setIcon(QIcon(":/22x22/redLEDOn.png"));

	connect(beamWizardButton_, SIGNAL(clicked()), this, SIGNAL(beamWizardPressed()));
	connect(cameraWizardButton_, SIGNAL(clicked()), this, SLOT(onCameraWizardButtonClicked()));
	connect(samplePlateWizardButton_, SIGNAL(clicked()), this, SIGNAL(samplePlateWizardPressed()));
	connect(rotationWizardButton_, SIGNAL(clicked()), this, SLOT(onRotationWizardButtonClicked()));

	connect(loadBeamConfigurationButton_, SIGNAL(clicked()), this, SIGNAL(requestLoadBeamConfiguration()));
	connect(loadCameraConfigurationButton_, SIGNAL(clicked()), this, SIGNAL(requestLoadCameraConfiguration()));
	connect(loadSamplePlateButton_, SIGNAL(clicked()), this, SIGNAL(requestLoadSamplePlate()));
	connect(loadRotationConfigurationButton_, SIGNAL(clicked()), this, SIGNAL(requestLoadRotationConfiguration()));

	connect(loadAllConfigurationsButton_, SIGNAL(clicked()), this, SLOT(onLoadAllConfigurationsButtonClicked()));

	connect(this, SIGNAL(beamCompleteChanged()), this, SLOT(onBeamCompleteChanged()));
	connect(this, SIGNAL(cameraCompleteChanged()), this, SLOT(onCameraCompleteChanged()));
	connect(this, SIGNAL(samplePlateCompleteChanged()), this, SLOT(onSamplePlateCompleteChanged()));
	connect(this, SIGNAL(rotationCompleteChanged()), this, SLOT(onRotationCompleteChanged()));

	connect(loadBeamConfigurationButton_, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onLoadBeamConfigurationButtonCustomContextMenuRequested(QPoint)));
	connect(loadCameraConfigurationButton_, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onLoadCameraConfigurationButtonCustomContextMenuRequested(QPoint)));
	connect(loadSamplePlateButton_, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onLoadSamplePlateConfigurationButtonCustomContextMenuRequested(QPoint)));
	connect(loadRotationConfigurationButton_, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onLoadRotationConfigurationButtonCustomContextMenuRequested(QPoint)));
}

bool AMSampleCameraWizardSelector::beamWizardComplete() const
{
	return beamWizardComplete_;
}

bool AMSampleCameraWizardSelector::cameraWizardComplete() const
{
	return cameraWizardComplete_;
}

bool AMSampleCameraWizardSelector::samplePlateWizardComplete() const
{
	return samplePlateWizardComplete_;
}

bool AMSampleCameraWizardSelector::rotationWizardComplete() const
{
	return rotationWizardComplete_;
}

void AMSampleCameraWizardSelector::setBeamWizardComplete(const bool &complete)
{
	if(complete != beamWizardComplete_)
	{
		beamWizardComplete_ = complete;
		emit beamCompleteChanged();
	}
}

void AMSampleCameraWizardSelector::setCameraWizardComplete(const bool &complete)
{
	if(complete != cameraWizardComplete_)
	{
		cameraWizardComplete_ = complete;
		emit cameraCompleteChanged();
	}
}

void AMSampleCameraWizardSelector::setSamplePlateWizardComplete(const bool &complete)
{
	if(complete != samplePlateWizardComplete_)
	{
		samplePlateWizardComplete_ = complete;
		emit samplePlateCompleteChanged();
	}
}

void AMSampleCameraWizardSelector::setRotationWizardComplete(const bool &complete)
{
	if(complete != rotationWizardComplete_)
	{
		rotationWizardComplete_ = complete;
		emit rotationCompleteChanged();
	}
}

void AMSampleCameraWizardSelector::onBeamWizardFinished()
{
	setBeamWizardComplete(true);
}

void AMSampleCameraWizardSelector::onCameraWizardFinished()
{
	setCameraWizardComplete(true);
}

void AMSampleCameraWizardSelector::onSamplePlateWizardFinished()
{
	setSamplePlateWizardComplete(true);
}

void AMSampleCameraWizardSelector::onRotationWizardFinished()
{
	setRotationWizardComplete(true);
}

void AMSampleCameraWizardSelector::onBeamCompleteChanged()
{

	if(beamWizardComplete())
	{
		beamWizardButton_->setIcon(QIcon(":/22x22/greenLEDOn.png"));
	}
	else
	{
		beamWizardButton_->setIcon(QIcon(":/22x22/redLEDOn.png"));
	}
}

void AMSampleCameraWizardSelector::onCameraCompleteChanged()
{
	if(cameraWizardComplete())
	{
		cameraWizardButton_->setIcon(QIcon(":/22x22/greenLEDOn.png"));

		rotationWizardButton_->setEnabled(true);
		loadRotationConfigurationButton_->setEnabled(true);
	}
	else
	{
		cameraWizardButton_->setIcon(QIcon(":/22x22/redLEDOn.png"));
	}
}

void AMSampleCameraWizardSelector::onSamplePlateCompleteChanged()
{
	if(samplePlateWizardComplete())
	{
		samplePlateWizardButton_->setIcon(QIcon(":/22x22/greenLEDOn.png"));

		beamWizardButton_->setEnabled(true);
		loadBeamConfigurationButton_->setEnabled(true);
	}
	else
	{
		samplePlateWizardButton_->setIcon(QIcon(":/22x22/redLEDOn.png"));
	}
}

void AMSampleCameraWizardSelector::onRotationCompleteChanged()
{
	if(rotationWizardComplete())
	{
		rotationWizardButton_->setIcon(QIcon(":/22x22/greenLEDOn.png"));

		samplePlateWizardButton_->setEnabled(true);
		loadSamplePlateButton_->setEnabled(true);
	}
	else
	{
		rotationWizardButton_->setIcon(QIcon(":/22x22/redLEDOn.png"));
	}
}

void AMSampleCameraWizardSelector::onLoadAllConfigurationsButtonClicked(){
	emit requestLoadCameraConfiguration();
	emit requestLoadRotationConfiguration();
	emit requestLoadSamplePlate();
	emit requestLoadBeamConfiguration();
}

void AMSampleCameraWizardSelector::onCameraWizardButtonClicked(){
	if(QApplication::instance()->arguments().contains("--developmentMode"))
		emit cameraWizardPressed();
	else{
		QMessageBox messageBox;
		messageBox.setText("You don't usually have to set up this feature. If you want to do so, please enter the password.");
		QPushButton *okButton = messageBox.addButton(QMessageBox::Ok);
		messageBox.addButton(QMessageBox::Cancel);
		QLineEdit *passwordLineEdit = new QLineEdit();
		passwordLineEdit->setEchoMode(QLineEdit::Password);
		QHBoxLayout *passwordHL = new QHBoxLayout();
		passwordHL->addWidget(new QLabel("Password: "));
		passwordHL->addWidget(passwordLineEdit);
		QWidget *passwordWidget = new QWidget();
		passwordWidget->setLayout(passwordHL);
		messageBox.layout()->addWidget(passwordWidget);

		messageBox.exec();

		if (messageBox.clickedButton() == okButton && passwordLineEdit->text() == "onmyheadbeit")
			emit cameraWizardPressed();
	}
}

void AMSampleCameraWizardSelector::onRotationWizardButtonClicked(){
	if(QApplication::instance()->arguments().contains("--developmentMode"))
		emit rotationWizardPressed();
	else{
		QMessageBox messageBox;
		messageBox.setText("You don't usually have to set up this feature. If you want to do so, please enter the password.");
		QPushButton *okButton = messageBox.addButton(QMessageBox::Ok);
		messageBox.addButton(QMessageBox::Cancel);
		QLineEdit *passwordLineEdit = new QLineEdit();
		passwordLineEdit->setEchoMode(QLineEdit::Password);
		QHBoxLayout *passwordHL = new QHBoxLayout();
		passwordHL->addWidget(new QLabel("Password: "));
		passwordHL->addWidget(passwordLineEdit);
		QWidget *passwordWidget = new QWidget();
		passwordWidget->setLayout(passwordHL);
		messageBox.layout()->addWidget(passwordWidget);

		messageBox.exec();

		if (messageBox.clickedButton() == okButton && passwordLineEdit->text() == "onmyheadbeit")
			emit rotationWizardPressed();
	}
}

void AMSampleCameraWizardSelector::onLoadBeamConfigurationButtonCustomContextMenuRequested(QPoint point){
	QMenu popup(this);
	QAction *temp = popup.addAction("Load A Beam Configuration from the Database");
	temp = popup.exec(loadBeamConfigurationButton_->mapToGlobal(point));

	// If a valid action was selected.
	if (temp)
		emit requestLoadBeamConfigurationFromDatabase();
}

void AMSampleCameraWizardSelector::onLoadCameraConfigurationButtonCustomContextMenuRequested(QPoint point){
	QMenu popup(this);
	QAction *temp = popup.addAction("Load A Camera Configuration from the Database");
	temp = popup.exec(loadCameraConfigurationButton_->mapToGlobal(point));

	// If a valid action was selected.
	if (temp)
		emit requestLoadCameraConfigurationFromDatabase();
}

void AMSampleCameraWizardSelector::onLoadSamplePlateConfigurationButtonCustomContextMenuRequested(QPoint point){
	QMenu popup(this);
	QAction *temp = popup.addAction("Load A Sample Plate Configuration from the Database");
	temp = popup.exec(loadSamplePlateButton_->mapToGlobal(point));

	// If a valid action was selected.
	if (temp)
		emit requestLoadSamplePlateConfigurationFromDatabase();
}

void AMSampleCameraWizardSelector::onLoadRotationConfigurationButtonCustomContextMenuRequested(QPoint point){
	QMenu popup(this);
	QAction *temp = popup.addAction("Load A Rotation Configuration from the Database");
	temp = popup.exec(loadRotationConfigurationButton_->mapToGlobal(point));

	// If a valid action was selected.
	if (temp)
		emit requestLoadRotationConfigurationFromDatabase();
}
