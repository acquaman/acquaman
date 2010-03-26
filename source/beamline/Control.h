#ifndef CONTROL_H
#define CONTROL_H

/* TODO first thing friday morning:

	- state refactoring; handle signals for connect/disconnect/etc. more intelligently: DONE
	- figure out why writing values doesn't work : IN PROGRESS
	- when put request fails, trigger moveFailed() (but only while should be moving)
	- data types beside double? QVariant? Subclasses?. FIXED. ProcessVariable subclasses can read/write any type, but have a native storage type.
	- Disconnects and error reporting: define system. DONE. Write doc. Query with bool isConnected(), or signals connected(bool), connected(), and disconnected(). Works with disconnects/reconnects!
	- alarms?
	- functionality in qepicsconnect: include, improve, or obsolidate
	- isMoving(), moveStarted(), and friends.... Define system, considering whether movingPV_ is or isn't connected. Separation of our moves and external moves?
	*/

#include <QObject>
#include <QString>
#include <QList>
#include "ProcessVariable.h"
#include <QDebug>

#include <cmath>

// A "Control" is the basic parent class for all setable/measurable beamline quantities/objects
// Every Control can have it's own value, as well as act as a parent for other controls.
// This parent class should be re-implemented for anything useful; it's not really helpful on its own, other than as an aggregate of many controls.
class Control : public QObject {

	Q_OBJECT
public:

	enum StateFlags {NotConnected=0, CanMeasure=1, CanMove=2};

	Control(const QString& name, const QString& units = "n/a", QObject* parent = 0) : QObject(parent), units_(units) {
		setObjectName(name);
		moveTarget_ = 0;
		state_ = NotConnected;

		connect(this, SIGNAL(connected(bool)), this, SLOT(onConnectedSignalForward(bool)));
		connect(this, SIGNAL(moving(bool)), this, SLOT(onMovingSignalForward(bool)));
	}

	// Provides a list of controls that are sub-controls of this control:
	QList<Control*> children() { return children_; }
	int numChildren() { return children_.count(); }
        /// Retreive a specific child control from the list by index
        Control* child(size_t index){
            if(index < (size_t)children_.count())
                return children_.at(index);
            return NULL;
        }

	// This value defines how close the final position must be to the move target, for the move to have succeeded.
	virtual double tolerance() { return -1; }
	// This represents the current value/position of the control. Must reimplement for actual controls.
	virtual double value() { return -1; }
	// Indicates a "fully-functional" control.
	virtual bool isConnected() { return false; }

	// This indicates the current ability / connection status of this control. Unconnected controls can't do anything; CanMeasure can read values; CanMove can set values.
	int state() { return state_; }
	bool canMeasure() { return state_ & CanMeasure; }
        virtual bool shouldMeasure() { return FALSE;}
	bool canMove() { return state_ & CanMove; }
        virtual bool shouldMove() { return FALSE;}

	// This indicates whether the control is currently being adjusted. The default implementation checks whether any of the children are moving.
	virtual bool isMoving() {
		bool isMoving = false;
		foreach(Control* c, children_) {
			isMoving |= c->isMoving();
		}
		return isMoving;
	}

	// Indicates the units of the value of this control. TODO: handle differences between measure and move units?
	virtual const QString& units() { return units_; }
	// These are the minimum and maximum allowed (input) range for this control
	virtual double minimumValue() { return -1; }
	virtual double maximumValue() { return -1; }

public slots:
	// This is used to set the control.  Must reimplement for actual controls
	virtual void move(double value) {
		moveTarget_ = value;
		emit moveFailed();  // The default implementation cannot move.
	};
	// This is used to cancel a move. Must reimplement for actual controls
	virtual void stop() {};
        /// Add a child to the control
        void addChild(Control *control) { children_ << control;}
        /// Set the control object's children (and grandchildren, etc) based on a QMap of QString and double values
        bool setStateList(const QMap<QString, double> controlList, unsigned int errorLevel = 0);

signals:
	// Announce changes in "completingMove()". These only apply to moves started by a move()... ie: they don't occur when another system causes the control to change.
	void moveStarted();
	void moveEnded();
	void moveFailed();

	// Announce changes in "isMoving()". These happen whenever the control value is changing... whether we move()'d it or not.
	void moving(bool);	// todo: make this work for parents when children move.
		void moving();	// same as moving(true)
		void stopped();	// same as moving(false)

	// Announces when the position of the control "value()" has changed.
	void valueChanged(double value);
	// Announces when the unit string text has changed. Necessary because we don't know until after a delayed control connects.
	void unitsChanged(const QString& units);


	void stateChanged(int state);	// Announces changes in state() (ie: between NotConnected, CanMeasure, and CanMove)

	void connected(bool);			// Announces changes in isConnected().  Convenience simplification of stateChanged(). Emitted (true) when full functionality is achieved. // Emitted (false) when full functionality is lost. Full functionality can be decided by the subclasses.
		void connected();					// same ase connected(true);
		void disconnected();				// same as connected(false);


protected:
	QList<Control*> children_;
	double moveTarget_;


protected slots:
	// This is used to set the unit text:
	virtual void setUnits(const QString& units) { units_ = units; emit unitsChanged(units_); }
	// This is used to set the state. It can be called by subclasses.
	virtual void setState(int state) { state_ = state; emit stateChanged(state_); }

