#ifndef SXRMBCRYSTALCHANGEMODEL_H
#define SXRMBCRYSTALCHANGEMODEL_H

#include <QObject>

#include "beamline/AMPVControl.h"

/// Class that encapuslates the crystal changes and some of the motors that are used to calibrate it after each change.
class SXRMBCrystalChangeModel : public QObject
{
	Q_OBJECT

	enum CrystalSelection {
		InSb = 0,
		Si,
		InBetween,
		Unknown
	};

public:
	/// Constructor.
	explicit SXRMBCrystalChangeModel(QObject *parent = 0);
	/// Destructor.
	~SXRMBCrystalChangeModel();

	/// Returns whehter the crystal selection is in between (unknown or inBetween.
	bool isInBetween() const;
	/// Returns the crystal selection
	SXRMBCrystalChangeModel::CrystalSelection crystalSelection() const;
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

	/// Notifier that the crystal selection has changed.
	void crystalSelectionChanged();
	/// Notifier that the crystal Y position has changed.
	void crystalYChanged(double);
	/// Notifier that the crystal Theta position has changed.
	void crystalThetaChanged(double);
	/// Notifier that the crystal Chi position has changed.
	void crystalChiChanged(double);

protected slots:
	/// SLOT to change the values of Theta and Chi when crystal selection change completed
	void onCrystalSelectionChangeCompleted();

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
