#ifndef CLSPSEUDOMOTORGROUPVIEW_H
#define CLSPSEUDOMOTORGROUPVIEW_H

#include "ui/AMMotorGroupView.h"

/// This class acts as a simple extension that enables the use of reseting the pseudo-motors.
/*!
	There are two distinct behaviours at the moment.  If in Exclusive view then
	the currently viewed motor will be reset, whereas in the Multiple view every
	motor that is visible will be reset.  Obviously, since CLSPseudoMotorGroupObjects
	and AMMotorGroupObjects can be a part of a CLSPseudoMotorGroupView, only
	CLSPseudoMotorGroupObjects will provide the ability to be reset.
  */
class CLSPseudoMotorGroupView : public AMMotorGroupView
{
	Q_OBJECT

public:
	/// Constructor.  Takes an AMMotorGroup since there was no need to subclass it.
	/// \note Maybe I should subclass AMMotorGroup for consistency in naming?
	CLSPseudoMotorGroupView(AMMotorGroup *motorGroup, QWidget *parent = 0);
	/// Constructor.  Takes an AMMotorGroup since there was no need to subclass it.
	CLSPseudoMotorGroupView(AMMotorGroup *motorGroup, ViewMode viewMode, QWidget *parent = 0);

protected slots:
	/// Handles the popup menu that allows you to change the motors you want to change.
	virtual void onCustomContextMenuRequested(const QPoint &pos);

protected:
	/// Adds the additional options to the popup menu.
	void buildCLSPseudoMotorGroupMenuItems(QMenu *menu);
	/// Handles the additional options of the popup menu.
	void handleCLSPseudoMotorGroupMenuItems(const QString &command);
};

#endif // CLSPSEUDOMOTORGROUPVIEW_H
