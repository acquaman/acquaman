/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMREGIONSVIEW_H
#define AMREGIONSVIEW_H

#include <QWidget>
#include <QtGui>

class QTableView;
class QPushButton;
class QHBoxLayout;
class QMenu;

#include "acquaman/AMRegionsList.h"

class AMRegionsStaticView : public QWidget
{
Q_OBJECT
public:
	explicit AMRegionsStaticView(AMRegionsList *regions, QWidget *parent = 0);
	virtual ~AMRegionsStaticView();

protected:
	/// The table view used to interact with the group of AMXASRegion.
	QTableView *tableView_;
	/// Vertical main layout
	QVBoxLayout *mainVL_;

	/// The model created from the AMRegion group to be used with the table view.
	AMRegionsList *regions_;
};

/// An AMRegionsView is a means of generating a default view for a group of AMRegion
/*!
  This default view utilizes a QTableView to interface with a group of AMRegion.
  By calling the constructor while passing a QList of AMRegion, the AMRegionsView:
  - creates add region and delete region buttons; and, \todo Hook these buttons up to something
  - uses the QList of AMRegion to make an AMRegionModel (the programmer need not do this nor understand how it works); and,
  - creates a table view on the newly created model.
  */
class AMRegionsView : public AMRegionsStaticView
{
Q_OBJECT
public:
	/// Constructor
	/*!
	  \param regions Pointer to a QList of AMXASRegions. Can be easily retreived using regionsPtr() function on an AMXASScanConfiguration or child.
	  \param parent Pointer to QWidget to act as parent.
	  */
	explicit AMRegionsView(AMRegionsList *regions, QWidget *parent = 0);
	virtual ~AMRegionsView();

signals:
	/// Notifier that the add region button has been clicked.
	void addRegionClicked();

public slots:
	/// Slot that builds a popup menu to determine where you want to add another region.  Sets up the necessary connections such that the model is informed with the choice.
	bool addRegion();
	/// Slot that builds a popup menu to determine which region should be removed.  Sets up the necessary connections to pass on the choice to the model.
	bool deleteRegion();
	/// Sets the index where the model needs to insert the new region.
	void setInsertIndex();
	/// Sets the index where the model needs to remove a region.
	void setRemoveIndex();

	/// Slot that adds a region to the model.
	bool insertRegion() { return regions_->addRegionSqueeze(insertIndex_); }
	/// Slot that removes a region from the model.
	bool removeRegion() { return regions_->deleteRegionSqueeze(removeIndex_); }

	/// Slot that disables the add and remove region buttons.
	void setDisabled(bool disabled);

protected:
	/// Reimplemented.  Defines how wide the columns need to be to fit nicely within the current width of the table without needing a horizontal scroll bar.
	void resizeEvent(QResizeEvent *event);

protected:
	/// Horizontal layout for add/delete buttons
	QHBoxLayout *addDeleteHL_;

	/// Button to add a region. \todo Figure out interface and hook up to something.
	QPushButton *addButton_;
	/// Button to delete a region. \todo Figure out interface and hook up to something.
	QPushButton *deleteButton_;
	/// The pointer holding the menu used for creating the add region menu.
	QMenu *addRegionMenu_;
	/// The pointer holding the menu used for creating the remove region menu.
	QMenu *deleteRegionMenu_;
	/// Index used to hold where the model should add another region.
	int insertIndex_;
	/// Index used to tell the model which region needs to be removed.
	int removeIndex_;
};

#endif // AMXASREGIONSVIEW_H
