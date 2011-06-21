#ifndef SGMELEMENTINFO_H
#define SGMELEMENTINFO_H

#include <QObject>
#include "util/AMElement.h"
#include "util/AMOrderedSet.h"

class SGMFastScanParameters;
//class SGMEnergyPosition;
class SGMEnergyPosition
{
public:
	SGMEnergyPosition(double energy = -1, int monoEncoderTarget = -1, int undulatorStepSetpoint = -1, double exitSlitDistance = -1);

	double energy() const;
	int monoEncoderTarget() const;
	int undulatorStepSetpoint() const;
	double exitSlitDistance() const;

protected:
	double energy_;
	int monoEncoderTarget_;
	int undulatorStepSetpoint_;
	double exitSlitDistance_;
};



class SGMElementInfo : public QObject
{
Q_OBJECT
public:
	SGMElementInfo(AMElement *element, const SGMEnergyPosition &standardStart, const SGMEnergyPosition &standardMiddle, const SGMEnergyPosition &standardEnd, QObject *parent = 0);

	AMElement* element() const;
	const SGMEnergyPosition& standardStart() const;
	const SGMEnergyPosition& standardMiddle() const;
	const SGMEnergyPosition& standardEnd() const;
	AMOrderedSet<int, SGMFastScanParameters*> availableFastScanParameters() const;

public slots:
	bool addFastScanParameters(SGMFastScanParameters *parameters);

protected:
	AMElement *element_;
	SGMEnergyPosition standardStart_;
	SGMEnergyPosition standardMiddle_;
	SGMEnergyPosition standardEnd_;
	AMOrderedSet<int, SGMFastScanParameters*> availableFastScanParameters_;
};

class SGMFastScanParameters : public QObject
{
	Q_OBJECT
public:
	SGMFastScanParameters(QObject *parent = 0);
	SGMFastScanParameters(const QString &element, double runSeconds, double energyStart, double energyMidpoint, double energyEnd, int velocity, int velocityBase, int acceleration, double scalerTime, int baseLine, int undulatorStartStep, int undulatorVelocity, int undulatorRelativeStep, QObject *parent = 0);

	bool operator==(const SGMFastScanParameters &other);

	QString element() const { return element_;}
	double runSeconds() const { return runSeconds_;}
	double energyStart() const { return energyStart_;}
	double energyMidpoint() const { return energyMidpoint_;}
	double energyEnd() const { return energyEnd_;}
	int velocity() const { return velocity_;}
	int velocityBase() const { return velocityBase_;}
	int acceleration() const { return acceleration_;}
	double scalerTime() const { return scalerTime_;}
	int baseLine() const { return baseLine_;}

	int undulatorStartStep() const { return undulatorStartStep_;}
	int undulatorVelocity() const { return undulatorVelocity_;}
	int undulatorRelativeStep() const { return undulatorRelativeStep_;}

public slots:
	void setElement(const QString &element) { element_ = element;}
	void setRunSeconds(double runSeconds) { runSeconds_ = runSeconds;}
	void setEnergyStart(double energyStart) { energyStart_ = energyStart;}
	void setEnergyMidpoint(double energyMidpoint) { energyMidpoint_ = energyMidpoint;}
	void setEnergyEnd(double energyEnd) { energyEnd_ = energyEnd;}
	void setVelocity(int velocity) { velocity_ = velocity;}
	void setVelocityBase(int velocityBase) { velocityBase_ = velocityBase;}
	void setAcceleration(int acceleration) { acceleration_ = acceleration;}
	void setScalerTime(double scalerTime) { scalerTime_ = scalerTime;}
	void setBaseLine(int baseLine) { baseLine_ = baseLine;}

	void setUndulatorStartStep(int undulatorStartStep) { undulatorStartStep_ = undulatorStartStep;}
	void setUndulatorVelocity(int undulatorVelocity) { undulatorVelocity_ = undulatorVelocity;}
	void setUndulatorRelativeStep(int undulatorRelativeStep) { undulatorRelativeStep_ = undulatorRelativeStep;}

protected:
	QString element_;
	double runSeconds_;
	double energyStart_;
	double energyMidpoint_;
	double energyEnd_;
	int velocity_;
	int velocityBase_;
	int acceleration_;
	double scalerTime_;
	int baseLine_;

	int undulatorStartStep_;
	int undulatorVelocity_;
	int undulatorRelativeStep_;
};

#endif // SGMELEMENTINFO_H
