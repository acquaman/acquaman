#ifndef SGMSAMPLECHAMBERVACUUMMOVEACTIONVIEW_H
#define SGMSAMPLECHAMBERVACUUMMOVEACTIONVIEW_H

#include <QWidget>
#include <QLayout>
#include <QLabel>

#include "ui/actions3/AMTimeoutActionView.h"

class SGMSampleChamberVacuumMoveAction;

class SGMSampleChamberVacuumMoveSubActionView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit SGMSampleChamberVacuumMoveSubActionView(AMAction3 *action, QWidget *parent = 0);
	/// Destructor.
	virtual ~SGMSampleChamberVacuumMoveSubActionView();

	/// Returns the action being viewed.
	AMAction3* action() const { return action_; }

signals:
	/// Notifier that the action being viewed has changed.
	void actionChanged(AMAction3 *newAction);

public slots:
	/// Clears the view.
	void clear();
	/// Refreshes the view.
	void refresh();

	/// Sets the action being viewed.
	void setAction(AMAction3 *newAction);

protected slots:
	/// Updates the action description label.
	void updateDescriptionLabel();

protected:
	/// The action being viewed.
	AMAction3 *action_;

	/// The action description label.
	QLabel *descriptionLabel_;
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
	/// Updates the currently displayed subaction view.
	void updateSubActionView();
	/// Updates the OK button.
	void updateOKButton();
	/// Updates the Cancel button.
	void updateCancelButton();

	/// Creates and returns a subaction view appropriate for viewing the given subaction.
	QWidget *createSubActionView(AMAction3 *subAction);

	/// Adds a subaction view to the list of views and the views layout, if that view is unique.
	void addSubActionView(QWidget *newView);

	/// Hides all subaction views.
	void hideAllSubActionViews();
	/// Removes all subaction views from the layout, but doesn't delete them.
	void removeAllSubActionViews();
	/// Deletes all subaction views, empties the subaction views list.
	void deleteAllSubActionViews();

protected:
	/// Returns the view for the corresponding action index.
	QWidget* viewForActionIndex(int index);

protected:
	/// The action being viewed.
	SGMSampleChamberVacuumMoveAction *action_;

	/// The list of subaction views.
	QList<QWidget*> subActionViews_;
	/// The subaction views layout.
	QVBoxLayout *subActionViewsLayout_;
	/// The OK button.
	QPushButton *okButton_;
	/// The Cancel button.
	QPushButton *cancelButton_;
};

#endif // SGMSAMPLECHAMBERVACUUMMOVEACTIONVIEW_H
