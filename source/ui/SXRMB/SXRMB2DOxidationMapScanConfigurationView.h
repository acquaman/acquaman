#ifndef SXRMB2DOXIDATIONMAPSCANCONFIGURATIONVIEW_H
#define SXRMB2DOXIDATIONMAPSCANCONFIGURATIONVIEW_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QButtonGroup>
#include <QComboBox>
#include <QCheckBox>


#include "acquaman/SXRMB/SXRMB2DMapScanConfiguration.h"

#include "ui/util/AMEnergyListView.h"
#include "ui/SXRMB/SXRMBScanConfigurationView.h"

class SXRMB2DOxidationMapScanConfigurationView : public SXRMBScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.
	SXRMB2DOxidationMapScanConfigurationView(SXRMB2DMapScanConfiguration *configuration, QWidget *parent = 0);
	/// Destructor.
	virtual ~SXRMB2DOxidationMapScanConfigurationView() {}

	/// Getter for the configuration.
	const AMScanConfiguration *configuration() const { return configuration_; }

	/// Method that updates the map info label based on the current values of the start, end, and step size.
	void updateMapInfo();

	/// Returns the energies stored inside the energy list.
	QList<double> oxidationEnergies() const { return oxidationEnergyListView_->energies().energies(); }

protected slots:
	/// SLot that handles beamline endstation switched signal
	void onBeamlineEndstationChanged(SXRMB::Endstation fromEndstation, SXRMB::Endstation toEndstation);

	/// Sets the x-axis start position.
	void setXAxisStart(const AMNumber &value);
	/// Sets the y-axis start position.
	void setYAxisStart(const AMNumber &value);
	/// Sets the x-axis step position.
	void setXAxisStep(const AMNumber &value);
	/// Sets the y-axis step position.
	void setYAxisStep(const AMNumber &value);
	/// Sets the x-axis end position.
	void setXAxisEnd(const AMNumber &value);
	/// Sets the y-axis start position.
	void setYAxisEnd(const AMNumber &value);
	/// Sets the dwell time.
	void setDwellTime(const AMNumber &value);

	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited();
	/// Handles setting the start position when the "Use Current" button is pushed.
	void onSetStartPosition();
	/// Handles setting the end position when the "Use Current" button is pushed.
	void onSetEndPosition();
	/// Handles setting the normal position when the "Set Normal" button is pushed.
	void onSetNormalPosition();
	/// Helper slot that manages setting the x axis start position.
	void onXStartChanged();
	/// Helper slot that manages setting the x axis end position.
	void onXEndChanged();
	/// Helper slot that manages setting the x axis step size.
	void onXStepChanged();
	/// Helper slot that manages setting the y axis start position.
	void onYStartChanged();
	/// Helper slot that manages setting the y axis end position.
	void onYEndChanged();
	/// Helper slot that manages setting the y axis step size.
	void onYStepChanged();
	/// Helper slot that manages setting the normal position.
	void onNormalPositionChanged();
	/// Helper slot that manages setting the time per point.
	void onDwellTimeChanged();

	/// Helper slot that handles the setting the estimated time label.
	void onEstimatedTimeChanged();
	/// Helper slot that sets whether we use SMAK or Ascii for the auto exporter.
	void updateAutoExporter(int useAscii);
	/// Handles changes to detector selection
	void onFluorescenceDetectorChanged(int detector);
	/// Handles changes to power on TEY HV control flag
	void onPowerOnTEYHVControlEnabled(bool);

	/// Helper method that checks to see if the map dimensions are acceptable.  Makes visual changes if not valid.
	void checkScanAxisValidity();

protected:
	/// Add the export options view.  Returns a pointer to the widget.
	QGroupBox *addExporterOptionsView(QStringList list);
	/// Add the scan name view.  Returns the widget.
	QLineEdit *createScanNameView(const QString &name);
	/// Build a position QDoubleSpinBox based on the prefix, suffix and value.  They have the same format, this should cut down on duplicate code.
	QDoubleSpinBox *createPositionDoubleSpinBox(const QString &prefix, const QString &suffix, double value, int decimals);
	/// Add the dwell time box.  Returns a pointer to the widget.
	QDoubleSpinBox *createDwellTimeSpinBox(double time);

	/// Pointer to the specific scan config the view is modifying.
	SXRMB2DMapScanConfiguration *configuration_;

	/// Pointer to the dwell time per point.
	QDoubleSpinBox *dwellTime_;
	/// Line edit for changing the name of the scan.
	QLineEdit *scanName_;
	/// Pointer to the horizontal start point.
	QDoubleSpinBox *hStart_;
	/// Pointer to the horizontal end point.
	QDoubleSpinBox *hEnd_;
	/// Pointer to the vertical start point.
	QDoubleSpinBox *vStart_;
	/// Pointer to the vertical end point.
	QDoubleSpinBox *vEnd_;
	/// Pointer to the horizontal step size.
	QDoubleSpinBox *hStep_;
	/// Pointer to the vertical step size.
	QDoubleSpinBox *vStep_;
	/// Pointer to the normal position used for the scan.
	QDoubleSpinBox *normalPosition_;

	/// Pointer to the label that holds the current map settings.
	QLabel *mapInfo_;
	/// Label holding the current estimated time for the scan to complete.  Takes into account extra time per point based on experience on the beamline.
	QLabel *estimatedTime_;

	/// This is an error display label.
	QLabel *errorLabel_;

	/// Button group for the exporter options.
	QButtonGroup *autoExportButtonGroup_;

	/// The energy list view.
	AMEnergyListView *oxidationEnergyListView_;
};

#endif // SXRMB2DOXIDATIONMAPSCANCONFIGURATIONVIEW_H
