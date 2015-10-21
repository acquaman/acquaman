#include "BioXASValves.h"

BioXASValves::BioXASValves(QObject *parent) :
	BioXASMasterValvesControl("BioXASValves", parent)
{
	frontEndValves_ = new BioXASFrontEndValves(this);
	setFrontEndValves(frontEndValves_);

	sideValves_ = new BioXASSideValves(this);
	setSideValves(sideValves_);

	mainValves_ = new BioXASMainValves(this);
	setMainValves(mainValves_);

	imagingValves_ = new BioXASImagingValves(this);
	setImagingValves(imagingValves_);
}

BioXASValves::~BioXASValves()
{

}
