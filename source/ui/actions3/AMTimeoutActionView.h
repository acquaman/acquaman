#ifndef AMTIMEOUTACTIONVIEW_H
#define AMTIMEOUTACTIONVIEW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLayout>

#include "actions3/actions/AMTimeoutAction.h"

class AMTimeoutActionView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMTimeoutActionView(AMTimeoutAction *action, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMTimeoutActionView();

	/// Returns the action being viewed.
	AMTimeoutAction* action() const { return action_; }

signals:
	/// Notifier that the action has changed.
	void actionChanged(AMTimeoutAction *newAction);

public slots:
	/// Clears the view.
	void clear();
	/// Refreshes the view.
	void refresh();

	/// Sets the action being viewed.
	void setAction(AMTimeoutAction *newAction);
	/// Sets the action as succeeded.
	void setActionSucceeded();

protected slots:
	/// Updates the instruction label with the action's current instruction description.
	void updateInstructionLabel();
	/// Updates the remaining time label with the action's current values.
	void updateRemainingTimeLabel();

protected:
	/// The action being viewed.
	AMTimeoutAction *action_;

	/// The label displaying the instructions/desired outcome.
	QLabel *instructionLabel_;
	/// The label displaying the time remaining before the action times out.
	QLabel *remainingTimeLabel_;
};

#endif // AMTIMEOUTACTIONVIEW_H
