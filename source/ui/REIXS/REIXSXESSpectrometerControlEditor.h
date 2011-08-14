#ifndef REIXSXESSPECTROMETERCONTROLEDITOR_H
#define REIXSXESSPECTROMETERCONTROLEDITOR_H

#include <QFrame>
#include "beamline/REIXS/REIXSBeamline.h"

namespace Ui {
	class REIXSXESSpectrometerControlEditor;
}

class REIXSXESSpectrometerControlEditor : public QFrame
{
	Q_OBJECT

public:
	explicit REIXSXESSpectrometerControlEditor(REIXSSpectrometer* spectrometer, QWidget *parent = 0);


	~REIXSXESSpectrometerControlEditor();

protected:
	REIXSSpectrometer* spectrometer_;

protected slots:

	// Respond to UI events
	void onMoveButtonClicked();
	void onStopButtonClicked();
	void onGratingComboBoxActivated(int grating);

	// external events from spectrometer
	void populateGratingComboBox();

	// External events, or internal (either the UI, or the spectrometer, has moved, so we need to update the current status)

	void updateCurrentEnergyStatus(double eV);
	void updateCurrentEnergyStatus();
	void updateCurrentGratingStatus();

	// Move failed: provide errorMon messages
	void onSpectrometerMoveSucceeded();
	void onSpectrometerMoveFailed(int reason);

private:
	Ui::REIXSXESSpectrometerControlEditor *ui_;
};

#endif // REIXSXESSPECTROMETERCONTROLEDITOR_H
