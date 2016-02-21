#ifndef BIOXASBEAMSTATUSBAR_H
#define BIOXASBEAMSTATUSBAR_H

#include <QWidget>
#include <QSignalMapper>
#include <QAbstractButton>
#include <QMap>

class AMControl;
class BioXASBeamStatus;
class BioXASButtonEditorBar;

class BioXASBeamStatusBar : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASBeamStatusBar(BioXASBeamStatus *beamStatus, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamStatusBar();

	/// Returns the beam status being viewed.
	BioXASBeamStatus* beamStatus() const { return beamStatus_; }

	/// Returns the list of components being viewed.
	QList<AMControl*> components() const { return controlButtonMap_.keys(); }
	/// Returns true if the beam status bar has a view for the given component.
	bool hasComponentView(AMControl *component) const { return controlButtonMap_.contains(component); }

signals:
	/// Notifier that the beam status being viewed has changed.
	void beamStatusChanged(BioXASBeamStatus *newStatus);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the beam status being viewed.
	void setBeamStatus(BioXASBeamStatus *newStatus);

	/// Adds a component to the view.
	void addComponentView(AMControl *newControl);
	/// Removes a component from the view.
	void removeComponentView(AMControl *control);
	/// Clears all components and views.
	void clearComponentViews();

protected slots:
	/// Handles updating the button color of the button corresponding to the given component.
	void updateButtonForComponent(QObject *component);

protected:
	/// Creates and returns a button suitable for viewing the given control.
	QAbstractButton* createControlButton(AMControl *control) const;
	/// Creates and returns a view suitable for displaying information about the given control.
	QWidget* createControlView(AMControl *control) const;

protected:
	/// The beam status being viewed.
	BioXASBeamStatus *beamStatus_;

	/// The button editor bar.
	BioXASButtonEditorBar *buttonBar_;

	/// The mapping between control and button.
	QMap<AMControl*, QAbstractButton*> controlButtonMap_;
	/// Control value update signal mapper.
	QSignalMapper *valueUpdateMapper_;
};

#endif // BIOXASBEAMSTATUSBAR_H
