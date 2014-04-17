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

class AMWizardManager : public QObject
{
	Q_OBJECT
public:
	enum WizardType {FULL, SIMPLE};
	explicit AMWizardManager(AMSampleCameraGraphicsView *view, QObject *parent = 0);
	
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

	/// sample wizard
	void initializeSampleShape();
	void shiftSampleShape(QPointF);
	void sampleShapeMousePressed(QPointF);
	void showSamplePlate(bool);


	/// Motor coordination
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

	void setBeamWizardType(WizardType type);
	void setSamplePlateWizardType(WizardType type);

	void cameraWizardUpdate();
	void beamWizardUpdate();
	void sampleWizardUpdate();
	void rotationWizardUpdate();

	void updateBeamWizardShape(QGraphicsPolygonItem *item);


protected slots:
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
