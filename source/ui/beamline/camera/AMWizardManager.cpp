#include "AMWizardManager.h"

#include "AMGraphicsViewWizard.h"
#include "AMCameraConfigurationWizard.h"
#include "AMRotationWizard.h"
#include "AMSamplePlateWizard.h"
#include "AMSimpleSamplePlateWizard.h"
#include "AMBeamConfigurationWizard.h"
#include "AMSimpleBeamConfigurationWizard.h"
#include "AMSampleCameraGraphicsView.h"
#include <QPointF>
#include <QVector3D>
#include <QGraphicsPolygonItem>
#include <QDebug>

AMWizardManager::AMWizardManager(AMSampleCameraGraphicsView *view, QObject *parent) :
	QObject(parent)
{
	parentView_ = view;
	setSamplePlateWizardType(SIMPLE);
	setBeamWizardType(SIMPLE);
	cameraWizard_ = 0;
	beamWizard_ = 0;
	samplePlateWizard_ = 0;
	rotationWizard_ = 0;
}

AMWizardManager::WizardType AMWizardManager::samplePlateWizardType()
{
	return samplePlateWizardType_;
}

AMWizardManager::WizardType AMWizardManager::beamWizardType()
{
	return beamWizardType_;
}

const AMGraphicsViewWizard *AMWizardManager::cameraWizard()
{
	const AMGraphicsViewWizard *cameraWizard;
	cameraWizard = cameraWizard_;
	return cameraWizard;
}

const AMGraphicsViewWizard *AMWizardManager::beamWizard()
{
	const AMGraphicsViewWizard *beamWizard;
	beamWizard = beamWizard_;
	return beamWizard;
}

const AMGraphicsViewWizard *AMWizardManager::samplePlateWizard()
{
	const AMGraphicsViewWizard *samplePlateWizard;
	samplePlateWizard = samplePlateWizard_;
	return samplePlateWizard;
}

const AMGraphicsViewWizard *AMWizardManager::rotationWizard()
{
	const AMGraphicsViewWizard *rotationWizard;
	rotationWizard = rotationWizard_;
	return rotationWizard;
}



void AMWizardManager::startCameraWizard()
{
	cameraWizard_ = new AMCameraConfigurationWizard();
	connect(cameraWizard_, SIGNAL(done(int)), this, SLOT(cameraWizardDone()));
	connect(cameraWizard_, SIGNAL(requestMotorMovementEnabled()), this, SIGNAL(transmitMotorMovementEnabled()));
	connect(cameraWizard_, SIGNAL(moveTo(QVector3D)), this, SIGNAL(moveBeamSamplePlate(QVector3D)));
	connect(this, SIGNAL(motorMovementEnabled(bool)), cameraWizard_, SLOT(setMotorMovementEnabled(bool)));
	connect(this,SIGNAL(moveSucceeded()), cameraWizard_, SLOT(testMoveSlot()));
	AMSampleCameraGraphicsView* view = new AMSampleCameraGraphicsView(0);
	view->setScene(parentView_->scene());
	view->setSceneRect(QRectF(QPointF(0,0),parentView_->size()));
	cameraWizard_->setView(view);
	cameraWizard_->show();
}

void AMWizardManager::startBeamWizard()
{
	if(beamWizardType() == FULL)
		beamWizard_ = new AMBeamConfigurationWizard();
	else if(beamWizardType() == SIMPLE)
		beamWizard_ = new AMSimpleBeamConfigurationWizard();
	connect(beamWizard_, SIGNAL(showShape(int)), this, SIGNAL(beamShape(int)));
	connect(beamWizard_, SIGNAL(done(int)), this, SLOT(beamWizardDone()));
	connect(beamWizard_, SIGNAL(moveTo(QVector3D)), this, SIGNAL(moveBeamSamplePlate(QVector3D)));
	connect(beamWizard_, SIGNAL(showBeamMarker(int)), this, SIGNAL(showBeamMarker(int)));
	connect(beamWizard_, SIGNAL(requestMotorMovementEnabled()), this, SIGNAL(transmitMotorMovementEnabled()));
	connect(beamWizard_, SIGNAL(createBeamShape(int)), this, SIGNAL(createBeamShape(int)));
	connect(beamWizard_, SIGNAL(moveBeamShape(QPointF, int)), this, SIGNAL(moveBeamShape(QPointF,int)));
	connect(beamWizard_, SIGNAL(mousePressed(QPointF,int)), this, SIGNAL(mousePressed(QPointF,int)));
	connect(this, SIGNAL(motorMovementEnabled(bool)), beamWizard_, SLOT(setMotorMovementEnabled(bool)));
	connect(this, SIGNAL(moveSucceeded()), beamWizard_, SIGNAL(moveSucceeded()));
	AMSampleCameraGraphicsView* view = new AMSampleCameraGraphicsView(0);
	view->setScene(parentView_->scene());
	view->setSceneRect(QRectF(QPointF(0,0),parentView_->size()));
	beamWizard_->setView(view);
	beamWizard_->show();
	emit showSamplePlate(true);
//	showSamplePlate_->setChecked(true);
}

