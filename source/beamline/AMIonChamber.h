#ifndef AMIONCHAMBER_H
#define AMIONCHAMBER_H

#include "dataman/info/AMIonChamberInfo.h"
#include "beamline/AMDetector.h"
#include "beamline/AMControl.h"

/*!
  This class is an abstraction for general ion chambers.
  */
class AMIonChamber : public AMIonChamberInfo, public AMDetector
{
    Q_OBJECT
public:
	explicit AMIonChamber(QObject *parent = 0);

	/// Get the current reading
	virtual double reading() const { return counts(); }

	/// AMDetector sub classes need to reimplement this to return their own detectorInfo class. NEEDS TO RETURN A NEW INSTANCE, CALLER IS RESPONSIBLE FOR MEMORY.
	virtual AMDetectorInfo* toInfo() const { return new AMIonChamberInfo(*this); }
	/// Info specific to the ion chamber.
	AMIonChamberInfo toIonChamberInfo() const { return AMIonChamberInfo(*this); }

	/// The description() of a detector is a human-readable, free-form string.
	virtual QString description() const { return description_; }
	/// Descriptions can be changed at will, and the detector will emit infoChanged() when this happens.
	virtual void setDescription(const QString& description) { description_ = description; }

	/// Sets the current detector from the given detector info.
	virtual bool setFromInfo(const AMDetectorInfo *info);
	/// Sets the current detector from the given detector info.
	bool setFromIonChamberInfo(const AMIonChamberInfo &info);

	/// Pure virtual function.  Returns the current counts for the ion chamber.
	double counts() = 0;
	/// Pure virtual function.  Returns the voltage for the ion chamber.
	double voltage() = 0;

signals:
	/// Notifier that the counts have changed.
	void countsChanged(double);
	/// Notifier that the voltage has changed.
	void voltageChanged(double);
	/// Notifier that the sensitivity has changed.
	void sensitivityChanged();

public slots:
	/// Pure virtual function.  Increases the sensitivity of the ion chamber.  How this is done is implementation specific.
	bool increaseSensitivity() = 0;
	/// Pure virtual function.  Decreases the sensitivity of the ion chamber.  How this is done is implementation specific.
	bool decreaseSensitivity() = 0;

protected:
	/// The description of the ion chamber.
	QString description_;
};

#endif // AMIONCHAMBER_H
