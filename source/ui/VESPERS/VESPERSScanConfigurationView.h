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


#ifndef VESPERSSCANCONFIGURATIONVIEW_H
#define VESPERSSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "dataman/AMUser.h"
#include "application/VESPERS/VESPERS.h"

#include <QLineEdit>
#include <QTextEdit>
#include <QButtonGroup>
#include <QDoubleSpinBox>
#include <QToolButton>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>
#include <QMenu>
#include <QComboBox>

/*! This class is primarily for having helper methods to all of the subclasses for when
	they build up the varios GUI elements.  Because many of them use the exact same code
	they are brought together here.  Many of the methods return a QWidget* but they still
	have their specific pointers to individual buttons, spin boxes, etc..
*/
class VESPERSScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.  Passes in the configuraiton that this view will modify.
	VESPERSScanConfigurationView(QWidget *parent = 0);
	/// Destructor.
	virtual ~VESPERSScanConfigurationView();

signals:
	/// Sends out a request that the current detector based on FluorescenceDetector to be configured.  Asks the app controller to change to the detector view.  String will be either "Single Element" or "Four Element".
	void configureDetector(const QString &);

protected slots:
	/// Handles propogating changes in the config to the It buttons.
	void updateItComboBox(int It);
	/// Handles propogating changes in the config to the I0 buttons.
	void updateI0ComboBox(int I0);
	/// Slot that updates the fluorescence detector buttons.
	void updateFluorescenceDetectorComboBox(int detector);
	/// Slot that updates the ccd detector buttons.
	void updateCCDDetectorComboBox(int detector);
	/// Slot that updates the motor choice buttons.
	void updateMotorSelectionComboBox(int choice);

	/// Handles the context menu.
	void onCustomContextMenuRequested(QPoint pos);
	/// Emits the configureDetector signal based on the current fluorescence detector choice given by \param id.
	QString fluorescenceDetectorIdToString(int id);
	/// Emits the configureDetector signal based on the current ccd detector choice given by \param id.
	QString ccdDetectorIdToString(int id);

protected:
	/// Creates a combo box for the fluorescenceDetector enum.  Returns a pointer to the widget.
	QComboBox *createFluorescenceComboBox();
	/// Creates a combo box for the CCDDetector enum.  Returns a pointer to the widget.
	QComboBox *createCCDComboBox();
	/// Creates a combo box for the I0 selection.  Returns a pointer to the widget.
	QComboBox *createIonChamberComboBox();
	/// Creates a combo box for the motor selection.  Returns a pointer to the widget.  \note Both lists must be the same size!
	QComboBox *createMotorSelectionComboBox(QStringList labels, QList<int> ids);
	/// Add the scan name view.  Returns the widget.
	QLineEdit *createScanNameView(const QString &name);
	/// Add the goToPosition group box.  Returns a pointer to the widget.
	QGroupBox *addGoToPositionView(bool goToPosition, double x, double y);
	/// Add the export path label.  Returns a pointer to the widget.
	QLabel *addExportPathLabel();
	/// Add the time offset label.
	QGroupBox *addTimeOffsetLabel(double time);
	/// Add the export options view.  Returns a pointer to the widget.
	QGroupBox *addExporterOptionsView(QStringList list, bool exportSpectra, bool exportSpectraInRows);
	/// Add the dwell time box.  Returns a pointer to the widget.
	QDoubleSpinBox *createDwellTimeSpinBox(double time);
	/// Build a position QDoubleSpinBox based on the prefix, suffix and value.  They have the same format, this should cut down on duplicate code.
	QDoubleSpinBox *createPositionDoubleSpinBox(const QString &prefix, const QString &suffix, double value, int decimals);
	/// Creates a group box that holds "things to do after a scan" option.
	QGroupBox *createAfterScanOptionsBox(bool autoCloseShutter, bool gotoOriginalPosition, bool cleanupScaler);

	/// This disables the common items for the fluorescence detector (disables None).
	void disableStandardFluorescenceOptions();
	/// This disables the common items for the CCD detectors (disables Mar and Roper).
	void disableStandardXRDOptions();
	/// This disables the common items for the I0 (Ipost).
	void disableStandardI0Options();
	/// This disables the common items for the It (Isplit).
	void disableStandardItOptions();

	/// Button group for the exporter options.
	QButtonGroup *autoExportButtonGroup_;

	/// Label holding what the currently saved x position is in the scan configuration.
	QLabel *savedXPosition_;
	/// Label holding what the currently saved y position is in the scan configuration.
	QLabel *savedYPosition_;
	/// Label holding whether or not the x and y positions have been saved yet.
	QLabel *positionsSaved_;
	/// Check box for enabling the go to position check box.
	QCheckBox *goToPositionCheckBox_;
	/// Button for saving the current coordinates of the motor.
	QPushButton *setCurrentPositionButton_;
	/// Check box for enabling/disabling whether or not spectra should be exported or not.
	QCheckBox *autoExportSpectra_;
	/// Check box for deciding whether the spectra should be exported in columns or rows.
	QCheckBox *exportSpectraInRows_;
	/// A label holding text for the the time offset spin box.
	QLabel *timeOffsetLabel_;
	/// A spin box holding the time offset.
	QDoubleSpinBox *timeOffset_;
	/// The label that holds useful information about the CCD - such as path and name.
	QLabel *ccdText_;
	/// The label that holds the help message when CCD file names conflict.
	QLabel *ccdHelpText_;
	/// Pointer to the dwell time per point.
	QDoubleSpinBox *dwellTime_;
	/// Line edit for changing the name of the scan.
	QLineEdit *scanName_;
	/// Combo box for choosing the fluorescence detector.
	QComboBox *fluorescenceDetectorComboBox_;
	/// Combo box for choosing the I0 ion chamber.
	QComboBox *i0ComboBox_;
	/// Combo box for choosing the It ion chamber.
	QComboBox *itComboBox_;
	/// Combo box for choosing the CCD detector.
	QComboBox *ccdComboBox_;
	/// Combo box for choosing the motors.
	QComboBox *motorSelectionComboBox_;
	/// Check box for whether the fast shutter should be closed at the end of a scan.
	QCheckBox *closeFastShutterCheckBox_;
	/// Check box for whether the sample stage should go back to its original position after a scan.
	QCheckBox *returnToOriginalPositionCheckBox_;
	/// Check box for whether the scaler should be cleaned up.
	QCheckBox *cleanupScalerCheckBox_;
};

#endif // VESPERSSCANCONFIGURATIONVIEW_H
