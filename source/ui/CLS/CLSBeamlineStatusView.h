#ifndef CLSBEAMSTATUSVIEW_H
#define CLSBEAMSTATUSVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QLayout>

#include "ui/CLS/CLSBeamlineStatusButtonBar.h"

class AMControl;
class CLSBeamlineStatus;
class AMExtendedControlEditor;

class CLSBeamlineStatusView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit CLSBeamlineStatusView(CLSBeamlineStatus *beamStatus, bool compactView=false, QWidget *parent=0);
	/// Destructor.
	virtual ~CLSBeamlineStatusView();

	/// Returns the beam status being viewed.
	CLSBeamlineStatus* beamStatus() const { return beamlineStatus_; }
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
	void setBeamlineStatus(CLSBeamlineStatus *newStatus);
	/// Sets the selected component.
	void setSelectedComponent(AMControl *newControl);

protected slots:
	/// Updates the selected component view.
	void updateSelectedComponentView();

protected:
	/// creates and layouts the compact beamline status view
	QWidget*  createCompactBeamlineStatusView();
	/// creates and layouts the full beamline status view
	QWidget* createFullBeamlineStatusView();
	/// creates and returns a layout with the component buttons
	QLayout* createBeamlineStatusButtonBarLayout();
	/// Creates and returns a component view for the given control.
	virtual QWidget* createComponentView(AMControl *control);

protected:
	/// flag to identify whether this is the compact status view or full status view
	bool compactView_;
	/// The beam status being viewed.
	CLSBeamlineStatus *beamlineStatus_;
	/// The selected component.
	AMControl *selectedComponent_;

	/// The beam status editor.
	AMExtendedControlEditor *beamStatusEditor_;
	/// The beam status control button bar.
	CLSBeamlineStatusButtonBar *componentButtonBar_;

	/// The selected component view.
	QWidget *selectedComponentView_;
	/// The selected component box.
	QGroupBox *selectedComponentBox_;
};

#endif // CLSBEAMSTATUSVIEW_H
