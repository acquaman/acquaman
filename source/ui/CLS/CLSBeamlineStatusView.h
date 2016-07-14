#ifndef CLSBEAMSTATUSVIEW_H
#define CLSBEAMSTATUSVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QLayout>
#include <QLabel>
#include <QPushButton>

#include "ui/CLS/CLSBeamlineStatusButtonBar.h"

class AMControl;
class CLSBeamline;
class CLSBeamlineStatus;
class CLSControlEditor;

class CLSBeamlineStatusView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit CLSBeamlineStatusView(CLSBeamline *beamline, bool compactView=false, bool showBeamStatusInCompactView=false, QWidget *parent=0);
	/// Destructor.
	virtual ~CLSBeamlineStatusView();

	/// add beam on/off button
	void enableBeamOnOffActions();

	/// Returns the beamline status being viewed.
	CLSBeamlineStatus* beamlineStatus() const { return beamlineStatus_; }
	/// Returns the selected component.
	AMControl* selectedComponent() const { return selectedComponent_; }

signals:
	/// Notifier that the beamline status being viewed has changed.
	void beamlineStatusComponentChanged(CLSBeamlineStatus *newStatus);
	/// Notifier that the beam on is requested
	void beamOnRequested();
	/// Notifier that the beam off is requested
	void beamOffRequested();
	/// Notifier that a component has been clicked.
	void controlClicked(AMControl *control);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the beam status being viewed.
	void setBeamlineStatusComponent(CLSBeamlineStatus *newStatus);
	/// Sets the selected component.
	void setSelectedComponent(AMControl *newControl);

protected slots:
	/// Updates the selected component view.
	void updateSelectedComponentView();

protected:
	/// creates and layouts the compact beamline status view
	QWidget*  createCompactBeamlineStatusView(bool showBeamStatus=false);
	/// creates and layouts the full beamline status view
	QWidget* createFullBeamlineStatusView();
	/// creates and returns a layout with the component buttons
	QLayout* createBeamlineStatusButtonBarLayout();

	/// creates and returns a layout with the beam on/off buttons
	QLayout* createBeamOnOffButtons();

	/// Creates and returns a component view for the given control.
	virtual QWidget* createComponentView(AMControl *control);

	/// helper functions to create a push button
	QPushButton *createPushButton(QString text);

protected:
	/// flag to identify whether this is the compact status view or full status view
	bool compactView_;

	/// The beamline being operated
	CLSBeamline *beamline_;
	/// The beam status being viewed.
	CLSBeamlineStatus *beamlineStatus_;
	/// The selected component.
	AMControl *selectedComponent_;

	QVBoxLayout *beamStatusContentLayout_;
	/// The beam status editor.
	CLSControlEditor *beamStatusEditor_;
	/// The beam status control button bar.
	CLSBeamlineStatusButtonBar *beamlineStatusButtonBar_;

	/// The selected component view.
	QWidget *selectedComponentView_;
	/// The selected component box.
	QGroupBox *selectedComponentBox_;

	/// the beam on button
	QPushButton *beamOnButton_;
	/// the beam off button
	QPushButton *beamOffButton_;

};

#endif // CLSBEAMSTATUSVIEW_H
