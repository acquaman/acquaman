#ifndef BIOXASSSRLMONOCHROMATORENERGYVIEW_H
#define BIOXASSSRLMONOCHROMATORENERGYVIEW_H

#include <QWidget>
#include <QLayout>
#include <QPushButton>
#include <QInputDialog>

#define BIOXASSSRLMONOCHROMATORENERGYVIEW_ENERGY_MIN -1000000
#define BIOXASSSRLMONOCHROMATORENERGYVIEW_ENERGY_MAX 1000000
#define BIOXASSSRLMONOCHROMATORENERGYVIEW_BRAGG_MIN -1000000
#define BIOXASSSRLMONOCHROMATORENERGYVIEW_BRAGG_MAX 1000000

class BioXASSSRLMonochromator;
class AMExtendedControlEditor;

class BioXASSSRLMonochromatorEnergyView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorEnergyView(BioXASSSRLMonochromator *mono, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorEnergyView();

	/// Returns the mono being viewed.
	BioXASSSRLMonochromator* mono() const { return mono_; }

signals:
	/// Notifier that the mono being viewed has changed.
	void monoChanged(BioXASSSRLMonochromator *newControl);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the mono being viewed.
	void setMono(BioXASSSRLMonochromator *newControl);

protected slots:
	/// Updates the step energy editor.
	void updateStepEnergyEditor();
	/// Updates the encoder energy editor.
	void updateEncoderEnergyEditor();
	/// Updates the step bragg editor.
	void updateStepBraggEditor();
	/// Updates the encoder bragg editor.
	void updateEncoderBraggEditor();
	/// Updates the M1 mirror pitch editor.
	void updateMirrorPitchEditor();
	/// Updates the calibrate energy button.
	void updateCalibrateEnergyButton();
	/// Updates the calibrate bragg button.
	void updateCalibrateBraggButton();

	/// Handles calibrating the energy when the button is clicked.
	void onCalibrateEnergyButtonClicked();
	/// Handles calibrating the bragg when the button is clicked.
	void onCalibrateBraggButtonClicked();

protected:
	/// The mono being viewed.
	BioXASSSRLMonochromator *mono_;

	/// The step energy editor.
	AMExtendedControlEditor *stepEnergyEditor_;
	/// The encoder energy editor.
	AMExtendedControlEditor *encoderEnergyEditor_;
	/// The step bragg editor.
	AMExtendedControlEditor *stepBraggEditor_;
	/// The encoder bragg editor.
	AMExtendedControlEditor *encoderBraggEditor_;
	/// The M1 mirror pitch editor.
	AMExtendedControlEditor *mirrorPitchEditor_;

	/// The calibrate energy button.
	QPushButton *calibrateEnergyButton_;
	/// The calibrate bragg button.
	QPushButton *calibrateBraggButton_;
};

#endif // BIOXASSSRLMONOCHROMATORENERGYVIEW_H