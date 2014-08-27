/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef REIXSRIXSSCANCONFIGURATIONVIEW_H
#define REIXSRIXSSCANCONFIGURATIONVIEW_H

#include "ui/dataman/AMChooseScanDialog.h"
#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"
#include "dataman/REIXS/REIXSXESCalibration2.h"

#include "dataman/AMScanSetModel.h"
#include "dataman/AMAnalysisBlock.h"

#include "source/ui/actions3/AMActionRunnerQueueView3.h"


#include <QGroupBox>

class QComboBox;
class QDoubleSpinBox;
class QTimeEdit;
class QLabel;
class QRadioButton;
class QCheckBox;
class AMSamplePre2013Editor;
class QLineEdit;
class QSpinBox;
class QPushButton;
class QTableWidget;

class MPlotWidget;
class MPlot;
class MPlotImageBasic;
class MPlotRectangle;
class MPlotSeriesBasic;
class MPlotColorMap;
class MPlotPoint;

class AMScan;
class AMScanView;
class AMAnalysisBlock;

class REIXSRIXSScanConfigurationView : public AMScanConfigurationView
{
    Q_OBJECT
public:
	explicit REIXSRIXSScanConfigurationView(REIXSXESScanConfiguration* config = 0, QWidget *parent = 0);
	virtual ~REIXSRIXSScanConfigurationView();

	virtual const AMScanConfiguration* configuration() const { return configuration_; }

	/// Add a scan to an editor. The editor retains an interest in \c newScan, keeping it in memory as long as required. \see AMScan::retain().
	void addScan(AMScan* newScan);

	void removeScan(AMScan* scan);


protected:
	/// The internal configuration that we modify.
	REIXSXESScanConfiguration* configuration_;
	/// If we've created scanConfiguration_, we can delete it when we are deleted. If it belongs to someone else, don't.
	bool ownsConfiguration_;

	/// A spectrometer calibration object to work with. Used to determine the options for other parameters.
	REIXSXESCalibration2 calibration_;

	/// The current calibrationId that we're working with (initially -1)
	int currentCalibrationId_;

	/// Dialog to ask the user for a scan
	AMChooseScanDialog* chooseScanDialog_;

	/// This is a model containing the current open scans
	AMScanSetModel* scanSetModel_;
	/// This is the currently-selected scan, or 0 non-existent
	AMScan* currentScan_;

	AMActionRunnerQueueView3* queueView_;

	/*
	MPlotSeriesBasic Plot_;
	MPlotWidget* plotWidget_;
	MPlot* plot_;
	*/

	/// Plot view capable of holding multiple scans.
	AMScanView* scanView_;



	//UI Elements

	QGroupBox* XASEnergySelect;
	QGroupBox* detectorOptions;
	QGroupBox* stopScanOptions;
	QGroupBox* nameOptions;


	QDoubleSpinBox* energyBox_;
	QLabel* energyBoxLabel_;
	QDoubleSpinBox* slitWidthBox_;

	QDoubleSpinBox* polarizationAngleBox_;
	QComboBox* polarizationBox_;
	QLabel* polarizationAngleLabel_;

	QCheckBox* doNotClearExistingCountsOption_;
	QCheckBox* applySlitWidthBox_;
	QCheckBox* applyPolarizationBox_;



	QDoubleSpinBox* maximumTotalCounts_;
	QTimeEdit* maximumTimeEdit_;

	QLineEdit* nameEdit_;
	QSpinBox* numberEdit_;
	AMSamplePre2013Editor* sampleSelector_;
	QCheckBox* autoNamingCheckBox_;

	QLabel* XASScanName_;
	QPushButton* loadXASData_;

	/*
	QPushButton* queueScan_;
	/// Overloaded to enable drag-dropping scans (when Drag Action = Qt::CopyAction and mime-type = "text/uri-list" with the proper format.)
	void dragEnterEvent(QDragEnterEvent *event);

	/// Overloaded to enable drag-dropping scans.
	 The Drag is accepted when:
	  - Drag Action = Qt::CopyAction
	  - One of the MIME types is "text/uri-list"... format is "amd://databaseConnectionName/tableName/id"
	  - There is at least one URL in the uri-list
	  - The URL scheme is "amd://"
	  - The database connection name returns a valid database, according to AMDatabase::dbByName(connectionName)
	  - The table is the main Objects table
	  - The id of the item can be found in the table

	void dropEvent(QDropEvent * event);
	*/



signals:

	/// Notifier that a scan has been added for visualization to the editor.  Passes a reference to the editor for when there are many editors open at a time.
	void scanAdded(REIXSRIXSScanConfigurationView *, AMScan *);


protected slots:

	/// Call this function to open a set of scans from the database. The scan information is contained inside a list of "amd://..." URLs.  For more information on the format, see dropEvent().   Returns true if the list contains at least one valid scan that was added.
	/*! This function is used as an internal helper function by dropEvent(); Normally you should use the dropScanURLs function in AMDatamanAppController() since it can check for scans being open in other editors*/
	bool dropScanURLs(const QList<QUrl>& urls);

	/// Call this to update the window title when a scan is added or removed
	void refreshWindowTitle();

	/// Forwards the signal when you adjust the time edit to control the maximum duration of the scan
	void onMaximumTimeEditChanged(const QTime& time);

	/// Disables the meta-data controls when the 'set automatically" checkbox is activated.
	void onAutoNamingCheckboxClicked(bool autoOn);

	/// Determines whether to enable the polarization angle (only if the polarization is enabled, and set to Linear Inclined).
	void reviewPolarizationAngleBoxEnabled();

	/// Pushes x value from scan view clisk to energy spin box
	void onDataPositionToolChanged(const QPointF &newData);

	/// Pushes x value from scan view clisk to energy spin box
	void onEnergySpinBoxChanged(double enegry);




public slots:

	void onloadXASDataClicked();
	void onloadXASDataChosen();
	/*
	void onQueueScanClicked();
	*/
};

#endif // REIXSRIXSSCANCONFIGURATIONVIEW_H
