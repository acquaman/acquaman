#ifndef BIOXASBEAMSTATUSVIEW_H
#define BIOXASBEAMSTATUSVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QLayout>

class AMControl;
class CLSBeamlineStatus;
class BioXASControlEditor;
class CLSBeamlineStatusButtonBar;

class BioXASBeamStatusView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASBeamStatusView(CLSBeamlineStatus *beamStatus, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamStatusView();

	/// Returns the beam status being viewed.
	CLSBeamlineStatus* beamStatus() const { return beamStatus_; }
	/// Returns the selected component.
	AMControl* selectedComponent() const { return selectedComponent_; }

signals:
	/// Notifier that the beam status being viewed has changed.
	void beamStatusChanged(CLSBeamlineStatus *newStatus);
	/// Notifier that the selected component has changed.
	void selectedComponentChanged(AMControl *newControl);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the beam status being viewed.
	void setBeamStatus(CLSBeamlineStatus *newStatus);
	/// Sets the selected component.
	void setSelectedComponent(AMControl *newControl);

protected slots:
	/// Updates the selected component view.
	void updateSelectedComponentView();

protected:
	/// Creatse and returns a component view for the given control.
	virtual QWidget* createComponentView(AMControl *control);

protected:
	/// The beam status being viewed.
	CLSBeamlineStatus *beamStatus_;
	/// The selected component.
	AMControl *selectedComponent_;

	/// The beam status editor.
	BioXASControlEditor *editor_;
	/// The beam status control button bar.
	CLSBeamlineStatusButtonBar *buttonBar_;

	/// The selected component view.
	QWidget *selectedComponentView_;
	/// The selected component box.
	QGroupBox *selectedComponentBox_;
	/// The selected component box layout.
	QVBoxLayout *selectedComponentBoxLayout_;
};

#endif // BIOXASBEAMSTATUSVIEW_H