void AMWizardManager::startSampleWizard()
{
	if(samplePlateWizardType() == FULL)
	{
		samplePlateWizard_ = new AMSamplePlateWizard();
		connect(samplePlateWizard_, SIGNAL(done(int)), this, SLOT(sampleWizardDone()));
	}
	else if(samplePlateWizardType() == SIMPLE)
	{
		samplePlateWizard_ = new AMSimpleSamplePlateWizard();
		connect(samplePlateWizard_, SIGNAL(done(int)), this, SLOT(sampleWizardDone()));
	}

	AMSampleCameraGraphicsView* view = new AMSampleCameraGraphicsView();
	connect(samplePlateWizard_, SIGNAL(requestMotorMovementEnabled()), this, SIGNAL(transmitMotorMovementEnabled()));
	connect(samplePlateWizard_, SIGNAL(moveTo(QVector3D,double)), this, SIGNAL(moveBeamSamplePlate(QVector3D,double)));
	connect(this, SIGNAL(motorMovementEnabled(bool)), samplePlateWizard_, SLOT(setMotorMovementEnabled(bool)));
	connect(this, SIGNAL(moveSucceeded()), samplePlateWizard_, SIGNAL(moveSucceeded()));
	connect(samplePlateWizard_, SIGNAL(requestRotation()), this, SIGNAL(transmitMotorRotation()));
	connect(this, SIGNAL(motorRotation(double)), samplePlateWizard_, SLOT(setCurrentRotation(double)));
	connect(samplePlateWizard_, SIGNAL(signalInitializeSampleShape()), this, SIGNAL(initializeSampleShape()));
	connect(samplePlateWizard_, SIGNAL(signalShiftSampleShape(QPointF)), this, SIGNAL(shiftSampleShape(QPointF)));
	connect(samplePlateWizard_, SIGNAL(signalSamplePlateWizardMousePressed(QPointF)), this, SIGNAL(sampleShapeMousePressed(QPointF)));
	view->setScene(parentView_->scene());
	view->setSceneRect(QRectF(QPointF(0,0),parentView_->size()));
	samplePlateWizard_->setView(view);
	samplePlateWizard_->show();
	emit showSamplePlate(true);
}

void AMWizardManager::startRotationWizard()
{
	rotationWizard_ = new AMRotationWizard();
	AMSampleCameraGraphicsView* view = new AMSampleCameraGraphicsView();
	/// \todo other things here
	connect(rotationWizard_, SIGNAL(done(int)), this, SLOT(rotationWizardDone()));
	connect(rotationWizard_, SIGNAL(requestMotorMovementEnabled()), this, SIGNAL(transmitMotorMovementEnabled()));
	connect(rotationWizard_, SIGNAL(moveTo(QVector3D,double)), this, SIGNAL(moveBeamSamplePlate(QVector3D,double)));
	connect(this, SIGNAL(motorMovementEnabled(bool)), rotationWizard_, SLOT(setMotorMovementEnabled(bool)));
	connect(this, SIGNAL(moveSucceeded()), rotationWizard_, SIGNAL(moveSucceeded()));
	view->setScene(parentView_->scene());
	view->setSceneRect(QRectF(QPointF(0,0),parentView_->size()));
	rotationWizard_->setView(view);
	rotationWizard_->show();

}


void AMWizardManager::setBeamWizardType(AMWizardManager::WizardType type)
{
	beamWizardType_ = type;
}

void AMWizardManager::setSamplePlateWizardType(AMWizardManager::WizardType type)
{
	samplePlateWizardType_ = type;
}

void AMWizardManager::cameraWizardUpdate()
{
	if(cameraWizard_)
	{
		AMSampleCameraGraphicsView *view = new AMSampleCameraGraphicsView();
		view->setScene(parentView_->scene());
		cameraWizard_->updateScene(view);
	}
}

void AMWizardManager::beamWizardUpdate()
{
	if(beamWizard_)
	{
		AMSampleCameraGraphicsView *view = new AMSampleCameraGraphicsView();
		view->setScene(parentView_->scene());
		beamWizard_->updateScene(view);
	}
}

void AMWizardManager::sampleWizardUpdate()
{
	if(samplePlateWizard_)
	{
		AMSampleCameraGraphicsView *view = new AMSampleCameraGraphicsView();
		view->setScene(parentView_->scene());
		samplePlateWizard_->updateScene(view);
	}
}

void AMWizardManager::rotationWizardUpdate()
{
	if(rotationWizard_)
	{
		AMSampleCameraGraphicsView *view = new AMSampleCameraGraphicsView();
		view->setScene(parentView_->scene());
		rotationWizard_->updateScene(view);
	}
}

void AMWizardManager::updateBeamWizardShape(QGraphicsPolygonItem *item)
{
	qDebug()<<"Here in AMWizardManager::updateBeamWizardShape";
	beamWizard_->updateShape(item);
	qDebug()<<"Leaving AMWizardManager::updateBeamWizardShape";
}

void AMWizardManager::cameraWizardDone()
{
	emit cameraWizardFinished();
	delete cameraWizard_;
	cameraWizard_ = 0;
}

void AMWizardManager::beamWizardDone()
{
	qDebug()<<"Beam Wizard done";
	emit beamWizardFinished();
	delete beamWizard_;
	beamWizard_ = 0;
}

void AMWizardManager::sampleWizardDone()
{
	if(samplePlateWizardType() == FULL)
		emit sampleWizardFinished();
	else if(samplePlateWizardType() == SIMPLE)
		emit simpleSampleWizardFinished();
	delete samplePlateWizard_;
	samplePlateWizard_ = 0;
}

void AMWizardManager::rotationWizardDone()
{
	emit rotationWizardFinished();
	delete rotationWizard_;
	rotationWizard_ = 0;
}
