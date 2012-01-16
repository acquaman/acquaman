/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef SGMELEMENTINFO_H
#define SGMELEMENTINFO_H

#include <QObject>
#include <QDebug>
#include "util/AMElement.h"
#include "util/AMOrderedSet.h"
#include "dataman/database/AMDbObject.h"

class SGMFastScanParameters;

class SGMEnergyPosition : public AMDbObject
{
Q_OBJECT

Q_PROPERTY(double energy READ energy WRITE setEnergy)
Q_PROPERTY(int monoEncoderTarget READ monoEncoderTarget WRITE setMonoEncoderTarget)
Q_PROPERTY(int undulatorStepSetpoint READ undulatorStepSetpoint WRITE setUndulatorStepSetpoint)
Q_PROPERTY(double exitSlitDistance READ exitSlitDistance WRITE setExitSlitDistance)
Q_PROPERTY(int sgmGrating READ sgmGrating WRITE setSGMGrating)

Q_CLASSINFO("AMDbObject_Attributes", "description=SGM Energy Settings")

public:
	Q_INVOKABLE SGMEnergyPosition(double energy = -1, int monoEncoderTarget = -1, int undulatorStepSetpoint = -1, double exitSlitDistance = -1, int sgmGrating = -1, const QString &name = QString());

	double energy() const { return energy_;}
	int monoEncoderTarget() const { return monoEncoderTarget_;}
	int undulatorStepSetpoint() const { return undulatorStepSetpoint_;}
	double exitSlitDistance() const { return exitSlitDistance_;}
	int sgmGrating() const { return sgmGrating_;}

	SGMEnergyPosition& operator=(const SGMEnergyPosition& other);
	friend QDebug operator<<(QDebug d, const SGMEnergyPosition &energyPosition);

public slots:
	void setEnergy(double energy) { energy_ = energy;}
	void setMonoEncoderTarget(int monoEncoderTarget) { monoEncoderTarget_ = monoEncoderTarget;}
	void setUndulatorStepSetpoint(int undulatorStepSetpoint) { undulatorStepSetpoint_ = undulatorStepSetpoint;}
	void setExitSlitDistance(double exitSlitDistance) { exitSlitDistance_ = exitSlitDistance;}
	void setSGMGrating(int sgmGrating) { sgmGrating_ = sgmGrating;}

protected:
	double energy_;
	int monoEncoderTarget_;
	int undulatorStepSetpoint_;
	double exitSlitDistance_;
	int sgmGrating_;
};

class SGMScanRangeInfo : public AMDbObject
{
Q_OBJECT

Q_PROPERTY(AMDbObject* start READ dbGetStart WRITE dbLoadStart)
Q_PROPERTY(AMDbObject* middle READ dbGetMiddle WRITE dbLoadMiddle)
Q_PROPERTY(AMDbObject* end READ dbGetEnd WRITE dbLoadEnd)

Q_CLASSINFO("AMDbObject_Attributes", "description=SGM Scan Range Info")

public:
	Q_INVOKABLE SGMScanRangeInfo(const SGMEnergyPosition &start = SGMEnergyPosition(), const SGMEnergyPosition &middle = SGMEnergyPosition(), const SGMEnergyPosition &end = SGMEnergyPosition(), const QString &name = QString());
	SGMScanRangeInfo(const SGMScanRangeInfo &other);

	SGMEnergyPosition start() const { return start_;}
	SGMEnergyPosition middle() const { return middle_;}
	SGMEnergyPosition end() const { return end_;}

	SGMScanRangeInfo& operator=(const SGMScanRangeInfo &other);
	friend QDebug operator<<(QDebug d, const SGMScanRangeInfo &scanRangeInfo);

protected:
	AMDbObject* dbGetStart() { return &start_;}
	AMDbObject* dbGetMiddle() { return &middle_;}
	AMDbObject* dbGetEnd() { return &end_;}

	void dbLoadStart(AMDbObject*) {} //never called
	void dbLoadMiddle(AMDbObject*) {} //never called
	void dbLoadEnd(AMDbObject*) {} //never called

protected:
	SGMEnergyPosition start_;
	SGMEnergyPosition middle_;
	SGMEnergyPosition end_;
};


class SGMEdgeInfo : public AMDbObject
{
Q_OBJECT

Q_PROPERTY(QString edge READ edge WRITE setEdge)
Q_PROPERTY(double energy READ energy WRITE setEnergy)
Q_PROPERTY(AMDbObject* standardRange READ dbGetStandardRange WRITE dbLoadStandardRange)

Q_CLASSINFO("AMDbObject_Attributes", "description=SGM Edge Info")

public:
	Q_INVOKABLE SGMEdgeInfo(const QString &edge = "", double energy = -1, const SGMScanRangeInfo &standardRange = SGMScanRangeInfo(), const QString &name = QString());
	SGMEdgeInfo(QPair<QString, QString> edgeAndEnergy, const SGMScanRangeInfo &standardRange, const QString &name = QString());

