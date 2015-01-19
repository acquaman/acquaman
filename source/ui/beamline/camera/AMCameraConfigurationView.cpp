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


#include "AMCameraConfigurationView.h"

#include <QBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QVector3D>
#include <QSlider>

#include "dataman/database/AMDbObjectSupport.h"
#include "ui/beamline/camera/AMBeamConfigurationView.h"
#include "beamline/camera/AMCameraConfiguration.h"
#include "util/AMErrorMonitor.h"

AMCameraConfigurationView::AMCameraConfigurationView(AMCameraConfiguration *cameraConfiguration, QWidget *parent) :
	QWidget(parent)
{
	if(cameraConfiguration == 0)
		AMErrorMon::alert(this, AMCAMERACONFIGURATIONVIEW_NULL_CONFIGURATION_AS_ARGUMENT, QString("AMCameraConfigurationView received a null object as the configuration.") );
	else
		cameraModel_ = cameraConfiguration;

	AMDatabase *dbSGM = AMDatabase::database("SGMPublic");
//	QList<int> matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMCameraConfiguration>(), "name", "defaultConfiguration");
	QVariantList matchIDs = dbSGM->retrieve(AMDbObjectSupport::s()->tableNameForClass<AMCameraConfiguration>(),"id");
	if(matchIDs.count() == 0)
	{
		cameraModel_->setName("defaultConfiguration");
		cameraModel_->setCameraDistortion(-0.09);
		cameraModel_->setCameraFocalLength(0.41);
		cameraModel_->setCameraFOV(0.387);
		cameraModel_->setCameraCentre(QVector3D(0,0,0));
		cameraModel_->setCameraPosition(QVector3D(0,0,1));
		cameraModel_->setCameraRotation(0);
		cameraModel_->setPixelAspectRatio(1);
		bool success = cameraModel_->storeToDb(dbSGM);

		if(!success)
			AMErrorMon::alert(this, AMCAMERACONFIGURATIONVIEW_SAVE_CONFIGURATION_FAILED_IN_CONSTRUCTOR, QString("AMCameraConfiguration failed to save its configuration in the constructor.") );
	}
	else{
		//cameraModel_->loadFromDb(dbSGM,matchIDs.first());
		cameraModel_->loadFromDb(dbSGM,matchIDs.last().toInt());
	}

	cameraRotationCentre_ = cameraModel_->cameraRotation();

	setWindowTitle("Configuration");

	//GUI SETUP
	QVBoxLayout* vbl = new QVBoxLayout();
	vbl->setSpacing(0);
	vbl->setContentsMargins(0,0,0,0);

	QFrame* positionFrame = new QFrame();
	QHBoxLayout* phl = new QHBoxLayout();
	phl->setContentsMargins(12,4,12,4);
	phl->addWidget(new QLabel("Camera Position"));
	phl->addSpacing(20);
	phl->addWidget(new QLabel("x:"));
	phl->addWidget(positionX_ = new QLineEdit());
	phl->addSpacing(20);
	phl->addWidget(new QLabel("y:"));
	phl->addWidget(positionY_ = new QLineEdit());
	phl->addSpacing(20);
	phl->addWidget(new QLabel("z:"));
	phl->addWidget(positionZ_ = new QLineEdit());
	phl->addSpacing(20);
	phl->addWidget(new QLabel("Offset x:"));
	phl->addWidget(centreOffsetX_ = new QLineEdit());
	phl->addStretch();
	positionFrame->setLayout(phl);

	QFrame* centerFrame = new QFrame();
	QHBoxLayout* chl = new QHBoxLayout();
	chl->setContentsMargins(12,4,12,4);
	chl->addWidget(new QLabel("Camera Center  "));
	chl->addSpacing(20);
	chl->addWidget(new QLabel("x:"));
	chl->addWidget(centerX_ = new QLineEdit());
	chl->addSpacing(20);
	chl->addWidget(new QLabel("y:"));
	chl->addWidget(centerY_ = new QLineEdit());
	chl->addSpacing(20);
	chl->addWidget(new QLabel("z:"));
	chl->addWidget(centerZ_ = new QLineEdit());
	phl->addSpacing(20);
	phl->addWidget(new QLabel("Offset y:"));
	phl->addWidget(centreOffsetY_ = new QLineEdit());
	chl->addStretch();
	centerFrame->setLayout(chl);

	QFrame* fovFrame = new QFrame();
	QHBoxLayout* fhl = new QHBoxLayout();
	fhl->setContentsMargins(12,4,12,4);
	fhl->addWidget(new QLabel("Camera Field of View"));
	fhl->addWidget(cameraFOV_ = new QLineEdit());
	fhl->addSpacing(20);
	fhl->addWidget(new QLabel("Camera Focal Length"));
	fhl->addWidget(cameraFocalLength_[CAMERAFRAME] = new QLineEdit());
	fhl->addSpacing(20);
	fhl->addWidget(new QLabel("Lens Distortion"));
	fhl->addWidget(cameraDistortion_[CAMERAFRAME] = new QLineEdit());
	fhl->addSpacing(20);
	fhl->addWidget(new QLabel("Pixel Aspect Ratio"));
	fhl->addWidget(pixelAspectRatio_[CAMERAFRAME] = new QLineEdit());
	fhl->addSpacing(20);
	fhl->addWidget(setButton_ = new QPushButton("Set"));
	fhl->addStretch();
	fovFrame->setLayout(fhl);

	QFrame* rotationFrame = new QFrame();
	QHBoxLayout* rhl = new QHBoxLayout();
	rhl->setContentsMargins(12,4,12,4);
	rhl->addWidget(new QLabel("Camera Rotation"));
	rhl->addWidget(cameraRotation_ = new QLineEdit());
	rhl->addWidget(cameraRotationSlider_ = new QSlider(Qt::Horizontal));
	rhl->addStretch();
	rotationFrame->setLayout(rhl);

	cameraRotationSlider_->setMaximumWidth(80);
	cameraRotationSlider_->setRange(0,80);
	cameraRotationSlider_->setValue(40);

	QFrame* selectionFrame = new QFrame();
	QHBoxLayout* shl = new QHBoxLayout();
	shl->setContentsMargins(12,4,12,4);
	shl->addWidget(configurationSelection_ = new QComboBox());
	shl->addSpacing(20);
	shl->addWidget(configurationName_ = new QLineEdit());
	shl->addSpacing(20);
	shl->addWidget(saveConfiguration_ = new QPushButton("Save Configuration"));
	shl->addSpacing(20);
	shl->addWidget(overwriteButton_ = new QPushButton("Overwrite configuration"));
	shl->addStretch();
	selectionFrame->setLayout(shl);


	cameraFrame_= new QFrame();
	QVBoxLayout* cvl  = new QVBoxLayout();
	cvl->setContentsMargins(0,0,0,0);
	cvl->addWidget(positionFrame);
	cvl->addWidget(centerFrame);
	cvl->addWidget(fovFrame);
	cvl->addWidget(rotationFrame);
	cvl->addStretch();
	cameraFrame_->setLayout(cvl);

	matrixFrame_ = new QFrame();
	QVBoxLayout* mvl = new QVBoxLayout();
	mvl->setContentsMargins(0,0,0,0);

	QFrame* rowFrame[3];
	QHBoxLayout* rfhl[3];
	for(int i = 0; i < 3; i++)
	{
		rowFrame[i] = new QFrame();
		rfhl[i] = new QHBoxLayout();
		rfhl[i]->setContentsMargins(12,4,12,4);
		for(int j = 0; j < 4; j++)
		{
			rfhl[i]->addWidget(matrixElement_[4*i+j] = new QLineEdit());
			rfhl[i]->addSpacing(20);
		}
		rfhl[i]->addStretch();
		rowFrame[i]->setLayout(rfhl[i]);
		mvl->addWidget(rowFrame[i]);
	}

	QFrame* otherParams = new QFrame();
	QHBoxLayout* otherParamsLayout = new QHBoxLayout();
	otherParamsLayout->setContentsMargins(12,4,12,4);
	otherParamsLayout->addWidget(new QLabel("Focal Length"));
	otherParamsLayout->addWidget(cameraFocalLength_[MATRIXFRAME] = new QLineEdit());
	otherParamsLayout->addSpacing(20);
	otherParamsLayout->addWidget(new QLabel("Distortion"));
	otherParamsLayout->addWidget(cameraDistortion_[MATRIXFRAME] = new QLineEdit());
	otherParamsLayout->addSpacing(20);
	otherParamsLayout->addWidget(new QLabel("Pixel Aspect Ratio"));
	otherParamsLayout->addWidget(pixelAspectRatio_[MATRIXFRAME] = new QLineEdit());
	otherParamsLayout->addStretch();
	otherParams->setLayout(otherParamsLayout);


	mvl->addWidget(otherParams);
	mvl->addStretch();
	matrixFrame_->setLayout(mvl);

	vbl->addWidget(cameraFrame_);
	vbl->addWidget(matrixFrame_);
	vbl->addWidget(selectionFrame);
	setLayout(vbl);
	matrixFrame_->hide();

	populateComboBox(1);



	/// connections

	connect(positionX_, SIGNAL(textChanged(QString)), this, SLOT(updatePositionX(QString)));
	connect(positionY_, SIGNAL(textChanged(QString)), this, SLOT(updatePositionY(QString)));
	connect(positionZ_, SIGNAL(textChanged(QString)), this, SLOT(updatePositionZ(QString)));
	connect(centerX_, SIGNAL(textChanged(QString)), this, SLOT(updateCenterX(QString)));
	connect(centerY_, SIGNAL(textChanged(QString)), this, SLOT(updateCenterY(QString)));
	connect(centerZ_, SIGNAL(textChanged(QString)), this, SLOT(updateCenterZ(QString)));
	connect(cameraFOV_, SIGNAL(textChanged(QString)), this, SLOT(updateFOV(QString)));
	connect(cameraFocalLength_[CAMERAFRAME], SIGNAL(textChanged(QString)), this, SLOT(updateFocalLength(QString)));
	connect(cameraFocalLength_[MATRIXFRAME], SIGNAL(textChanged(QString)), this, SLOT(updateFocalLength(QString)));
	connect(cameraDistortion_[CAMERAFRAME], SIGNAL(textChanged(QString)), this, SLOT(updateDistortion(QString)));
	connect(cameraDistortion_[MATRIXFRAME], SIGNAL(textChanged(QString)), this, SLOT(updateDistortion(QString)));
	connect(setButton_, SIGNAL(clicked()), this, SLOT(updateAll()));


	connect(cameraRotation_, SIGNAL(textChanged(QString)), this, SLOT(updateRotation(QString)));
	connect(cameraRotationSlider_, SIGNAL(valueChanged(int)), this, SLOT(updateRotationSlider(int)));

	connect(pixelAspectRatio_[CAMERAFRAME], SIGNAL(textChanged(QString)), this, SLOT(updatePixelAspectRatio(QString)));
	connect(pixelAspectRatio_[MATRIXFRAME], SIGNAL(textChanged(QString)), this, SLOT(updatePixelAspectRatio(QString)));

	connect(centreOffsetX_, SIGNAL(textChanged(QString)), this, SLOT(updateCentreOffsetX(QString)));
	connect(centreOffsetY_, SIGNAL(textChanged(QString)), this, SLOT(updateCentreOffsetY(QString)));


	connect(saveConfiguration_, SIGNAL(clicked()), this, SLOT(saveConfiguration()));
	connect(configurationName_, SIGNAL(textChanged(QString)), this, SLOT(updateName(QString)));
	connect(configurationSelection_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSelection(int)));
	connect(overwriteButton_, SIGNAL(clicked()), this, SLOT(overwriteConfiguration()));

	for(int i = 0; i < 12; i++)
	{
		connect(matrixElement_[i], SIGNAL(textChanged(QString)), this, SLOT(updateMatrix()));
	}


}


