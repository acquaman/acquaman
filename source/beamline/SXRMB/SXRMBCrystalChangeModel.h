#ifndef SXRMBCRYSTALCHANGEMODEL_H
#define SXRMBCRYSTALCHANGEMODEL_H

#include <QObject>

#include "beamline/AMPVControl.h"

/// Class that encapuslates the crystal changes and some of the motors that are used to calibrate it after each change.
class SXRMBCrystalChangeModel : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	explicit SXRMBCrystalChangeModel(QObject *parent = 0);
	/// Destructor.
	~SXRMBCrystalChangeModel();

	/// Returns the crystal selection.
	double crystalSelection() const;
	/// Returns the crystal Y position.
	double crystalY() const;
	/// Returns the crystal theta position.
	double crystalTheta() const;
	/// Returns the crystal chi position.
	double crystalChi();

	/// Returns the crystal selection control.
	AMControl *crystalSelectionControl() const;
	/// Returns the crystal Y control.
	AMControl *crystalYControl() const;
	/// Returns the crystal theta control.
	AMControl *crystalThetaControl() const;
	/// Returns the crystal chi control.
	AMControl *crystalChiControl() const;

signals:

public slots:

protected:
	/// The crystal selection control.
	AMPVControl *crystalSelection_;
	/// The crystal Y position control.
	AMPVwStatusControl *crystalY_;
	/// The crystal theta control.
	AMPVwStatusControl *crystalTheta_;
	/// The crystal chi control.
	AMPVwStatusControl *crystalChi_;
};

#endif // SXRMBCRYSTALCHANGEMODEL_H
