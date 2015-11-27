#ifndef SGMSAMPLECHAMBERVACUUMMOVEACTIONVIEW_H
#define SGMSAMPLECHAMBERVACUUMMOVEACTIONVIEW_H

#include <QWidget>
#include <QLayout>
#include <QLabel>

#include "ui/actions3/AMActionInfoDescriptionView.h"
#include "ui/actions3/AMActionCountdownView.h"

class SGMSampleChamberVacuumMoveAction;

class SGMSampleChamberVacuumMoveActionStatusView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	SGMSampleChamberVacuumMoveActionStatusView(SGMSampleChamberVacuumMoveAction *action, QWidget *parent = 0);
	/// Destructor.
	virtual ~SGMSampleChamberVacuumMoveActionStatusView();

	/// Returns the action being viewed.
	SGMSampleChamberVacuumMoveAction* action() const { return action_; }

signals:
	/// Notifier that the action being viewed has changed.
	void actionChanged(SGMSampleChamberVacuumMoveAction *newAction);

public slots:
	/// Clears the view.
	void clear();
	/// Refreshes the view.
	void refresh();

	/// Sets the action being viewed.
	void setAction(SGMSampleChamberVacuumMoveAction *newAction);

protected slots:
	/// Updates the status label.
	void updateStatusLabel();

protected:
	/// Returns a string representation of the given action state.
	QString stateToString(AMAction3::State state) const;

protected:
	/// The action being viewed.
	SGMSampleChamberVacuumMoveAction *action_;

	/// The status label.
	QLabel *statusLabel_;
};

//

class SGMSampleChamberVacuumMoveActionView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit SGMSampleChamberVacuumMoveActionView(SGMSampleChamberVacuumMoveAction *action, QWidget *parent = 0);
	/// Destructor.
	virtual ~SGMSampleChamberVacuumMoveActionView();

	/// Returns the action being viewed.
	SGMSampleChamberVacuumMoveAction* action() const { return action_; }

signals:
	/// Notifier that the action being viewed has changed.
	void actionChanged(SGMSampleChamberVacuumMoveAction *newAction);

public slots:
	/// Clears the view.
	void clear();
	/// Refreshes the view.
	void refresh();

	/// Sets the action being viewed.
	void setAction(SGMSampleChamberVacuumMoveAction *newAction);

protected slots:
	/// Updates the action info description view.
	void updateDescriptionView();
	/// Updates the action status view.
	void updateStatusView();
	/// Updates the action countdown view.
	void updateCountdownView();

protected:
	/// The action being viewed.
	SGMSampleChamberVacuumMoveAction *action_;

	/// The current subaction info description view.
	AMActionInfoDescriptionView *descriptionView_;
	/// The view for the action's current state.
	SGMSampleChamberVacuumMoveActionStatusView *statusView_;
	/// The current subaction countdown view.
	AMActionCountdownView *countdownView_;
};

#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>

class SGMSampleChamberVacuumMoveActionDialog : public QDialog
{
	Q_OBJECT

public:
	/// Constructor.
	SGMSampleChamberVacuumMoveActionDialog(SGMSampleChamberVacuumMoveAction *action, QWidget *parent = 0);
	/// Destructor.
	virtual ~SGMSampleChamberVacuumMoveActionDialog();

signals:
	/// Notifier that the action being viewed has changed.
	void actionChanged(SGMSampleChamberVacuumMoveAction *newAction);

public slots:
	/// Clears the view.
	void clear();
	/// Refreshes the view.
	void refresh();

	/// Sets the action being viewed.
	void setAction(SGMSampleChamberVacuumMoveAction *newAction);

protected slots:
	/// Updates the move action view.
	void updateMoveActionView();
	/// Updates the buttons.
	void updateButtons();
	/// Updates the visibility of the OK button.
	void updateOKButton();
	/// Updates the visibility of the Cancel button.
	void updateCancelButton();

	/// Handles situation when the OK button is clicked.
	void onOKButtonClicked();
	/// Handles situation when the Cancel button is clicked.
	void onCancelButtonClicked();

protected:
	/// The action being viewed.
	SGMSampleChamberVacuumMoveAction *action_;

	/// The move action view.
	SGMSampleChamberVacuumMoveActionView *moveView_;
	/// The OK button.
	QPushButton *okButton_;
	/// The Cancel button.
	QPushButton *cancelButton_;
	/// The dialog button box.
	QDialogButtonBox *buttonBox_;
};

#endif // SGMSAMPLECHAMBERVACUUMMOVEACTIONVIEW_H
