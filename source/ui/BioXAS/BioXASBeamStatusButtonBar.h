#ifndef BIOXASBEAMSTATUSBUTTONBAR_H
#define BIOXASBEAMSTATUSBUTTONBAR_H

#include <QWidget>
#include <QLayout>
#include <QButtonGroup>

#include "beamline/BioXAS/BioXASBeamStatus.h"
#include "ui/CLS/CLSButtonBar.h"
#include "ui/beamline/AMControlToolButton.h"

class BioXASBeamStatusButtonBar : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASBeamStatusButtonBar(BioXASBeamStatus *beamStatus, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamStatusButtonBar();

	/// Returns the beam status being viewed.
	BioXASBeamStatus* beamStatus() const { return beamStatus_; }
	/// Returns the selected component.
	AMControl* selectedComponent() const { return selectedComponent_; }

signals:
	/// Notifier that the beam status being viewed has changed.
	void beamStatusChanged(BioXASBeamStatus *newBeamStatus);
	/// Notifier that the selected component has changed.
	void selectedComponentChanged(AMControl *newSelection);
	/// Notifier that the button corresponding to the given component was clicked.
	void componentClicked(AMControl *clickedComponent);

public slots:
	/// Sets the beam status being viewed.
	void setBeamStatus(BioXASBeamStatus *newBeamStatus);

	/// Sets the selected component.
	void setSelectedComponent(AMControl *control);

protected slots:
	/// Updates the selected component.
	void updateSelectedComponent();

	/// Updates the whole button bar.
	void updateButtonBar();
	/// Updates the button bar selected button.
	void updateButtonBarSelectedButton();

	/// Handles emitting the 'componentClicked' signal in response to the button bar signaling a button was clicked.
	void onButtonBarButtonClicked(QAbstractButton *clickedButton);

protected:
	/// Creates and returns a new control button.
	virtual QAbstractButton* createButton(AMControl *control);

	/// Returns the color state associated with the given beam status state.
	virtual AMToolButton::ColorState getColorState(BioXASBeamStatus::Value beamStatusValue) const;

	/// Returns the component that corresponds to the given button. Returns 0 if no match is found.
	virtual AMControl* getComponentForButton(QAbstractButton *button) const;
	/// Returns the button that corresponds to the given component. Returns 0 if no match is found.
	virtual QAbstractButton* getButtonForComponent(AMControl *control) const;

protected:
	/// The beam status.
	BioXASBeamStatus *beamStatus_;
	/// The selected component.
	AMControl *selectedComponent_;
	/// The mapping between components and buttons.
	QMap<AMControl*, QAbstractButton*> componentButtonMap_;

	/// The button bar.
	CLSButtonBar *buttonBar_;
};

#endif // BIOXASBEAMSTATUSBUTTONBAR_H
