#ifndef SXRMBOXIDATIONMAPSCANCONFIGURATIONVIEWHOLDER_H
#define SXRMBOXIDATIONMAPSCANCONFIGURATIONVIEWHOLDER_H

#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/SXRMB/SXRMB2DOxidationMapScanConfigurationView.h"

/// This holder extends the normal holder to make a list of 2D maps for the oxidation maps done on SXRMB.
class SXRMBOxidationMapScanConfigurationViewHolder : public AMScanConfigurationViewHolder3
{
	Q_OBJECT

public:
	/// Constructor.
	SXRMBOxidationMapScanConfigurationViewHolder(SXRMB2DOxidationMapScanConfigurationView *view = 0, QWidget *parent = 0);
	/// Destructor.
	virtual ~SXRMBOxidationMapScanConfigurationViewHolder();

protected:
	/// Re-implemented to make a list of of 2D maps with specific energies.
	virtual AMAction3 *createScan();
};

#endif // SXRMBOXIDATIONMAPSCANCONFIGURATIONVIEWHOLDER_H
