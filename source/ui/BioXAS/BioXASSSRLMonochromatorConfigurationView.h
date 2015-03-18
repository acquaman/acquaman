#ifndef BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_H
#define BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_H

#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLayout>
#include <QLabel>

#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorRegionControlView.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorRegionControlEditor.h"

#define BRAGG_OFFSET_MIN -1000
#define BRAGG_OFFSET_MAX 1000

class BioXASSSRLMonochromatorConfigurationView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorConfigurationView(BioXASSSRLMonochromator *mono, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorConfigurationView();

	/// Returns the mono being viewed.
	BioXASSSRLMonochromator* mono() const { return mono_; }

signals:
	/// Notifier that the mono being viewed has changed.
	void monoChanged(BioXASSSRLMonochromator *newMono);

public slots:
	/// Sets the mono being viewed.
	void setMono(BioXASSSRLMonochromator *newMono);

protected slots:
	/// Updates the view when the bragg motor offset has changed.
	void onBraggMotorOffsetChanged();
	/// Updates the view when the bragg encoder move type has changed.
	void onBraggMotorEncoderMoveTypeChanged();
	/// Updates the view when the bragg motor settling time has changed.
	void onBraggMotorSettlingTimeChanged();

	/// Clears UI elements of mono-specific settings. Leaves the view looking as it should if there was no valid mono provided.
	void clearUI();
	/// Initializes UI elements with mono-specific settings.
	void initializeUI();
	/// Clears and re-initializes the UI elements.
	void refreshUI();

protected:
	/// The mono being viewed.
	BioXASSSRLMonochromator *mono_;

	// The region editor.
	BioXASSSRLMonochromatorRegionControlEditor *regionEditor_;
	// The energy editor.
	AMExtendedControlEditor *energyEditor_;
	/// The bragg motor position editor.
	AMExtendedControlEditor *braggMotorEditor_;
	/// The region status display.
	BioXASSSRLMonochromatorRegionControlView *regionStatusWidget_;
	/// Display for the bragg motor degree offset.
	QDoubleSpinBox *braggOffset_;
	/// Display for the bragg motor encoder move type.
	QComboBox *braggEncoderMoveType_;
	/// Display for the bragg motor move settling time.
	QDoubleSpinBox *braggSettlingTime_;

};

#endif // BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_H
