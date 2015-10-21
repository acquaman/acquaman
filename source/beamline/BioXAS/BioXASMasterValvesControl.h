#ifndef BIOXASMASTERVALVESCONTROL_H
#define BIOXASMASTERVALVESCONTROL_H

#include "beamline/BioXAS/BioXASValvesControl.h"
#include "beamline/BioXAS/BioXASValvesSetControl.h"

class BioXASMasterValvesControl : public BioXASValvesControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMasterValvesControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMasterValvesControl();

	/// Returns true if this control can take a measurement right now.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now.
	virtual bool canMove() const;

	/// Returns true if this control's value is Open, false otherwise.
	virtual bool isOpen() const;
	/// Returns true if this control's value is Closed, false otherwise.
	virtual bool isClosed() const { return !isOpen(); }

	/// Returns the front-end valves.
	BioXASValvesSetControl* frontEndValves() const { return frontEndValves_; }
	/// Returns the Side valves.
	BioXASValvesSetControl* sideValves() const { return sideValves_; }
	/// Returns the Main valves.
	BioXASValvesSetControl* mainValves() const { return mainValves_; }
	/// Returns the Imaging valves.
	BioXASValvesSetControl* imagingValves() const { return imagingValves_; }

signals:
	/// Notifier that the front-end valves have changed.
	void frontEndValvesChanged(AMControl *newValves);
	/// Notifier that the Side valves have changed.
	void sideValvesChanged(AMControl *newValves);
	/// Notifier that the Main valves have changed.
	void mainValvesChanged(AMControl *newValves);
	/// Notifier that the Imaging valves have changed.
	void imagingValvesChanged(AMControl *newValves);

public slots:
	/// Sets the front-end valves.
	void setFrontEndValves(BioXASValvesSetControl *newValves);
	/// Sets the Side valves.
	void setSideValves(BioXASValvesSetControl *newValves);
	/// Sets the Main valves.
	void setMainValves(BioXASValvesSetControl *newValves);
	/// Sets the Imaging valves.
	void setImagingValves(BioXASValvesSetControl *newValves);

protected slots:
	/// Updates the current connected state.
	virtual void updateConnected();

protected:
	/// Creates and returns an action that opens the valves in the given control set.
	virtual AMAction3* createOpenValvesAction();

protected:
	/// The front-end valves.
	BioXASValvesSetControl *frontEndValves_;
	/// The Side valves.
	BioXASValvesSetControl *sideValves_;
	/// The Main valves.
	BioXASValvesSetControl *mainValves_;
	/// The Imaging valves.
	BioXASValvesSetControl *imagingValves_;
};

#endif // BIOXASMASTERVALVESCONTROL_H
