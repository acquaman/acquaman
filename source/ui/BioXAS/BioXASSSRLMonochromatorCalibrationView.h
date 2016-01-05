#ifndef BIOXASSSRLMONOCHROMATORCALIBRATIONVIEW_H
#define BIOXASSSRLMONOCHROMATORCALIBRATIONVIEW_H

#include <QWidget>
#include <QLayout>
#include <QPushButton>
#include <QInputDialog>

#define BIOXASSSRLMONOCHROMATORENERGYVIEW_ENERGY_MIN -1000000
#define BIOXASSSRLMONOCHROMATORENERGYVIEW_ENERGY_MAX 1000000
#define BIOXASSSRLMONOCHROMATORENERGYVIEW_BRAGG_MIN -1000000
#define BIOXASSSRLMONOCHROMATORENERGYVIEW_BRAGG_MAX 1000000

class BioXASSSRLMonochromator;

class BioXASSSRLMonochromatorCalibrationView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorCalibrationView(BioXASSSRLMonochromator *mono, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorCalibrationView();

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

	/// The calibrate energy button.
	QPushButton *calibrateEnergyButton_;
	/// The calibrate bragg button.
	QPushButton *calibrateBraggButton_;
};

#endif // BIOXASSSRLMONOCHROMATORCALIBRATIONVIEW_H
