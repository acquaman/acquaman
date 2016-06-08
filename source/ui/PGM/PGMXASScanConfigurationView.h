#ifndef PGMXASSCANCONFIGURATIONVIEW_H
#define PGMXASSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "ui/acquaman/AMGenericStepScanConfigurationDetectorsView.h"
#include "ui/dataman/AMStepScanAxisView.h"

#include "acquaman/PGM/PGMXASScanConfiguration.h"

#include "beamline/AMDetectorSet.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>

/// The scan configuration view for XAS on PGM.
class PGMXASScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.
        PGMXASScanConfigurationView(PGMXASScanConfiguration *configuration, QWidget *parent = 0);
	/// Destructor.
	virtual ~PGMXASScanConfigurationView(){}

	/// Returns the configuration.
	virtual AMScanConfiguration *configuration() const { return configuration_; }

signals:

public slots:

protected slots:
	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited();

        void onExportSelectionChanged(QAbstractButton *button);

protected:
	/// The configuration.
	PGMXASScanConfiguration *configuration_;

	/// The regions view.
	AMStepScanAxisView *regionsView_;

	/// The scan name.
	QLineEdit *scanName_;

        /// Set of dectors to display.

        QCheckBox *exportSpectraCheckBox_;

        AMGenericStepScanConfigurationDetectorsView *scientificDetectorsView_;

        AMGenericStepScanConfigurationDetectorsView *allDetectorsView_;

};

#endif // PGMXASSCANCONFIGURATIONVIEW_H
