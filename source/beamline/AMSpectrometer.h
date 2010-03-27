#ifndef ACQMAN_SPECTROMETER_H_
#define ACQMAN_SPECTROMETER_H_

#include "AMControl.h"

class AMHexapod : public AMControl {
	Q_OBJECT
public:
		AMHexapod(const QString& name, QObject* parent = 0) : AMControl(name, "", parent) {
			//old: x_ = new AMPVControl("x", "HXPD1610-4-I20:X:sp", "HXPD1610-4-I20:X", "HXPD1610-4-I20:moving", 0.01, this);
			x_ = new AMPVControl("x", "HXPD1610-4-I20:X:sp", "HXPD1610-4-I20:X", 0.01, 10.0, this);
		// x_ = new PVControl("x", "SMTR1610-4-I20-RES-X:step:sp", "SMTR1610-4-I20-RES-X:step", "SMTR1610-4-I20-RES-X:moving", 0.01, this);
		// x_ = new PVControl("x", "TESTPV1610-4-I20:ai", "TESTPV1610-4-I20:ao", "", 0.01, this);

		children_ << x_;
				y_ = new AMReadOnlyPVControl("y", "HXPD1610-4-I20:Y:sp", this);
		children_ << y_;
				z_ = new AMReadOnlyPVControl("z", "HXPD1610-4-I20:Z:sp", this);
		children_ << z_;
	}

        AMPVControl* x() const		{ return x_; }
        AMReadOnlyPVControl* y() const		{ return y_; }
        AMReadOnlyPVControl* z() const		{ return z_; }

        AMReadOnlyPVControl *y_, *z_;
        AMPVControl* x_;
};

class AMSpectrometer : public AMControl
{
	Q_OBJECT
public:
		AMSpectrometer(const QString& name, QObject* parent = 0) : AMControl(name, "", parent) {

                hexapod_ = new AMHexapod("hexapod", this);
		children_ << hexapod_;

	}


        AMHexapod* hexapod() const		{ return hexapod_; }

        virtual ~AMSpectrometer() {}

protected:
        AMHexapod* hexapod_;

};

#endif /*SPECTROMETER_H_*/
