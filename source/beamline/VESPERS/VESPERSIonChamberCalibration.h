#ifndef VESPERSIONCHAMBERCALIBRATION_H
#define VESPERSIONCHAMBERCALIBRATION_H

#include <QObject>
#include <QList>

#include "beamline/VESPERS/VESPERSIonChamber.h"

/*!
  This class gives access to calibration of the ion chambers on the VESPERS beamline.  This essenstially acts like a container class.
  */
class VESPERSIonChamberCalibration : public QObject
{
	Q_OBJECT

public:
	/// Constructor.  Builds the VESPERS specific ion chamber calibration model.
	explicit VESPERSIonChamberCalibration(QObject *parent = 0);

	/// Appends a VESPERSIonChamber to the end of the list.
	void addIonChamber(VESPERSIonChamber *newIonChamber) { ionChambers_ << newIonChamber; }
	/// Removes a VESPERSIonChamber from the list based on the name.  Does NOT delete the ion chamber.
	void removeIonChamber(QString name)
	{
		for (int i = 0; i < ionChambers_.size(); i++)
			if (name == ionChambers_.at(i)->name())
				ionChambers_.removeAt(i);
	}
	/// Returns the number of ion chambers.
	int ionChamberCount() const { return ionChambers_.size(); }

	/// Returns the ion chamber from \param index.  Must be between 0 and ionChamberCount().
	VESPERSIonChamber *ionChamberAt(int index) { return ionChambers_.at(index); }

signals:

public slots:

protected:
	/// A list holding VESPERSIonChambers.

	QList<VESPERSIonChamber *> ionChambers_;
};

#endif // VESPERSIONCHAMBERCALIBRATION_H
