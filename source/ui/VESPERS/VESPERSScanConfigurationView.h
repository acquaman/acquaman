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

	/// Method that updates the map info label based on the current values of the start, end, and step size.
	void updateScanDimensionInfo();

signals:
	/// Sends out a request that the current detector based on FluorescenceDetector to be configured.  Asks the app controller to change to the detector view.  String will be either "Single Element" or "Four Element".
	void configureDetector(const QString &);

protected slots:
	/// Handles propogating changes in the config to the It buttons.
	void updateItButtons(int It);
	/// Handles propogating changes in the config to the I0 buttons.
	void updateI0Buttons(int I0);

	/// Handles the context menu.
	void onCustomContextMenuRequested(QPoint pos);
	/// Emits the configureDetector signal based on the current fluorescence detector choice given by \param id.
	void onConfigureXRFDetectorClicked(int id);

protected:
	/// Figure out the current configuration of the regions of interest and write it out in a readable way.
	void updateAndSetRoiTextBox(int xrfId);

	/// Add the fluorescenceDetector view.  Returns a pointer to the widget.
	QGroupBox *addFluorescenceDetectorSelectionView();
	/// Add the I0 selection view.  Returns a pointer to the widget.
	QGroupBox *addI0SelectionView();
	/// Add the It selection view.  Returns a pointer to the widget.
	QGroupBox *addItSelectionView();
	/// Add the scan name view.  Returns the widget.
	QLineEdit *addScanNameView(const QString &name);
	/// Add the goToPosition group box.  Returns a pointer to the widget.
	QGroupBox *addGoToPositionView(bool goToPosition, double x, double y);
	/// Add the export path label.  Returns a pointer to the widget.
	QLabel *addExportPathLabel();
	/// Add the time offset label.
	QGroupBox *addTimeOffsetLabel(double time);
	/// Add the export options view.  Returns a pointer to the widget.
	QGroupBox *addExporterOptionsView(QStringList list, bool exportSpectra);
	/// Add the dwell time box.  Returns a pointer to the widget.
	QDoubleSpinBox *addDwellTimeWidget(double time);
	/// Build a position QDoubleSpinBox based on the prefix, suffix and value.  They have the same format, this should cut down on duplicate code.
	QDoubleSpinBox *buildPositionDoubleSpinBox(const QString &prefix, const QString &suffix, double value, int decimals);

	/// Button group for the fluorescence detector selection.
	QButtonGroup *fluorescenceButtonGroup_;
	/// Button group for the It ion chamber selection.
	QButtonGroup *ItGroup_;
	/// Button group for the I0 ion chamber selection.
	QButtonGroup *I0Group_;
	/// Button group for the exporter options.
	QButtonGroup *autoExportButtonGroup_;

	/// The spin box that holds the x coordinate for the scan position.
	QDoubleSpinBox *xPosition_;
	/// The spin box htat holds the y coordinate for the scan position.
	QDoubleSpinBox *yPosition_;
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
	/// A label holding text for the the time offset spin box.
	QLabel *timeOffsetLabel_;
	/// A spin box holding the time offset.
	QDoubleSpinBox *timeOffset_;
	/// The text edit that holds all the names of the regions of interest.
	QTextEdit *roiText_;
	/// Pointer to the dwell time per point.
	QDoubleSpinBox *dwellTime_;
	/// Line edit for changing the name of the scan.
	QLineEdit *scanName_;
};

#endif // VESPERSSCANCONFIGURATIONVIEW_H