double AMCameraConfigurationView::positionX()
{
	return cameraModel_->cameraPositionX();
}

double AMCameraConfigurationView::positionY()
{
	return cameraModel_->cameraPositionY();
}

double AMCameraConfigurationView::positionZ()
{
	return cameraModel_->cameraPositionZ();
}

double AMCameraConfigurationView::centerX()
{
	return cameraModel_->cameraCentreX();
}

double AMCameraConfigurationView::centerY()
{
	return cameraModel_->cameraCentreY();
}

double AMCameraConfigurationView::centerZ()
{
	return cameraModel_->cameraCentreZ();
}

double AMCameraConfigurationView::cameraFOV()
{
	return cameraModel_->cameraFOV();
}

double AMCameraConfigurationView::cameraFocalLength()
{
	return cameraModel_->cameraFocalLength();

}

double AMCameraConfigurationView::cameraDistortion()
{
	return cameraModel_->cameraDistortion();
}

double AMCameraConfigurationView::cameraRotation()
{
	return cameraModel_->cameraRotation();
}

double AMCameraConfigurationView::pixelAspectRatio()
{
	return cameraModel_->pixelAspectRatio();
}

double AMCameraConfigurationView::centreOffsetX()
{
	return cameraModel_->imageCentre().x();
}

double AMCameraConfigurationView::centreOffsetY()
{
	return cameraModel_->imageCentre().y();
}

