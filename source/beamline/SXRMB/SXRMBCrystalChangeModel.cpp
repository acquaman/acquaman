#include "SXRMBCrystalChangeModel.h"

SXRMBCrystalChangeModel::SXRMBCrystalChangeModel(QObject *parent)
	: QObject(parent)
{
	crystalSelection_ = new AMPVControl("Crystal Selection", "MONO1606-4-B10-01:CrysSel:fbk", "MONO1606-4-B10-01:CrysSel", QString(), this, 0.1);
	crystalY_ = new AMPVwStatusControl("Crystal Y", "SMTR1606-4-B10-12:mm:sp", "SMTR1606-4-B10-12:mm", "SMTR1606-4-B10-12:status", "SMTR1606-4-B10-12:stop", this, 0.1);
	crystalTheta_ = new AMPVwStatusControl("Crystal Theta", "SMTR1606-4-B10-13:dgr:sp", "SMTR1606-4-B10-13:dgr", "SMTR1606-4-B10-13:status", "SMTR1606-4-B10-13:stop", this, 0.1);
	crystalChi_ = new AMPVwStatusControl("Crystal Chi", "SMTR1606-4-B10-14:dgr:sp", "SMTR1606-4-B10-14:dgr", "SMTR1606-4-B10-14:status", "SMTR1606-4-B10-14:stop", this, 0.1);

	connect(crystalSelection_, SIGNAL(valueChanged(double)), this, SIGNAL(crystalSelectionChanged()));
	connect(crystalY_, SIGNAL(valueChanged(double)), this, SIGNAL(crystalYChanged(double)));
	connect(crystalTheta_, SIGNAL(valueChanged(double)), this, SIGNAL(crystalThetaChanged(double)));
	connect(crystalChi_, SIGNAL(valueChanged(double)), this, SIGNAL(crystalChiChanged(double)));
}

SXRMBCrystalChangeModel::~SXRMBCrystalChangeModel()
{

}

int SXRMBCrystalChangeModel::crystalSelection() const
{
	return int(crystalSelection_->value());
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
