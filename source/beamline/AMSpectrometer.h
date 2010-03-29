#ifndef ACQMAN_SPECTROMETER_H_
#define ACQMAN_SPECTROMETER_H_

#include "AMControl.h"

class AMHexapod : public AMControl {
	Q_OBJECT
public:
		AMHexapod(const QString& name, QObject* parent = 0) : AMControl(name, "", parent) {
			x_ = new AMPVwStatusControl("x", "HXPD1610-4-I20:X:sp", "HXPD1610-4-I20:X", "HXPD1610-4-I20:moving", this, 0.01);

		// x_ = new PVControl("x", "SMTR1610-4-I20-RES-X:step:sp", "SMTR1610-4-I20-RES-X:step", "SMTR1610-4-I20-RES-X:moving", 0.01, this);
		// x_ = new PVControl("x", "TESTPV1610-4-I20:ai", "TESTPV1610-4-I20:ao", "", 0.01, this);

		children_ << x_;
				y_ = new AMReadOnlyPVwStatusControl("y", "HXPD1610-4-I20:Y:sp", "HXPD1610-4-I20:moving", this);
		children_ << y_;
				z_ = new AMReadOnlyPVwStatusControl("z", "HXPD1610-4-I20:Z:sp", "HXPD1610-4-I20:moving", this);
		children_ << z_;
	}

		AMControl* x() const		{ return x_; }
		AMControl* y() const		{ return y_; }
		AMControl* z() const		{ return z_; }

		AMControl *y_, *z_;
		AMControl* x_;
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

#endif /*ACQMAN_SPECTROMETER_H_*/