void AMCameraConfigurationView::setPositionX(double x)
{
	cameraModel_->setCameraPositionX(x);
}

void AMCameraConfigurationView::setPositionY(double y)
{
	cameraModel_->setCameraPositionY(y);
}

void AMCameraConfigurationView::setPositionZ(double z)
{
	cameraModel_->setCameraPositionZ(z);
}

void AMCameraConfigurationView::setCenterX(double x)
{
	cameraModel_->setCameraCentreX(x);
}

void AMCameraConfigurationView::setCenterY(double y)
{
	cameraModel_->setCameraCentreY(y);
}

void AMCameraConfigurationView::setCenterZ(double z)
{
	cameraModel_->setCameraCentreZ(z);
}

void AMCameraConfigurationView::setCameraFOV(double fov)
{
	cameraModel_->setCameraFOV(fov);
}

void AMCameraConfigurationView::setCameraFocalLength(double focalLength)
{
	cameraModel_->setCameraFocalLength(focalLength);
}

void AMCameraConfigurationView::setCameraDistortion(double distortion)
{
	cameraModel_->setCameraDistortion(distortion);
}

void AMCameraConfigurationView::setCameraRotation(double rotation)
{
	cameraModel_->setCameraRotation(rotation);
	cameraRotationCentre_ = rotation;
	bool block = true;
	block = cameraRotationSlider_->blockSignals(block);
	cameraRotationSlider_->setValue(cameraRotationSlider_->maximum()/2);
	cameraRotationSlider_->blockSignals(block);
}

