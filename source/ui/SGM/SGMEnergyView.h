#ifndef SGMENERGYVIEW_H
#define SGMENERGYVIEW_H

#include <QWidget>
#include <QPushButton>

class AMExtendedControlEditor;
class AMValidatorIcon;
class SGMEnergyControlSet;
class SGMEnergyTrajectoryView;
/*!
  * A ui class representing a view for visualizing the energy controls of the
  * SGM beamline.
  *
  * TODO: Rig up the validator of the energy control set to the view.
  */
class SGMEnergyView : public QWidget
{
	Q_OBJECT
public:
	/// Enumerates the different levels of detail that an energy view is to display
	enum EnergyViewType {
		Simple,			// Just shows the energy, grating translation, translation mode and harmonic.
		Advanced		// Shows all the energy controls
	};

	/*!
	  * Creates an instance of an energy control view, for the visualization of
	  * the beamline energy controls provided.
	  * \param energyControlSet ~ The energy controls to visualize.
	  * \param viewType ~ Optional parameter for specifying the level of detail
	  * to display in the view. Defaults to simple.
	  */
	explicit SGMEnergyView(SGMEnergyControlSet* energyControlSet,
	                       EnergyViewType viewType = Simple,
	                       QWidget *parent = 0);

signals:

public slots:
protected slots:
	/*!
	  * Slot which handles the checkbox for controlling the undulator tracking being
	  * toggled.
	  */
	void onUndulatorTrackingButtonToggled(bool isToggled);

	/*!
	  * Slot which handles the checkbox for controlling the exit slit tracking being
	  * toggled.
	  */
	void onExitSlitPositionTrackingButtonToggled(bool isToggled);

	/*!
	  * Slot which handles the undulator tracking being altered at the PV level.
	  */
	void onEnergyControlUndulatorTrackingChanged(double);

	/*!
	  * Slot which handles the exit slit tracking being altered at the PV level.
	  */
	void onEnergyControlExitSlitTrackingChanged(double);

	/*!
	  * Slot which handles the grating translation optimization mode being altered
	  * at the PV level.
	  */
	void onEnergyControlGratingTrackingOptimizationModeChanged(double);

	/*!
	  * Slot which handles the stop button being clicked.
	  */
	void onStopEnergyButtonClicked();
protected:

	/*!
	  * Helper function for initializing and performing layout of the child widgets
	  * for the view.
	  * \param viewType ~ Whether the view to setup should be Simple or Advanced.
	  */
	void setupUi(EnergyViewType viewType);

	AMExtendedControlEditor* energyEditor_;
	AMExtendedControlEditor* gratingAngleEditor_;
	AMExtendedControlEditor* gratingTranslationModeEditor_;
	AMExtendedControlEditor* gratingTranslationEditor_;

	AMExtendedControlEditor* undulatorDetuneOffsetEditor_;
	AMExtendedControlEditor* undulatorHarmonicEditor_;
	AMExtendedControlEditor* undulatorPositionEditor_;
	AMExtendedControlEditor* exitSlitPositionEditor_;

	SGMEnergyTrajectoryView* energyTrajectoryView_;

	QPushButton* undulatorTrackingButton_;
	QPushButton* exitSlitTrackingButton_;
	QPushButton* stopEnergyButton_;

	SGMEnergyControlSet* energyControlSet_;
	AMValidatorIcon* errorValidatorIcon_;
	AMValidatorIcon* warningValidatorIcon_;
};

#endif // SGMENERGYVIEW_H
