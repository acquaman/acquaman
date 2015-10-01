#ifndef BIOXASVALVESCONTROL_H
#define BIOXASVALVESCONTROL_H

#include "beamline/AMControlSet.h"
#include "beamline/AMPseudoMotorControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/CLS/CLSBiStateControl.h"

class BioXASValvesControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Enum providing the values this control can have.
	enum Value { None = 0, Open, Closed };

	/// Constructor.
    explicit BioXASValvesControl(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASValvesControl();

	/// Returns true if the value is always measurable, provided the control is connected.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move is always possible, provided control is connected.
	virtual bool shouldMove() const { return true; }

	/// Returns true if this control can take a measurement right now.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now.
	virtual bool canMove() const;

	/// Returns true if this control's value is Open, false otherwise.
	bool isOpen();
	/// Returns true if this control's value is Closed, false otherwise.
	bool isClosed();

	/// Returns the first front end vacuum valve control.
	AMControl *vvr1() const { return vvr1_; }
	/// Returns the second front end vacuum valve control.
	AMControl *vvr2() const { return vvr2_; }
	/// Returns the third front end vacuum valve control.
	AMControl *vvr3() const { return vvr3_; }
	/// Returns the fourth front end vacuum valve control.
	AMControl *vvr4() const { return vvr4_; }
	/// Returns the fifth front end vacuum valve control.
	AMControl *vvr5() const { return vvr5_; }

	AMControl *vvrSide1() const { return vvrSide1_; }
	AMControl *vvrSide2() const { return vvrSide2_; }
	AMControl *vvrSide3() const { return vvrSide3_; }
	AMControl *vvrSide4() const { return vvrSide4_; }
	AMControl *vvrSide5() const { return vvrSide5_; }
	AMControl *vvrSide6() const { return vvrSide6_; }

	/// Returns a string representation of the given value.
	QString valueToString(BioXASValvesControl::Value value);

signals:

public slots:

protected slots:
	/// Updates the current connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the current moving state.
	virtual void updateMoving();

protected:
	/// The first front end vacuum valve control.
	CLSBiStateControl *vvr1_;
	/// The second front end vacuum valve control.
	CLSBiStateControl *vvr2_;
	/// The third front end vacuum valve control.
	CLSBiStateControl *vvr3_;
	/// The fourth front end vacuum valve control.
	CLSBiStateControl *vvr4_;
	/// The fifth front end vacuum valve control.
	CLSBiStateControl *vvr5_;

	/// The Side beamline vacuum valve controls.
	CLSBiStateControl *vvrSide1_;
	CLSBiStateControl *vvrSide2_;
	CLSBiStateControl *vvrSide3_;
	CLSBiStateControl *vvrSide4_;
	CLSBiStateControl *vvrSide5_;
	CLSBiStateControl *vvrSide6_;
};

#endif // BIOXASVALVESCONTROL_H