void AMCameraConfigurationView::setPixelAspectRatio(double pixelAspectRatio)
{
	cameraModel_->setPixelAspectRatio(pixelAspectRatio);
}

void AMCameraConfigurationView::setCentreOffsetX(double centreOffsetX)
{
	cameraModel_->setImageCentreX(centreOffsetX);
}

void AMCameraConfigurationView::setCentreOffsetY(double centreOffsetY)
{
	cameraModel_->setImageCentreY(centreOffsetY);
}

void AMCameraConfigurationView::setCameraConfiguration(AMCameraConfiguration *newCameraConfiguration)
{
	cameraModel_ = newCameraConfiguration;
}

void AMCameraConfigurationView::updatePositionX(QString x)
{
	bool* conversionOK = new bool(false);
	double newValue = x.toDouble(conversionOK);
	if(*conversionOK) setPositionX(newValue);
}

void AMCameraConfigurationView::updatePositionY(QString y)
{
	bool* conversionOK = new bool(false);
	double newValue = y.toDouble(conversionOK);
	if(*conversionOK) setPositionY(newValue);
}

void AMCameraConfigurationView::updatePositionZ(QString z)
{
	bool* conversionOK = new bool(false);
	double newValue = z.toDouble(conversionOK);
	if(*conversionOK) setPositionZ(newValue);
}

