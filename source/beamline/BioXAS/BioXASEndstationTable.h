#ifndef BIOXASENDSTATIONTABLE_H
#define BIOXASENDSTATIONTABLE_H

#include <QObject>

#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"

class BioXASEndstationTable : public QObject
{
    Q_OBJECT
public:
	/// constructor:
	explicit BioXASEndstationTable(const QString &name, const QString &pvBaseName, const bool canRoll=false, QObject *parent = 0);

	/// returns whether the endstation table is connected or not
	bool isConnected() const;

	/// exposes the height PV conroller
	AMControl *heightPVController() const { return heightPVController_; }
	/// exposes the pitch PV conroller
	AMControl *pitchPVController() const { return pitchPVController_; }
	/// exposes the lateral PV conroller
	AMControl *lateralPVController() const { return lateralPVController_; }
	/// exposes the yawPV conroller
	AMControl *yawPVController() const { return yawPVController_; }
	/// exposes the roll PV conroller
	AMControl *rollPVController() const { return rollPVController_; }

signals:
	/// signal for the connected status of all the PVs of the endtable
	void connected(bool);

	/// signal for the height value changed
	void heightValueChanged(double);
	/// signal for the pitch value changed
	void pitchValueChanged(double);
	/// signal for the lateral value changed
	void lateralValueChanged(double);
	/// signal for the yaw value changed
	void yawValueChanged(double);
	/// signal for the roll value changed
	void rollValueChanged(double);

public slots:
	/// handle the signal / command to move the table vertically
	void moveHeight(double);
	/// handle the signal / command to pitch the table
	void movePitch(double);
	/// handle the signal / command to move the table horizontally
	void moveLateral(double);
	/// handle the signal / command to yaw the table
	void moveYaw(double);
	/// handle the signal / command to roll the table
	void moveRoll(double);

protected slots:
	/// handle the PV connected singal
	void onPVConnected();

protected:
	/// the connected status of the table
	bool wasConnected_;

	/// the name of the endstation table
	QString name_;

	/// the PV control of the height PV
	AMControl *heightPVController_;
	/// the PV conrol of the pitch PV
	AMControl *pitchPVController_;
	/// the PV control of the lateral PV
	AMControl *lateralPVController_;
	/// the PV control of the yaw PV
	AMControl *yawPVController_;
	/// the PV control of the roll PV. NOTE: not all endstations have this roll PV
	AMControl *rollPVController_;
};

#endif // BIOXASENDSTATIONTABLE_H