	// This is called internally whenever a move finishes
	virtual void onMoveEnded() {
		emit moveEnded();
		emit moving(false);
		if(fabs(value() - moveTarget_) > tolerance())
			emit moveFailed();
	}
	virtual void onMoveStarted() {
		emit moveStarted();
		emit moving(true);
	}
	virtual void onMoveFailed() {
		emit moveFailed();
	}

        /// Used internally by setStateList, called recursively. MIGHT NEED TO BE VIRTUAL for reimplementation in child classes
        bool searchSetChildren(QMap<QString, double> *controlList, QMap<QString, Control*> *executeList, unsigned int errorLevel);

private: // subclasses should use the protected methods to access these, to ensure signal delivery.
	int state_;
	QString units_;

private slots:
	// This simply forwards connected(true/false) signals on as connected() and disconnected().
	void onConnectedSignalForward(bool isConnected) { if(isConnected) emit connected(); else emit disconnected(); }
	void onMovingSignalForward(bool movingYes) { if(movingYes) emit moving(); else emit stopped(); }
};

/* This class is a read-only, numeric-valued control derived from an EPICS process variable (optionally with a second process variable to indicate whether it is moving or stationary.)
Most useful members:
value(),
isMoving(),
objectName()
isConnected()
and signals
moveStarted(),
moveEnded()

TODO:
- check/handle channel disconnects and failures to connect
- access process variable names
*/
class ReadOnlyPVControl : public Control {

	Q_OBJECT

public:
	ReadOnlyPVControl(const QString& name, const QString& readPVname, const QString& movingPVname = "", QObject* parent = 0);

	// Reimplemented Public Functions:
	virtual double value() { return readPV_->lastValue(); }
	virtual bool isMoving() { if(movingPV_) return movingPV_->lastValue() || Control::isMoving(); else return Control::isMoving(); }
	virtual bool isConnected() { return readPV_->canRead(); }
	virtual double minimumValue() { return readPV_->lowerAlarmValue(); }
	virtual double maximumValue() { return readPV_->upperAlarmValue(); }

	// Additional public functions:
	QString readPVName() { return readPV_->pvName(); }
	QString movingPVName() { return (movingPV_) ? movingPV_->pvName() : QString(""); }
        bool shouldMeasure() { return TRUE;}


signals:	// These are specialized to report on PV channel connection status.  You should be free to ignore them and use the interface defined in Control::.
	void readConnectionTimeout();

protected:
	DoubleProcessVariable* readPV_;
	IntProcessVariable* movingPV_;

	// This is used to detect changes in the moving/not-moving status
	bool wasMoving_;

protected slots:
	// This is called when the read from the unit string PV completes successfully.
	virtual void onReadPVInitialized();

	// override this if you want custom handling if the readPV fails to connect.
	// You can also monitor the readConnectionTimeout() signal.
	virtual void onConnectionTimeout() { setState(Control::NotConnected); setUnits("?"); emit connected(false); emit readConnectionTimeout(); }

	// This is called when a PV channel connects or disconnects
	virtual void onPVConnected(bool connected);
	// This is called when there is a PV channel error:
	virtual void onPVError(int error);
	// This is called whenever there is an update from the move status PV
	virtual void onMovingChanged(int isMoving);

};

// This Control subclass uses one PV to write to a setpoint, and another PV to read the current value.
/* Most useful new functions:
		move()
		stop()
   and signals:
		moveStarted()
		moveEnded()
		moveFailed()
*/
class PVControl : public ReadOnlyPVControl {

	Q_OBJECT

public:
	PVControl(const QString& name, const QString& readPVname, const QString& writePVname, const QString& movingPVname = "", double tolerance = 0.01, QObject* parent = 0);

	// Reimplemented public functions:
	virtual double tolerance() { return tolerance_; }
	virtual bool isConnected() { return readPV_->canRead() && writePV_->canWrite(); }
	virtual double minimumValue() { return writePV_->lowerControlLimit(); }
	virtual double maximumValue() { return writePV_->upperControlLimit(); }
        bool valueOutOfRange(double value) { return (value > maximumValue() || value< minimumValue() ) ? TRUE : FALSE;}
        bool shouldMove() { return TRUE;}

	// Reimplemented public slots:
public slots:
	virtual void move(double value);
	// This is used to cancel a move.
	virtual void stop();

	// New signals:
signals:
	void writeConnectionTimeout();

	// Adapts the "connected()", "disconnected()", etc. signals from ReadOnlyPVControl. For example, connected() indicates that both readPVConnected() and writePVConnected() happened. disconnected() is given if either one disconnects.
protected slots:

	// override this if you want custom handling if the readPV fails to connect.
	// You can also monitor the connectionTimeout() signal.
	virtual void onConnectionTimeout();

	// This is called when a PV channel connects or disconnects
	virtual void onPVConnected(bool connected);
	// This is called when there is a PV channel error:
	virtual void onPVError(int error);


protected:
	DoubleProcessVariable* writePV_;
	IntProcessVariable* stopPV_;
	double tolerance_;

};

#endif // CONTROL_H