void AMCameraConfigurationView::updateCenterX(QString x)
{
	bool* conversionOK = new bool(false);
	double newValue = x.toDouble(conversionOK);
	if(*conversionOK) setCenterX(newValue);
}

void AMCameraConfigurationView::updateCenterY(QString y)
{
	bool* conversionOK = new bool(false);
	double newValue = y.toDouble(conversionOK);
	if(*conversionOK) setCenterY(newValue);
}

void AMCameraConfigurationView::updateCenterZ(QString z)
{
	bool* conversionOK = new bool(false);
	double newValue = z.toDouble(conversionOK);
	if(*conversionOK) setCenterZ(newValue);
}

void AMCameraConfigurationView::updateFOV(QString fov)
{
	bool* conversionOK = new bool(false);
	double newValue = fov.toDouble(conversionOK);
	if(*conversionOK) setCameraFOV(newValue);
}

void AMCameraConfigurationView::updateFocalLength(QString focalLength)
{
	bool* conversionOK = new bool(false);
	double newValue = focalLength.toDouble(conversionOK);
	if(*conversionOK) setCameraFocalLength(newValue);
}

void AMCameraConfigurationView::updateDistortion(QString distortion)
{
	bool* conversionOK = new bool(false);
	double newValue = distortion.toDouble(conversionOK);
	if(*conversionOK) setCameraDistortion(newValue);
}

void AMCameraConfigurationView::updateRotation(QString rotation)
{
	bool* conversionOK = new bool(false);
	double newValue = rotation.toDouble(conversionOK);
	if(*conversionOK) setCameraRotation(newValue);
}

void AMCameraConfigurationView::updateRotationSlider(int rotation)
{
	double range = 0.25;
	double value = (double)rotation;
	double max = (double)cameraRotationSlider_->maximum();
	double fraction = value/max;
	cameraModel_->setCameraRotation(fraction*2*range +(cameraRotationCentre_-range));
	bool block = cameraRotation_->blockSignals(true);
	cameraRotation_->setText(QString::number(cameraRotation()));
	cameraRotation_->blockSignals(block);
	emit(update(cameraModel_));

}

void AMCameraConfigurationView::updatePixelAspectRatio(QString pixelAspectRatio)
{
	bool* conversionOK = new bool(false);
	double newValue = pixelAspectRatio.toDouble(conversionOK);
	if(*conversionOK) setPixelAspectRatio(newValue);
}

void AMCameraConfigurationView::updateCentreOffsetX(QString centreOffsetX)
{
	bool* conversionOK = new bool(false);
	double newValue = centreOffsetX.toDouble(conversionOK);
	if(*conversionOK) setCentreOffsetX(newValue);
}

void AMCameraConfigurationView::updateCentreOffsetY(QString centreOffsetY)
{
	bool* conversionOK = new bool(false);
	double newValue = centreOffsetY.toDouble(conversionOK);
	if(*conversionOK) setCentreOffsetY(newValue);
}

void AMCameraConfigurationView::updateName(QString name)
{
	cameraModel_->setName(name);
}

void AMCameraConfigurationView::updateSelection(int id)
{
	AMDatabase *dataBase = AMDatabase::database("SGMPublic");
	cameraModel_->loadFromDb(dataBase,id+1);
	updateAll();
}

/// Saves the current configuration as a new database item
/// must have a unique name
void AMCameraConfigurationView::saveConfiguration()
{
	AMDatabase *dataBase = AMDatabase::database("SGMPublic");
	QList<int> matchIDs = dataBase->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMCameraConfiguration>(), "name", cameraModel_->name());
	if(0 == matchIDs.count())
	{
		cameraModel_->dissociateFromDb();
		cameraModel_->storeToDb(dataBase);
		populateComboBox(cameraModel_->id());
	}
	else
		AMErrorMon::alert(this, AMCAMERACONFIGURATIONVIEW_SAVE_CONFIGURATION_NAME_CONFLICT, QString("Cannot save the camera configuration because the name (%1) is already taken.").arg(cameraModel_->name()) );
}

