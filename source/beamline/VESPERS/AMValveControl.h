#ifndef AMVALVECONTROL_H
#define AMVALVECONTROL_H

#include <QObject>

#include "beamline/AMProcessVariable.h"

/*!
	This is a bare bones control class.  It doesn't inherit AMControl right now because this is currently a class meant to be used while we
	still don't have a good way of handling controls of this type.  This will probably be replaced once a better distinction between analog
	and digital controls is established in AMControl.
  */
class AMValveControl : public QObject
{
	Q_OBJECT
public:
	/// Constructor.  Takes in the names of the PVs for reading, opening and closing.
	explicit AMValveControl(QString name, QString readPV, QString openPV, QString closePV, QObject *parent = 0);

	/// Returns the name of the Valve control.
	QString name() const { return name_; }

	/// Get the current status of the control. 0 is closed, 1 is between, and 2 is open.
	int state() const
	{
		switch(readPV_->getInt()){
		case 1:
			return 2;
		case 4:
			return 0;
		default:
			return 1;
		}
	}
	/// Same as state() but returns a QString. Passes Open, Between, or Closed.
	QString stateString() const
	{

		switch(readPV_->getInt()){
		case 1:
			return QString("Open");
		case 4:
			return QString("Closed");
		default:
			return QString("Between");
		}
	}

signals:
	/// Notifies that the status has changed and passes on the state.
	void stateChanged(int);

public slots:
	/// Open the valve.
	void openValve() { openPV_->setValue(1); }
	/// Close the valve.
	void closeValve() { closePV_->setValue(1); }
	/// Sets the name of the valve control.
	void setName(QString name) { name_ = name; }

protected:

	// The name.
	QString name_;

	// The PVs.
	AMProcessVariable *readPV_;
	AMProcessVariable *openPV_;
	AMProcessVariable *closePV_;
};

#endif // AMVALVECONTROL_H
