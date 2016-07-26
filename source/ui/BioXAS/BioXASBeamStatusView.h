#ifndef BIOXASBEAMSTATUSVIEW_H
#define BIOXASBEAMSTATUSVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/BioXAS/BioXASBeamStatus.h"
#include "ui/BioXAS/BioXASBeamStatusButtonBar.h"
#include "ui/CLS/CLSControlEditor.h"

class BioXASBeamStatusView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASBeamStatusView(BioXASBeamStatus *beamStatus, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamStatusView();

	/// Returns the beam status being viewed.
	BioXASBeamStatus* beamStatus() const { return beamStatus_; }
	/// Returns the selected beam status component.
	AMControl* selectedComponent() const { return getSelectedComponent(); }

signals:
	/// Notifier that the beam status being viewed has changed.
	void beamStatusChanged(BioXASBeamStatus *newBeamStatus);
	/// Notifier that the selected component has changed.
	void selectedComponentChanged(AMControl *newSelection);

public slots:
	/// Sets the beam status being viewed.
	void setBeamStatus(BioXASBeamStatus *newBeamStatus);
	/// Sets the selected beam status component.
	void setSelectedComponent(AMControl *newSelection);

protected slots:
	/// Updates the whole beam status view.
	void updateBeamStatusView();
	/// Updates the beam status editor.
	void updateBeamStatusEditor();
	/// Updates the button bar.
	void updateButtonBar();
	/// Updates the selected component view.
	void updateSelectedComponentView();

protected:
	/// Creates and returns a component view for the given component.
	virtual QWidget* createComponentView(AMControl *component);

	/// Returns the selected component.
	virtual AMControl* getSelectedComponent() const;

protected:
	/// The beam status.
	BioXASBeamStatus *beamStatus_;

	/// The selected component view.
	QWidget *selectedComponentView_;

	/// The beam status editor.
	CLSControlEditor *beamStatusEditor_;
	/// The beam status button bar.
	BioXASBeamStatusButtonBar *buttonBar_;
	/// The layout for the selected component box.
	QVBoxLayout *selectedComponentBoxLayout_;
	/// The selected component box.
	QGroupBox *selectedComponentBox_;
};

#endif // BIOXASBEAMSTATUSVIEW_H