	QString edge() const { return edge_;}
	double energy() const { return energy_;}
	SGMScanRangeInfo standardRange() const { return standardRange_;}
	SGMEnergyPosition standardStart() const { return standardRange_.start();}
	SGMEnergyPosition standardMiddle() const { return standardRange_.middle();}
	SGMEnergyPosition standardEnd() const { return standardRange_.end();}

	SGMEdgeInfo& operator=(const SGMEdgeInfo &other);
	friend QDebug operator<<(QDebug d, const SGMEdgeInfo &edgeInfo);

public slots:
	void setEdge(const QString &edge) { edge_ = edge;}
	void setEnergy(double energy) { energy_ = energy;}

protected:
	AMDbObject* dbGetStandardRange() { return &standardRange_;}
	void dbLoadStandardRange(AMDbObject*) {} //never called

protected:
	QString edge_;
	double energy_;
	SGMScanRangeInfo standardRange_;
};

class SGMStandardScanInfo : public AMDbObject
{
Q_OBJECT

Q_PROPERTY(QString scanName READ scanName WRITE setScanName)
Q_PROPERTY(AMDbObject* standardRange READ dbGetStandardRange WRITE dbLoadStandardRange)

Q_CLASSINFO("AMDbObject_Attributes", "description=SGM Standard Scan Info")

public:
	Q_INVOKABLE SGMStandardScanInfo(const QString &scanName = "", const SGMScanRangeInfo &standardRange = SGMScanRangeInfo(), const QString &name = QString());

	QString scanName() const { return scanName_;}
	SGMScanRangeInfo standardRange() const { return standardRange_;}
	SGMEnergyPosition standardStart() const { return standardRange_.start();}
	SGMEnergyPosition standardMiddle() const { return standardRange_.middle();}
	SGMEnergyPosition standardEnd() const { return standardRange_.end();}

	SGMStandardScanInfo& operator=(const SGMStandardScanInfo &other);
	friend QDebug operator<<(QDebug d, const SGMStandardScanInfo &standardScanInfo);

public slots:
	void setScanName(const QString &scanName) { scanName_ = scanName;}

protected:
	AMDbObject* dbGetStandardRange() { return &standardRange_;}
	void dbLoadStandardRange(AMDbObject*) {} //never called

protected:
	QString scanName_;
	SGMScanRangeInfo standardRange_;
};


class SGMElementInfo : public AMDbObject
{
Q_OBJECT

Q_PROPERTY(AMDbObjectList sgmEdgeInfos READ dbReadSGMEdgeInfos WRITE dbLoadSGMEdgeInfos)
Q_PROPERTY(AMDbObjectList sgmStandardScanInfos READ dbReadSGMStandardScanInfos WRITE dbLoadSGMStandardScanInfos)
Q_PROPERTY(AMDbObjectList sgmFastScanParameters READ dbReadSGMFastScanParameters WRITE dbLoadSGMFastScanParameters)

Q_CLASSINFO("AMDbObject_Attributes", "description=SGM Element Info")

public:
	Q_INVOKABLE SGMElementInfo(AMElement *element = 0, QObject *parent = 0, const QString &name = QString());

	AMElement* element() const;

	AMOrderedSet<QString, SGMEdgeInfo> sgmEdgeInfos() const;
	AMOrderedSet<QString, SGMStandardScanInfo> sgmStandardScanInfos() const;
	AMOrderedSet<int, SGMFastScanParameters*> availableFastScanParameters() const;

public slots:
	bool addEdgeInfo(const SGMEdgeInfo &edgeInfo);
	bool addStandardScanInfo(const SGMStandardScanInfo &standardScanInfo);
	bool addFastScanParameters(SGMFastScanParameters *parameters);

protected:
	AMDbObjectList dbReadSGMEdgeInfos();
	AMDbObjectList dbReadSGMStandardScanInfos();
	AMDbObjectList dbReadSGMFastScanParameters();

	void dbLoadSGMEdgeInfos(const AMDbObjectList &sgmEdgeInfos);
	void dbLoadSGMStandardScanInfos(const AMDbObjectList &sgmStandardScanInfos);
	void dbLoadSGMFastScanParameters(const AMDbObjectList &sgmFastScanParameters);

protected:
	AMElement *element_;
	AMOrderedSet<QString, SGMEdgeInfo> sgmEdgeInfos_;
	AMOrderedSet<QString, SGMStandardScanInfo> sgmStandardScanInfos_;

	AMOrderedSet<int, SGMFastScanParameters*> availableFastScanParameters_;
};

class SGMFastScanSettings : public AMDbObject
{
Q_OBJECT

public:
	explicit SGMFastScanSettings(double runSeconds = -1, int motorSettings = 0, double scalerTime = -1, int baseLine = -1, int undulatorVelocity = 0, QObject *parent = 0, const QString &name = QString());

	double runSeconds() const;
	int motorSettings() const;
	double scalerTime() const;
	int baseLine() const;
	int undulatorVelocity() const;

