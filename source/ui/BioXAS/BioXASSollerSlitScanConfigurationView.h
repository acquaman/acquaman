#ifndef BIOXASSOLLERSLITSCANCONFIGURATIONVIEW_H
#define BIOXASSOLLERSLITSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/BioXAS/BioXASSollerSlitScanConfiguration.h"

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>

class BioXASSollerSlitScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASSollerSlitScanConfigurationView(BioXASSollerSlitScanConfiguration *configuration, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSollerSlitScanConfigurationView();

	/// Returns the scan configuration.
	virtual const AMScanConfiguration *configuration() const;

protected slots:
	/// Sets the x soller slit start position.
	void setXStart(const AMNumber &value);
	/// Sets the x soller slit step size.
	void setXStep(const AMNumber &value);
	/// Sets the x soller slit end position.
	void setXEnd(const AMNumber &value);
	/// Sets the z soller slit start position.
	void setZStart(const AMNumber &value);
	/// Sets the z soller slit step size.
	void setZStep(const AMNumber &value);
	/// Sets the z soller slit end position.
	void setZEnd(const AMNumber &value);
	/// Sets the dwell time.
	void setDwellTime(const AMNumber &value);

	/// Helper slot that manages setting the x soller slit start position.
	void onXStartChanged();
	/// Helper slot that manages setting the x soller slit step size.
	void onXStepChanged();
	/// Helper slot that manages setting the x soller slit end position.
	void onXEndChanged();
	/// Helper slot that manages setting the z soller slit start position.
	void onZStartChanged();
	/// Helper slot that manages setting the z soller slit step size.
	void onZStepChanged();
	/// Helper slot that manages setting the z soller slit end position.
	void onZEndChanged();
	/// Helper slot that manages setting the time per point.
	void onDwellTimeChanged();

	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited();
	/// Helper slot that handles the setting the estimated time label.
	void onEstimatedTimeChanged();

protected:
	/// Method that updates the map info label based on the current values of the start, end, and step size.
	void updateScanInformation();
	/// Helper method that takes a time in seconds and returns a string of d:h:m:s.
	QString convertTimeToString(double time);
	/// Creates a double spin box for the position spin boxes.
	QDoubleSpinBox *createPositionDoubleSpinBox(const QString &prefix, const QString &suffix, double value, int decimals);

	/// The soller slit configuration.
	BioXASSollerSlitScanConfiguration *configuration_;

	/// The start position for the x soller slit.
	QDoubleSpinBox *xStart_;
	/// The step position for the x soller slit.
	QDoubleSpinBox *xStep_;
	/// The end position for the x soller slit.
	QDoubleSpinBox *xEnd_;
	/// The start position for the z soller slit.
	QDoubleSpinBox *zStart_;
	/// The step position for the z soller slit.
	QDoubleSpinBox *zStep_;
	/// The end position for the z soller slit.
	QDoubleSpinBox *zEnd_;
	/// Line edit for changing the name of the scan.
	QLineEdit *scanName_;
	/// Pointer to the dwell time per point.
	QDoubleSpinBox *dwellTime_;
	/// Pointer to the label that holds the current map settings.
	QLabel *scanInformation_;
	/// Label holding the current estimated time for the scan to complete.  Takes into account extra time per point based on experience on the beamline.
	QLabel *estimatedTime_;
};

#endif // BIOXASSOLLERSLITSCANCONFIGURATIONVIEW_H
