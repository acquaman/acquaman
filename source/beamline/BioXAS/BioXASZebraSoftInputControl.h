#ifndef BIOXASZEBRASOFTINPUTCONTROL_H
#define BIOXASZEBRASOFTINPUTCONTROL_H

#include "beamline/AMPVControl.h"

class BioXASZebraSoftInputControl : public AMSinglePVControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASZebraSoftInputControl(const QString& name,
										 const QString& PVname,
										 QObject* parent = 0,
										 double tolerance = AMCONTROL_TOLERANCE_DONT_CARE,
										 double completionTimeoutSeconds = 10.0,
										 const QString &description = "");
	/// Destructor.
	virtual ~BioXASZebraSoftInputControl();

	/// Returns the high value.
	double highValue() const;

	/// Returns the high value control.
	AMSinglePVControl* highValueControl() const { return highValue_; }

signals:
	/// Notifier that the high value has changed.
	void highValueChanged(double newValue);

public slots:
	/// Sets the high value.
	void setHighValue(double newValue);

protected:
	/// The .HIGH field.
	AMSinglePVControl *highValue_;
};

#endif // BIOXASZEBRASOFTINPUTCONTROL_H