	SGMFastScanSettings& operator =(const SGMFastScanSettings &other);

public slots:
	void setRunSeconds(double runSeconds);
	void setMotorSettings(int motorSettings);
	void setScalerTime(double scalerTime);
	void setBaseLine(int baseLine);
	void setUndulatorVelocity(int undulatorVelocity);

signals:
	void runSecondsChanged(double runSeconds);
	void motorSettingsChanged(int motorSetting);
	void scalerTimeChanged(double scalerTime);
	void baseLineChanged(int baseLine);
	void undulatorVelocityChanged(int undulatorVelocity);

protected:
	double runSeconds_;
	int motorSettings_;
	double scalerTime_;
	int baseLine_;
	int undulatorVelocity_;
};

class SGMFastScanParameters : public AMDbObject
{
Q_OBJECT

Q_PROPERTY(QString element READ element WRITE setElement)
Q_PROPERTY(AMDbObject* edgeInfo READ dbGetEdgeInfo WRITE dbLoadEdgeInfo)
Q_PROPERTY(AMDbObject* standardScanInfo READ dbGetStandardScanInfo WRITE dbLoadStandardScanInfo)
Q_PROPERTY(AMDbObject* fastScanSettings READ dbGetFastScanSettings WRITE dbLoadFastScanSettings)
Q_PROPERTY(bool usingEdgeInfo READ usingEdgeInfo WRITE setUsingEdgeInfo)

Q_CLASSINFO("AMDbObject_Attributes", "description=SGM Fast Scan Parameters")

public:
	SGMFastScanParameters(QObject *parent = 0);
	SGMFastScanParameters(const QString &element, const SGMEdgeInfo &edgeInfo, const SGMFastScanSettings &fastScanSettings, QObject *parent = 0, const QString &name = QString());
	SGMFastScanParameters(const QString &element, const SGMStandardScanInfo &standardScanInfo, const SGMFastScanSettings &fastScanSettings, QObject *parent = 0, const QString &name = QString());


	bool operator==(const SGMFastScanParameters &other);

	QString element() const;// { return element_;}
	QString edge() const; //{ return edge_;}
	double runSeconds() const; //{ return runSeconds_;}
	double energyStart() const; //{ return energyStart_;}
	double energyMidpoint() const; //{ return energyMidpoint_;}
	double energyEnd() const; //{ return energyEnd_;}
	int velocity() const; //{ return velocity_;}
	int velocityBase() const; //{ return velocityBase_;}
	int acceleration() const; //{ return acceleration_;}
	double scalerTime() const; //{ return scalerTime_;}
	int baseLine() const; //{ return baseLine_;}

	int undulatorStartStep() const; //{ return undulatorStartStep_;}
	int undulatorVelocity() const; //{ return undulatorVelocity_;}
	int undulatorRelativeStep() const; //{ return undulatorRelativeStep_;}
	double exitSlitDistance() const; //{ return exitSlitDistance_;}
	int sgmGrating() const; //{ return sgmGrating_;}

	SGMEdgeInfo edgeInfo() const;
	SGMStandardScanInfo standardScanInfo() const;
	SGMFastScanSettings fastScanSettings() const;

public slots:
	void setElement(const QString &element); //{ element_ = element;}

	void setEdge(const QString &edge);
	void setRunSeconds(double runSeconds);
	void setEnergyStart(double energyStart);
	void setEnergyMidpoint(double energyMidpoint);
	void setEnergyEnd(double energyEnd);
	void setVelocity(int velocity);
	void setVelocityBase(int velocityBase);
	void setAcceleration(int acceleration);
	void setScalerTime(double scalerTime);
	void setBaseLine(int baseLine);
	void setUndulatorStartStep(int undulatorStartStep);
	void setUndulatorVelocity(int undulatorVelocity);
	void setUndulatorRelativeStep(int undulatorRelativeStep);
	void setExitSlitDistance(double exitSlitDistance);
	void setSGMGrating(int sgmGrating);

	void setEdgeInfo(SGMEdgeInfo &edgeInfo);
	void setStandardScanInfo(SGMStandardScanInfo &standardScanInfo);
	void setFastScanSettings(SGMFastScanSettings &fastScanSettings);

protected slots:
	void setUsingEdgeInfo(bool usingEdgeInfo);

protected:
	AMDbObject* dbGetEdgeInfo() { return &edgeInfo_;}
	void dbLoadEdgeInfo(AMDbObject*) {} //never called

	AMDbObject* dbGetStandardScanInfo() {return &standardScanInfo_;}
	void dbLoadStandardScanInfo(AMDbObject*) {} //never called

	AMDbObject* dbGetFastScanSettings() { return &fastScanSettings_;}
	void dbLoadFastScanSettings(AMDbObject*) {} //never called

	bool usingEdgeInfo() const;

protected:
	QString element_;

	SGMEdgeInfo edgeInfo_;
	SGMStandardScanInfo standardScanInfo_;
	SGMFastScanSettings fastScanSettings_;

	bool usingEdgeInfo_;
};

#endif // SGMELEMENTINFO_H
