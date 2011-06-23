#ifndef SGMELEMENTINFO_H
#define SGMELEMENTINFO_H

#include <QObject>
#include <QDebug>
#include "util/AMElement.h"
#include "util/AMOrderedSet.h"

class SGMFastScanParameters;
//class SGMEnergyPosition;
class SGMEnergyPosition
{
public:
	SGMEnergyPosition(double energy = -1, int monoEncoderTarget = -1, int undulatorStepSetpoint = -1, double exitSlitDistance = -1);

	double energy() const { return energy_;}
	int monoEncoderTarget() const { return monoEncoderTarget_;}
	int undulatorStepSetpoint() const { return undulatorStepSetpoint_;}
	double exitSlitDistance() const { return exitSlitDistance_;}

	SGMEnergyPosition& operator=(const SGMEnergyPosition& other);
	friend QDebug operator<<(QDebug d, const SGMEnergyPosition &energyPosition);

protected:
	double energy_;
	int monoEncoderTarget_;
	int undulatorStepSetpoint_;
	double exitSlitDistance_;
};

class SGMScanRangeInfo
{
public:
	SGMScanRangeInfo(const SGMEnergyPosition &start, const SGMEnergyPosition &middle, const SGMEnergyPosition &end);
	SGMScanRangeInfo(const SGMScanRangeInfo &other);

	SGMEnergyPosition start() const { return start_;}
	SGMEnergyPosition middle() const { return middle_;}
	SGMEnergyPosition end() const { return end_;}

	friend QDebug operator<<(QDebug d, const SGMScanRangeInfo &scanRangeInfo);

protected:
	SGMEnergyPosition start_;
	SGMEnergyPosition middle_;
	SGMEnergyPosition end_;
};


class SGMEdgeInfo
{
public:
	SGMEdgeInfo(const QString &edge, double energy, const SGMScanRangeInfo &standardRange);
	SGMEdgeInfo(QPair<QString, QString> edgeAndEnergy, const SGMScanRangeInfo &standardRange);

	QString edge() const { return edge_;}
	double energy() const { return energy_;}
	SGMScanRangeInfo standardRange() const { return standardRange_;}
	SGMEnergyPosition standardStart() const { return standardRange_.start();}
	SGMEnergyPosition standardMiddle() const { return standardRange_.middle();}
	SGMEnergyPosition standardEnd() const { return standardRange_.end();}

	friend QDebug operator<<(QDebug d, const SGMEdgeInfo &edgeInfo);

protected:
	QString edge_;
	double energy_;
	SGMScanRangeInfo standardRange_;
};

class SGMStandardScanInfo
{
public:
	SGMStandardScanInfo(const QString &name, const SGMScanRangeInfo &standardRange);

	QString name() const { return name_;}
	SGMScanRangeInfo standardRange() const { return standardRange_;}
	SGMEnergyPosition standardStart() const { return standardRange_.start();}
	SGMEnergyPosition standardMiddle() const { return standardRange_.middle();}
	SGMEnergyPosition standardEnd() const { return standardRange_.end();}

	friend QDebug operator<<(QDebug d, const SGMStandardScanInfo &standardScanInfo);

protected:
	QString name_;
	SGMScanRangeInfo standardRange_;
};


class SGMElementInfo : public QObject
{
Q_OBJECT
public:
	SGMElementInfo(AMElement *element, QObject *parent = 0);

	AMElement* element() const;

	AMOrderedSet<QString, SGMEdgeInfo> sgmEdgeInfos() const;
	AMOrderedSet<QString, SGMStandardScanInfo> sgmStandardScanInfos() const;
	AMOrderedSet<int, SGMFastScanParameters*> availableFastScanParameters() const;

public slots:
	bool addEdgeInfo(const SGMEdgeInfo &edgeInfo);
	bool addStandardScanInfo(const SGMStandardScanInfo &standardScanInfo);
	bool addFastScanParameters(SGMFastScanParameters *parameters);

protected:
	AMElement *element_;
	AMOrderedSet<QString, SGMEdgeInfo> sgmEdgeInfos_;
	AMOrderedSet<QString, SGMStandardScanInfo> sgmStandardScanInfos_;

	AMOrderedSet<int, SGMFastScanParameters*> availableFastScanParameters_;
};

class SGMFastScanParameters : public QObject
{
	Q_OBJECT
public:
	SGMFastScanParameters(QObject *parent = 0);
	SGMFastScanParameters(const QString &element, double runSeconds, double energyStart, double energyMidpoint, double energyEnd, int velocity, int velocityBase, int acceleration, double scalerTime, int baseLine, int undulatorStartStep, int undulatorVelocity, int undulatorRelativeStep, QObject *parent = 0);
	SGMFastScanParameters(const QString &element, const SGMEdgeInfo &edgeInfo, double runSeconds, int motorSettings, double scalerTime, int baseLine, int undulatorVelocity, QObject *parent = 0);
	SGMFastScanParameters(const QString &element, const SGMStandardScanInfo &standardScanInfo, double runSeconds, int motorSettings, double scalerTime, int baseLine, int undulatorVelocity, QObject *parent = 0);


	bool operator==(const SGMFastScanParameters &other);

	QString element() const { return element_;}
	QString edge() const { return edge_;}
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
	double exitSlitDistance() const { return exitSlitDistance_;}

public slots:
	void setElement(const QString &element) { element_ = element;}
	void setEdge(const QString &edge) { edge_ = edge;}
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
	void setExitSlitDistance(double exitSlitDistance) { exitSlitDistance_ = exitSlitDistance;}

protected:
	QString element_;
	QString edge_;
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
	double exitSlitDistance_;
};

#endif // SGMELEMENTINFO_H
