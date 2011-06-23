#ifndef AMFASTSCANCONFIGURATION_H
#define AMFASTSCANCONFIGURATION_H

#include "AMScanConfiguration.h"

class AMFastScanConfiguration : public AMScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(double startEnergy READ startEnergy WRITE setStartEnergy)
	Q_PROPERTY(double endEnergy READ endEnergy WRITE setEndEnergy)
	Q_CLASSINFO("AMDbObject_Attributes", "description=Generic Fast Scan Configuration")

public:
	AMFastScanConfiguration(QObject *parent = 0);

	double startEnergy() const { return startEnergy_;}
	double endEnergy() const { return endEnergy_;}

	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details. Used by AMBeamlineScanAction to set the title for the action view.
	virtual QString description() const {
		return QString("Fast Scan from %1 to %2").arg(startEnergy()).arg(endEnergy());
	}

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by AMBeamlineScanAction to set the main text in the action view.
	virtual QString detailedDescription() const{
		return QString("Fast Scan from %1 to %2").arg(startEnergy()).arg(endEnergy());
	}

	/// The auto-generated scan name. Can be re-implemented to customize for each scan type.
	virtual QString autoScanName() const{
		return QString("Fast Scan");
	}

public slots:
	bool setStartEnergy(double startEnergy);// { startEnergy_ = startEnergy; setModified(true); return true;}
	bool setEndEnergy(double endEnergy);// { endEnergy_ = endEnergy; setModified(true); return true;}

signals:
	void energyRangeChanged();

protected:
	double startEnergy_;
	double endEnergy_;
};

#endif // AMFASTSCANCONFIGURATION_H