/// overwrites the values of the current configuration
void AMCameraConfigurationView::overwriteConfiguration()
{
	AMDatabase *dataBase = AMDatabase::database("SGMPublic");
	cameraModel_->storeToDb(dataBase);
	populateComboBox(cameraModel_->id());
}

void AMCameraConfigurationView::updateMatrix()
{
	QVector<QVector3D> cameraModel;
	QVector3D columns[4];
	for(int i = 0; i < 4; i++)
	{
		columns[i] = QVector3D(matrixElement_[i]->text().toDouble(),matrixElement_[4+i]->text().toDouble(),matrixElement_[8+i]->text().toDouble());
		cameraModel<<columns[i];
	}
	cameraModel_->setCameraMatrix(cameraModel);

}

/// looks up currently saved configurations, and shows them in the combo box
void AMCameraConfigurationView::populateComboBox(int dbIndex)
{
	configurationSelection_->clear();
	// populate the selection box
	AMDatabase *database = AMDatabase::database("SGMPublic");
	QList<QVariant> nameList = database->retrieve(AMDbObjectSupport::s()->tableNameForClass<AMCameraConfiguration>(),"name");

	QList<QVariant>::iterator i;
	for(i = nameList.begin(); i != nameList.end(); i++)
	{
		configurationSelection_->addItem((*i).toString());
	}
	configurationSelection_->setCurrentIndex(dbIndex-1);
}

/// updates all the fields in the window
void AMCameraConfigurationView::updateAll()
{
	QVector3D position = cameraModel_->cameraPosition();
	QVector3D center = cameraModel_->cameraCentre();
	QString name = cameraModel_->name();
	emit update(cameraModel_);
	positionX_->setText(QString::number(position.x()));
	positionY_->setText(QString::number(position.y()));
	positionZ_->setText(QString::number(position.z()));
	centerX_->setText(QString::number(center.x()));
	centerY_->setText(QString::number(center.y()));
	centerZ_->setText(QString::number(center.z()));
	cameraFOV_->setText(QString::number(cameraFOV()));
	cameraFocalLength_[CAMERAFRAME]->setText(QString::number(cameraFocalLength()));
	cameraFocalLength_[MATRIXFRAME]->setText(QString::number(cameraFocalLength()));
	cameraDistortion_[CAMERAFRAME]->setText(QString::number(cameraDistortion()));
	cameraDistortion_[MATRIXFRAME]->setText(QString::number(cameraDistortion()));
	configurationName_->setText(name);
	cameraRotation_->setText(QString::number(cameraRotation()));
	pixelAspectRatio_[CAMERAFRAME]->setText(QString::number(pixelAspectRatio()));
	pixelAspectRatio_[MATRIXFRAME]->setText(QString::number(pixelAspectRatio()));
	centreOffsetX_->setText(QString::number(centreOffsetX()));
	centreOffsetY_->setText(QString::number(centreOffsetY()));
	QVector<QVector3D> cameraMatrix;
	cameraMatrix = cameraModel_->cameraMatrix();
	if(cameraMatrix.isEmpty())
	{
		for(int i = 0; i < 4; i++)
		{
			cameraMatrix<<QVector3D(0,0,0);
		}
	}
	for(int i = 0; i < 4; i++)
	{
		matrixElement_[i]->setText(QString("%1").arg(cameraMatrix.at(i).x()));
		matrixElement_[4+i]->setText(QString("%1").arg(cameraMatrix.at(i).y()));
		matrixElement_[8+i]->setText(QString("%1").arg(cameraMatrix.at(i).z()));
	}
}

void AMCameraConfigurationView::hideCameraParameters(bool hide)
{

	if(hide)
	{
		cameraFrame_->hide();
		matrixFrame_->show();
	}
	else
	{
		matrixFrame_->hide();
		cameraFrame_->show();
	}

}

void AMCameraConfigurationView::onCameraConfigurationChanged(AMCameraConfiguration* cameraConfiguration)
{
	setCameraConfiguration(cameraConfiguration);
	updateAll();
}



