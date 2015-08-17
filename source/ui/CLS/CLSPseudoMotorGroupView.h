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

/*!
  * A view which displays information related to a CLSPseudoMotorGroup. Displays
  * information in a similar manner to a regular AMMotorGroupView, but gives the
  * option to reset the pseudo motors before use.
  */
class CLSPseudoMotorGroupView : public AMMotorGroupView
{
	Q_OBJECT
public:
	/*!
	  * Creates an instance of a CLSPseudoMotorGroupView, which will visualize
	  * the data for the provided pseudo motor group.
	  * \param motorGroup ~ The motor group model whose data the view will visualize.
	  * \param viewMode ~ The layout style of the view.
	  */
	explicit CLSPseudoMotorGroupView(CLSPseudoMotorGroup* motorGroup,
									 AMMotorGroupView::ViewMode viewMode,
									 QWidget* parent = 0);

	/*!
	  * Virtual destructor for the motor group view.
	  */
	virtual ~CLSPseudoMotorGroupView() {}

protected slots:
	/*!
	  * Handles signals indicating that a custom context menu has been requested.
	  * Gives the user the option to reset the current visible motor group.
	  * /param pos ~ The position at which the context menu was requested.
	  */
	virtual void onCustomContextMenuRequested(const QPoint& pos);

	/*!
	  * Handles signals indicating the the current visible motor group object has
	  * been changed.
	  * \param groupObjectName ~ The name of the now visible motor group object.
	  */
	void onGroupObjectViewChanged(const QString& groupObjectName);

protected:
	/*!
	  * Helper method which builds the context menu items.
	  * \param menu ~ A reference to the context menu which is being built.
	  */
	void buildCLSPseudoMotorGroupMenuItems(QMenu *menu);

	/*!
	  * Helper method which resets the current visible motor group object, if it
	  * is a pseudo motor group object.
	  */
	void resetCLSPseudoMotorGroupMenuItem();

	/*!
	  * Helper method which displays a warning message to users which indicates
	  * that it is recommended for them to reset the pseudomotor before using it
	  * and provides for them an option for doing so easily.
	  * \param motorObject ~ The pseudo motor group object which it is recommended
	  * the user reset.
	  */
	void recommendResettingCLSPseudoMotorGroupObject(CLSPseudoMotorGroupObject *motorObject);

	bool isInitialized_;
};

#endif // CLSPSEUDOMOTORGROUPVIEW_H
