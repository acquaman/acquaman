#ifndef SPECTROMETER_H_
#define SPECTROMETER_H_

#include "Control.h"

class Hexapod : public Control {
	Q_OBJECT
public:
	Hexapod(const QString& name, QObject* parent = 0) : Control(name, "", parent) {
		x_ = new PVControl("x", "HXPD1610-4-I20:X:sp", "HXPD1610-4-I20:X", "HXPD1610-4-I20:moving", 0.01, this);
		// x_ = new PVControl("x", "SMTR1610-4-I20-RES-X:step:sp", "SMTR1610-4-I20-RES-X:step", "SMTR1610-4-I20-RES-X:moving", 0.01, this);
		// x_ = new PVControl("x", "TESTPV1610-4-I20:ai", "TESTPV1610-4-I20:ao", "", 0.01, this);

		children_ << x_;
		y_ = new ReadOnlyPVControl("y", "HXPD1610-4-I20:Y:sp", "HXPD1610-4-I20:moving", this);
		children_ << y_;
		z_ = new ReadOnlyPVControl("z", "HXPD1610-4-I20:Z:sp", "HXPD1610-4-I20:moving", this);
		children_ << z_;
	}

	PVControl* x() const		{ return x_; }
	ReadOnlyPVControl* y() const		{ return y_; }
	ReadOnlyPVControl* z() const		{ return z_; }

	ReadOnlyPVControl *y_, *z_;
	PVControl* x_;
};

class Spectrometer : public Control
{
	Q_OBJECT
public:
	Spectrometer(const QString& name, QObject* parent = 0) : Control(name, "", parent) {

		hexapod_ = new Hexapod("hexapod", this);
		children_ << hexapod_;

	}


	Hexapod* hexapod() const		{ return hexapod_; }

	virtual ~Spectrometer() {}

protected:
	Hexapod* hexapod_;

};

#endif /*SPECTROMETER_H_*/
