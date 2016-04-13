#include "BioXASCryostatStage.h"

BioXASCryostatStage::BioXASCryostatStage(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	cryostatX_ = 0;
	cryostatY_ = 0;
	cryostatZ_ = 0;

	cryostatStageMotors_ = new AMMotorGroup(this);
}

BioXASCryostatStage::~BioXASCryostatStage()
{

}

bool BioXASCryostatStage::isConnected() const
{
	bool connected = (
				cryostatX_ && cryostatX_->isConnected() &&
				cryostatY_ && cryostatY_->isConnected() &&
				cryostatZ_ && cryostatZ_->isConnected()
				);

	return connected;
}

