#ifndef BIOXASMASTERVALVES_H
#define BIOXASMASTERVALVES_H

#include "beamline/BioXAS/BioXASValves.h"

class BioXASFrontEndValves;
class BioXASSideValves;
class BioXASMainValves;
class BioXASImagingValves;

class BioXASMasterValves : public BioXASValves
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASMasterValves(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMasterValves();

	/// Returns the front end valves.
	BioXASFrontEndValves* frontEndValves() const { return frontEndValves_; }
	/// Returns the Side valves.
	BioXASSideValves* sideValves() const { return sideValves_; }
	/// Returns the Main valves.
	BioXASMainValves* mainValves() const { return mainValves_; }
	/// Returns the Imaging valves.
	BioXASImagingValves* imagingValves() const { return imagingValves_; }

signals:
	/// Notifier that the front end valves have changed.
	void frontEndValvesChanged(BioXASFrontEndValves *newValves);
	/// Notifier that the Side valves have changed.
	void sideValvesChanged(BioXASSideValves *newValves);
	/// Notifier that the Main valves have changed.
	void mainValvesChanged(BioXASMainValves *newValves);
	/// Notifier that the Imaging valves have changed.
	void imagingValvesChanged(BioXASImagingValves *newValves);

public slots:
	/// Sets the front end valves.
	void setFrontEndValves(BioXASFrontEndValves *newValves);
	/// Sets the Side valves.
	void setSideValves(BioXASSideValves *newValves);
	/// Sets the Main valves.
	void setMainValves(BioXASMainValves *newValves);
	/// Sets the Imaging valves.
	void setImagingValves(BioXASImagingValves *newValves);

protected slots:
	/// Updates the connected state. Reimplemented to only consider the expected valve options.
	virtual void updateConnected();

protected:
	/// The front end valves.
	BioXASFrontEndValves *frontEndValves_;
	/// The Side valves.
	BioXASSideValves *sideValves_;
	/// The Main valves.
	BioXASMainValves *mainValves_;
	/// The Imaging valves.
	BioXASImagingValves *imagingValves_;
};

#endif // BIOXASMASTERVALVES_H
