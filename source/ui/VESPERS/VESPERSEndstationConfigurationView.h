#ifndef VESPERSENDSTATIONCONFIGURATIONVIEW_H
#define VESPERSENDSTATIONCONFIGURATIONVIEW_H

#include <QWidget>

#include <QButtonGroup>
#include <QGroupBox>
#include <QCheckBox>

#include "beamline/VESPERS/VESPERSEndstationConfiguration.h"

/*! This view builds the endstation configuration.  It enables the choices for geometry, technique, and detectors.  Based on this,
	it will setup various pieces of the beamline to be congruent with the choices made here.
  */
class VESPERSEndstationConfigurationView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Builds the view based on the endstation configuration it receives.
	explicit VESPERSEndstationConfigurationView(VESPERSEndstationConfiguration *config, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Slot that handles changes in geometry.  Takes in the button id.
	void onGeometryClicked(int id);
	/// Slot that handles changes in the techniques.  Takes in the button id.
	void onTechniquesClicked(int id);
	/// Slot that handles changes in the detectors.  Takes in the button id.
	void onDetectorsClicked(int id);

protected:
	/// Helper method that goes through all the possibilities of the endstation configuration and enables/disables detectors and techniques depending on the geometry.
	void updateAppearance();

	/// Pointer to the configuration we are viewing.
	VESPERSEndstationConfiguration *endstation_;

	/// Button group for the geometry.
	QButtonGroup *geometry_;
	/// Button group for the techniques.
	QButtonGroup *techniques_;
	/// Button group for the detectors.
	QButtonGroup *detectors_;

	/// The group box for the geometry.
	QGroupBox *geometryBox_;
	/// The group box for the techniques.
	QGroupBox *techniquesBox_;
	/// The group box for the detectors.
	QGroupBox *detectorsBox_;

	/// The check box for XAS.
	QCheckBox *xas_;
	/// The check box for XRF.
	QCheckBox *xrf_;
	/// The check box for XRD.
	QCheckBox *xrd_;

	/// The check box for the ion chambers.
	QCheckBox *ionChambers_;
	/// The check box for the single element vortex.
	QCheckBox *vortex1E_;
	/// The check box for the four element vortex.
	QCheckBox *vortex4E_;
	/// The check box for the Roper CCD.
	QCheckBox *roperCCD_;
};

#endif // VESPERSENDSTATIONCONFIGURATIONVIEW_H
