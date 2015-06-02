#ifndef BIOXASAPPCONTROLLER_H
#define BIOXASAPPCONTROLLER_H

#include <QGroupBox>
#include <QWidget>
#include <QLayout>

#include "acquaman/BioXAS/BioXASScanConfigurationDbObject.h"
#include "acquaman/BioXAS/BioXASXRFScanConfiguration.h"

#include "actions3/actions/AMScanAction.h"
#include "actions3/actions/CLSSIS3820ScalerDarkCurrentMeasurementAction.h"
#include "actions3/AMActionRunner3.h"
#include "actions3/AMListAction3.h"

#include "analysis/AM1DDarkCurrentCorrectionAB.h"

#include "application/AMAppController.h"
#include "application/AMAppControllerSupport.h"

#include "beamline/AMBeamline.h"
#include "beamline/CLS/CLSFacilityID.h"

#include "dataman/AMRun.h"
#include "dataman/AMScanAxisEXAFSRegion.h"
#include "dataman/BioXAS/BioXASUserConfiguration.h"
#include "dataman/database/AMDbObjectSupport.h"

#include "ui/AMMainWindow.h"
#include "ui/AMTopFrame.h"
#include "ui/acquaman/AMGenericStepScanConfigurationView.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "ui/BioXAS/BioXAS32ElementGeDetectorView.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorConfigurationView.h"
#include "ui/BioXAS/BioXASXIAFiltersView.h"
#include "ui/BioXAS/BioXASCarbonFilterFarmView.h"
#include "ui/BioXAS/BioXASM1MirrorView.h"
#include "ui/BioXAS/BioXASM2MirrorView.h"
#include "ui/BioXAS/BioXASDBHRMirrorView.h"
#include "ui/BioXAS/BioXASSIS3820ScalerView.h"
#include "ui/BioXAS/BioXASFourElementVortexDetectorView.h"
#include "ui/CLS/CLSJJSlitsView.h"
#include "ui/util/AMChooseDataFolderDialog.h"

#include "util/AMPeriodicTable.h"

class BioXASAppController : public AMAppController
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASAppController(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASAppController();

	/// Destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

	/// Registers all of the necessary classes that are BioXAS-specific.
	virtual void registerClasses();

signals:

public slots:

protected:
	/// Creates and returns a QGroupBox widget containing the given view that's stretched horizontally and vertically, with the given title.
	QGroupBox *createSqueezeGroupBoxWithView(QWidget *view, const QString &title, const QString &iconName);
};

#endif // BIOXASAPPCONTROLLER_H
