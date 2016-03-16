#ifndef BIOXASBEAMLINECONFIGURATIONVIEW_H
#define BIOXASBEAMLINECONFIGURATIONVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QCheckBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QLayout>

#define BIOXASBEAMLINECONFIGURATIONVIEW_NONE_BUTTON_INDEX 0
#define BIOXASBEAMLINECONFIGURATIONVIEW_DIODE_BUTTON_INDEX 1
#define BIOXASBEAMLINECONFIGURATIONVIEW_PIPS_BUTTON_INDEX 2
#define BIOXASBEAMLINECONFIGURATIONVIEW_LYTLE_BUTTON_INDEX 3

class BioXASBeamlineConfigurationView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASBeamlineConfigurationView(QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamlineConfigurationView();

public slots:
	/// Refreshes the view.
	void refresh();

protected slots:
	/// Updates the cryostat button.
	void updateCryostatButton();

	/// Updates the scaler channel buttons.
	void updateScalerChannelButtons();
	/// Updates the none button.
	void updateNoneButton();
	/// Updates the diode button.
	void updateDiodeButton();
	/// Updates the PIPS button.
	void updatePIPSButton();
	/// Updates the Lytle button.
	void updateLytleButton();

	/// Handles updating the beamline with the cryostat button selection.
	void onCryostatButtonClicked();
	/// Handles updating the beamline with the current extra detector selection.
	void onChannelDetectorButtonClicked(int buttonIndex);

protected:
	/// The button group for the optional detectors.
	QButtonGroup *extraChannelDetectorsButtonGroup_;

	/// The cryostat button.
	QCheckBox *cryostatButton_;

	/// The none button, an extra channel detectors option.
	QRadioButton *noneButton_;
	/// The diode button, an extra channel detectors option.
	QRadioButton *diodeButton_;
	/// The PIPS button, an extra channel detectors option.
	QRadioButton *pipsButton_;
	/// The Lytle button, an extra channel detectors option.
	QRadioButton *lytleButton_;
};

#endif // BIOXASBEAMLINECONFIGURATIONVIEW_H
