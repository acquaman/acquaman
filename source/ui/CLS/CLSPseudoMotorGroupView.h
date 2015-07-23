/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef CLSPSEUDOMOTORGROUPVIEW_H
#define CLSPSEUDOMOTORGROUPVIEW_H

#include "ui/AMMotorGroupView.h"
#include "beamline/CLS/CLSPseudoMotorGroup.h"

class CLSPseudoMotorGroupObjectView : public AMMotorGroupObjectView
{
	Q_OBJECT

public:
	explicit CLSPseudoMotorGroupObjectView(CLSPseudoMotorGroupObject* motorGroupObject,
										   QWidget* parent = 0);

	virtual ~CLSPseudoMotorGroupObjectView() {}
};


class CLSPseudoMotorGroupView : public AMMotorGroupView
{
	Q_OBJECT
public:
	explicit CLSPseudoMotorGroupView(CLSPseudoMotorGroup* motorGroup,
									 AMMotorGroupView::ViewMode viewMode,
									 QWidget* parent = 0);

	virtual ~CLSPseudoMotorGroupView() {}
};

///// This class extends the AMMotorGroupObjectView for the CLS pseudo motor application.
//class CLSPseudoMotorGroupObjectView : public AMMotorGroupObjectView
//{
//	Q_OBJECT

//public:
//	/// Constructor.  Builds a view based around the provided \param motorGroupObject.
//	explicit CLSPseudoMotorGroupObjectView(CLSPseudoMotorGroupObject *motorGroupObject, QWidget *parent = 0);
//	/// Destructor.
//	virtual ~CLSPseudoMotorGroupObjectView();
//};

///// This class acts as a simple extension that enables the use of reseting the pseudo-motors.
///*!
//	There are two distinct behaviours at the moment.  If in Exclusive view then
//	the currently viewed motor will be reset, whereas in the Multiple view every
//	motor that is visible will be reset.  Obviously, since CLSPseudoMotorGroupObjects
//	and AMMotorGroupObjects can be a part of a CLSPseudoMotorGroupView, only
//	CLSPseudoMotorGroupObjects will provide the ability to be reset.
//  */
//class CLSPseudoMotorGroupView : public AMMotorGroupView
//{
//	Q_OBJECT

//public:
//	/// Constructor.  Takes an AMMotorGroup since there was no need to subclass it.
//	/// \note Maybe I should subclass AMMotorGroup for consistency in naming?
//	CLSPseudoMotorGroupView(CLSPseudoMotorGroup *motorGroup, QWidget *parent = 0);
//	/// Constructor.  Takes an AMMotorGroup since there was no need to subclass it.
//	CLSPseudoMotorGroupView(CLSPseudoMotorGroup *motorGroup, ViewMode viewMode, QWidget *parent = 0);
//	/// Destructor.
//	virtual ~CLSPseudoMotorGroupView();

//protected slots:
//	/// Handles the popup menu that allows you to change the motors you want to change.
//	virtual void onCustomContextMenuRequested(const QPoint &pos);
//	/// Handles checking if the new exclusive view needs to emit the pseudo motor recommendation.
//	void onNewExclusiveViewMotorGroupViewChange(const QString &name);
//	/// Handles checking if the new selected motor needs to be zeroed.  Only checks if it is visible since if it's not visible then it won't be used.
//	void onNewMultipleViewMotorGroupViewChange(const QString &name);

//protected:
//	/// Adds the additional options to the popup menu.
//	void buildCLSPseudoMotorGroupMenuItems(QMenu *menu);
//	/// Handles the additional options of the popup menu.
//	void resetCLSPseudoMotorGroupMenuItems(const QString &command);
//	/// Pops up a message when switching to a new CLSPseudoMotorGroupObject, recommending reseting the object before using it.
//	void recommendResettingCLSPseudoMotorGroupObject(CLSPseudoMotorGroupObject *motorObject);

//	/// An initialize flag.  Required to stop the intialization (choosing the first motor group view programmatically) happens during startup.  Maybe a better solution exists, but this will do for now.
//	bool initialized_;
//};

#endif // CLSPSEUDOMOTORGROUPVIEW_H
