#ifndef BIOXASSIDEZEBRA_H
#define BIOXASSIDEZEBRA_H

#include "beamline/BioXAS/BioXASZebra.h"

class BioXASSideZebra : public BioXASZebra
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASSideZebra(const QString &baseName, QObject *parent = 0);
	/// Destuctor.
	virtual ~BioXASSideZebra();

	/// Returns true if the fast shutter AND block is connected to the 'scan' pulse, false otherwise.
	bool fastShutterBlockActive() const;
	/// Returns true if the scaler OR block is connected to the 'scan' pulse, false otherwise.
	bool scalerBlockActive() const;
	/// Returns true if the Xspress3 pulse block is connected to the 'scan' pulse, false otherwise.
	bool xspress3BlockActive() const;

	/// Returns the fast shutter AND block.
	BioXASZebraLogicBlock* fastShutterBlock() const { return andBlockAt(0); }
	/// Returns the scaler OR block.
	BioXASZebraLogicBlock* scalerBlock() const { return orBlockAt(1); }
	/// Returns the Ge detector pulse block.
	BioXASZebraPulseControl* geDetectorBlock() const { return pulseControlAt(2); }

signals:
	/// Notifier that the fast shutter block activity has changed.
	void fastShutterBlockActivityChanged(bool isActive);
	/// Notifier that the scaler block activity has changed.
	void scalerBlockActivityChanged(bool isActive);
	/// Notifier that the Xspress3 block activity has changed.
	void xspress3BlockActivityChanged(bool isActive);

public slots:
	/// Sets the fast shutter block as active.
	void setFastShutterBlockActive();
	/// Sets the fast shutter block as inactive.
	void setFastShutterBlockInactive();

	/// Sets the scaler block as active.
	void setScalerBlockActive();
	/// Sets the scaler block as inactive.
	void setScalerBlockInactive();

	/// Sets the Xspress3 block as active.
	void setXspress3BlockActive();
	/// Sets the Xspress3 block as inactive.
	void setXspress3BlockInactive();

protected slots:
	/// Handles emitting the notifier that the fast shutter block activity has changed.
	void onFastShutterBlockInputValueChanged();
	/// Handles emitting the notifier that the scaler block activity has changed.
	void onScalerBlockInputValueChanged();
	/// Handles emitting the notifier that the Xspress3 block activity has changed.
	void onXspress3BlockInputValueChanged();

	/// Handles updating the activity of Zebra blocks, in response to a change in detectors or detector managers.
	virtual void updateBlockActivity();
};

#endif // BIOXASSIDEZEBRA_H
