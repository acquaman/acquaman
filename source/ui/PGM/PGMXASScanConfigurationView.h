#ifndef PGMXASSCANCONFIGURATIONVIEW_H
#define PGMXASSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/PGM/PGMXASScanConfiguration.h"
#include "beamline/AMDetectorSet.h"

#include "ui/dataman/AMStepScanAxisView.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>

/// The scan configuration view for XAS on PGM.
class PGMXASScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.
        PGMXASScanConfigurationView(PGMXASScanConfiguration *configuration, AMDetectorSet *detectors, QWidget *parent = 0);
	/// Destructor.
	virtual ~PGMXASScanConfigurationView(){}

	/// Returns the configuration.
	virtual AMScanConfiguration *configuration() const { return configuration_; }

signals:

public slots:

        ///
        void setDetectors(AMDetectorSet *newDetectors );

protected slots:
	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited();

        /// Handles the change of the detector
        void onDecectorSelectionChanged(QAbstractButton *button);

        void onExportSelectionChanged(QAbstractButton *button);

protected:
	/// The configuration.
	PGMXASScanConfiguration *configuration_;

	/// The regions view.
	AMStepScanAxisView *regionsView_;

	/// The scan name.
	QLineEdit *scanName_;

        /// Set of dectors to display.
        AMDetectorSet *detectors_;
        QButtonGroup *detectorGroup_;
        QVBoxLayout *detectorLayout_;
        QMap<AMDetector*, QAbstractButton*> detectorButtonMap_;

        QCheckBox *exportSpectraCheckBox_;

};

#endif // PGMXASSCANCONFIGURATIONVIEW_H
