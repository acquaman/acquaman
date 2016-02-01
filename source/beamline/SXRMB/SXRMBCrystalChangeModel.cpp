#include "SXRMBCrystalChangeModel.h"

SXRMBCrystalChangeModel::SXRMBCrystalChangeModel(QObject *parent)
	: QObject(parent)
{
	crystalSelection_ = new AMPVControl("Crystal Selection", "BL1606-B1-1:AddOns:CrystalSelection:fbk", "BL1606-B1-1:AddOns:CrystalSelection", QString(), this, 0.1, 60);
	crystalY_ = new AMPVwStatusControl("Crystal Y", "SMTR1606-4-B10-12:mm:sp", "SMTR1606-4-B10-12:mm", "SMTR1606-4-B10-12:status", "SMTR1606-4-B10-12:stop", this, 0.1);
	crystalTheta_ = new AMPVwStatusControl("Crystal Theta", "SMTR1606-4-B10-13:dgr:sp", "SMTR1606-4-B10-13:dgr", "SMTR1606-4-B10-13:status", "SMTR1606-4-B10-13:stop", this, 0.01);
	crystalChi_ = new AMPVwStatusControl("Crystal Chi", "SMTR1606-4-B10-14:dgr:sp", "SMTR1606-4-B10-14:dgr", "SMTR1606-4-B10-14:status", "SMTR1606-4-B10-14:stop", this, 0.01);

	connect(crystalSelection_, SIGNAL(valueChanged(double)), this, SIGNAL(crystalSelectionChanged()));
	connect(crystalSelection_, SIGNAL(moveSucceeded()), this, SLOT(onCrystalSelectionChangeCompleted()));
	connect(crystalY_, SIGNAL(valueChanged(double)), this, SIGNAL(crystalYChanged(double)));
	connect(crystalTheta_, SIGNAL(valueChanged(double)), this, SIGNAL(crystalThetaChanged(double)));
	connect(crystalChi_, SIGNAL(valueChanged(double)), this, SIGNAL(crystalChiChanged(double)));
}

SXRMBCrystalChangeModel::~SXRMBCrystalChangeModel()
{

}

bool SXRMBCrystalChangeModel::isInBetween() const
{
	SXRMBCrystalChangeModel::CrystalSelection curCrystalSelection = crystalSelection();
	return (curCrystalSelection == Unknown || curCrystalSelection == InBetween);
}

SXRMBCrystalChangeModel::CrystalSelection SXRMBCrystalChangeModel::crystalSelection() const
{
	int crystalSelectionValue = int(crystalSelection_->value());
	return SXRMBCrystalChangeModel::CrystalSelection(crystalSelectionValue);
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

void SXRMBCrystalChangeModel::onCrystalSelectionChangeCompleted()
{
	SXRMBCrystalChangeModel::CrystalSelection curCrystalSelection = crystalSelection();
	switch (curCrystalSelection) {
	case SXRMBCrystalChangeModel::InSb:
		if (!crystalTheta_->withinTolerance(0.064))
			crystalTheta_->move(0.064);
		if (!crystalChi_->withinTolerance(0.067))
			crystalChi_->move(0.067);
		break;

	case SXRMBCrystalChangeModel::Si:
		if (!crystalTheta_->withinTolerance(0.0975))
			crystalTheta_->move(0.0975);
		if (!crystalChi_->withinTolerance(-0.02))
			crystalChi_->move(-0.02);
		break;

	default: ;
	}
}
