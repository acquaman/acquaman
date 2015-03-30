#include "SXRMBCrystalChangeModel.h"

SXRMBCrystalChangeModel::SXRMBCrystalChangeModel(QObject *parent)
	: QObject(parent)
{
	crystalSelection_ = new AMPVControl("Crystal Selection", "MONO1606-4-B10-01:CrysSel:fbk", "MONO1606-4-B10-01:CrysSel:fbk", QString(), this, 0.1);
	crystalY_ = new AMPVwStatusControl("Crystal Y", "SMTR1606-4-B10-12:mm:sp", "SMTR1606-4-B10-12:mm", "SMTR1606-4-B10-12:status", "SMTR1606-4-B10-12:stop", this, 0.1);
	crystalTheta_ = new AMPVwStatusControl("Crystal Theta", "SMTR1606-4-B10-13:dgr:sp", "SMTR1606-4-B10-13:dgr", "SMTR1606-4-B10-13:status", "SMTR1606-4-B10-13:stop", this, 0.1);
	crystalChi_ = new AMPVwStatusControl("Crystal Chi", "SMTR1606-4-B10-14:dgr:sp", "SMTR1606-4-B10-14:dgr", "SMTR1606-4-B10-14:status", "SMTR1606-4-B10-14:stop", this, 0.1);
}

SXRMBCrystalChangeModel::~SXRMBCrystalChangeModel()
{

}

double SXRMBCrystalChangeModel::crystalSelection() const
{
	return crystalSelection_->value();
}

double SXRMBCrystalChangeModel::crystalY() const
{
	return crystalY_->value();
}

double SXRMBCrystalChangeModel::crystalTheta() const
{
	return crystalTheta_->value();
}

double SXRMBCrystalChangeModel::crystalChi()
{
	return crystalChi_->value();
}

AMControl * SXRMBCrystalChangeModel::crystalSelectionControl() const
{
	return crystalSelection_;
}

AMControl * SXRMBCrystalChangeModel::crystalYControl() const
{
	return crystalY_;
}

AMControl * SXRMBCrystalChangeModel::crystalThetaControl() const
{
	return crystalTheta_;
}

AMControl * SXRMBCrystalChangeModel::crystalChiControl() const
{
	return crystalChi_;
}
