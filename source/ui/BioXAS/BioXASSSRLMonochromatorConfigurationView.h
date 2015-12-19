#ifndef BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_H
#define BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_H

#include <QWidget>
#include <QLayout>

class BioXASSSRLMonochromator;
class BioXASSSRLMonochromatorEnergyView;
class BioXASSSRLMonochromatorRegionControlView;
class BioXASSSRLMonochromatorRegionControlEditor;
class BioXASSSRLMonochromatorCrystalsView;
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
	/// Updates the upper blade editor.
	void updateUpperBladeEditor();
	/// Updates the lower blade editor.
	void updateLowerBladeEditor();
	/// Updates the height editor.
	void updateHeightEditor();
	/// Updates the lateral editor.
	void updateLateralEditor();
	/// Updates the paddle editor.
	void updatePaddleEditor();
	/// Updates the region editor.
	void updateRegionEditor();
	/// Updates the region status view.
	void updateRegionStatusView();

protected:
	/// The mono being viewed.
	BioXASSSRLMonochromator *mono_;

	/// The energy view.
	BioXASSSRLMonochromatorEnergyView *energyView_;

	/// The upper blade editor.
	AMExtendedControlEditor *upperBladeEditor_;
	/// The lower blade editor.
	AMExtendedControlEditor *lowerBladeEditor_;
	/// The height editor.
	AMExtendedControlEditor *heightEditor_;
	/// The lateral editor.
	AMExtendedControlEditor *lateralEditor_;
	/// The paddle editor.
	AMExtendedControlEditor *paddleEditor_;

	/// The region editor.
	BioXASSSRLMonochromatorRegionControlEditor *regionEditor_;
	/// The region status display.
	BioXASSSRLMonochromatorRegionControlView *regionStatusWidget_;
	/// The crystals view.
	BioXASSSRLMonochromatorCrystalsView *crystalsView_;
};

#endif // BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_H
