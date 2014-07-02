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


#ifndef AMWIZARDMANAGER_H
#define AMWIZARDMANAGER_H

#include <QObject>
#include <QVector3D>
#include <QPointF>

class AMCameraConfigurationWizard;
class AMGraphicsViewWizard;
class AMRotationWizard;
class AMSampleCameraGraphicsView;
class QGraphicsPolygonItem;

/**
 * @brief The AMWizardManager class is responsible for interacting with
 *	the wizards and the AMSampleCameraView.  Contains the wizards
 *	and a pointer to the AMSampleCameraView AMSampleCameraGraphicsView.
 */



class AMWizardManager : public QObject
{
	Q_OBJECT
public:
	enum WizardType {FULL, SIMPLE};
	explicit AMWizardManager(AMSampleCameraGraphicsView *view, QObject *parent = 0);
	
	/// Sample plate wizard and beam wizards have two different types, full or simple
	WizardType samplePlateWizardType();
	WizardType beamWizardType();

	const AMGraphicsViewWizard *cameraWizard();
	const AMGraphicsViewWizard *beamWizard();
	const AMGraphicsViewWizard *samplePlateWizard();
	const AMGraphicsViewWizard *rotationWizard();

signals:
	void cameraWizardFinished();
	void beamWizardFinished();
	void sampleWizardFinished();
	void simpleSampleWizardFinished();
	void rotationWizardFinished();

	/// Beam Wizard Signals
	void beamShape(int);
	void moveBeamSamplePlate(QVector3D);
	void moveBeamSamplePlate(QVector3D, double);
	void showBeamMarker(int);
	void createBeamShape(int);
	void moveBeamShape(QPointF, int);
	void mousePressed(QPointF, int);

	/// sample plate wizard signals
	void initializeSampleShape();
	void shiftSampleShape(QPointF);
	void sampleShapeMousePressed(QPointF);
	void showSamplePlate(bool);


	/// Motor coordination signals
	void transmitMotorMovementEnabled();
	void motorMovementEnabled(bool);
	void moveSucceeded();
	void transmitMotorRotation();
	void motorRotation(double);
	
public slots:
	void startCameraWizard();
	void startBeamWizard();
	void startSampleWizard();
	void startRotationWizard();

	/// set whether to use the full or simple versions of the beam and
	/// sample plate wizards.  By default, uses the SIMPLE versions
	void setBeamWizardType(WizardType type);
	void setSamplePlateWizardType(WizardType type);

	/// update the view of each wizard
	void cameraWizardUpdate();
	void beamWizardUpdate();
	void sampleWizardUpdate();
	void rotationWizardUpdate();

	void updateBeamWizardShape(QGraphicsPolygonItem *item);


protected slots:
	/// calls final configurations for each wizard then deletes it
	void cameraWizardDone();
	void beamWizardDone();
	void sampleWizardDone();
	void rotationWizardDone();


protected:
	/// Camera Configuration Wizard
	AMCameraConfigurationWizard* cameraWizard_;

	/// Beam Configuration Wizard
	AMGraphicsViewWizard *beamWizard_;
	WizardType beamWizardType_;

	/// Sample Plate Wizard
	AMGraphicsViewWizard* samplePlateWizard_;
	WizardType samplePlateWizardType_;

	/// Rotation Wizard
	AMRotationWizard* rotationWizard_;

	AMSampleCameraGraphicsView *parentView_;

	
};

#endif // AMWIZARDMANAGER_H
