#ifndef BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_H
#define BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_H

#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QInputDialog>

#define ENERGY_MIN -1000000
#define ENERGY_MAX 1000000
#define BRAGG_POSITION_MIN -1000000
#define BRAGG_POSITION_MAX 1000000
#define VIEW_WIDTH_MIN 300

class BioXASSSRLMonochromator;
class BioXASSSRLMonochromatorEnergyView;
class BioXASSSRLMonochromatorRegionControlView;
class BioXASSSRLMonochromatorRegionControlEditor;
class BioXASSSRLMonochromatorCrystalsView;
class CLSMAXvMotorConfigurationView;
class AMExtendedControlEditor;

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
	/// Refreshes the view.
	void refresh();
	/// Sets the mono being viewed.
	void setMono(BioXASSSRLMonochromator *newMono);

protected slots:
	/// Updates the bragg configuration view.
	void updateBraggConfigurationView();

protected:
	/// The mono being viewed.
	BioXASSSRLMonochromator *mono_;

	/// The upper slit blade editor.
	AMExtendedControlEditor *upperSlitEditor_;
	/// The lower slit blade editor.
	AMExtendedControlEditor *lowerSlitEditor_;
	/// The height editor.
	AMExtendedControlEditor *heightEditor_;
	/// The lateral editor.
	AMExtendedControlEditor *lateralEditor_;
	/// The paddle editor.
	AMExtendedControlEditor *paddleEditor_;

	/// The energy view.
	BioXASSSRLMonochromatorEnergyView *energyView_;

	/// The region editor.
	BioXASSSRLMonochromatorRegionControlEditor *regionEditor_;
	/// The region status display.
	BioXASSSRLMonochromatorRegionControlView *regionStatusWidget_;
	/// The crystals view.
	BioXASSSRLMonochromatorCrystalsView *crystalsView_;

	/// The bragg configuration view.
	CLSMAXvMotorConfigurationView *braggConfigurationView_;
};

#endif // BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_H
