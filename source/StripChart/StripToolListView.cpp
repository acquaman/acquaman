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


#include "StripToolListView.h"

StripToolListView::StripToolListView(QWidget *parent) :
	QListView(parent)
{
	edit_ = 0;
	remove_ = 0;
	delete_ = 0;
	color_ = 0;

	buildComponents();
	makeConnections();
	defaultSettings();

	qDebug() << "StripToolListView object created.";
}



StripToolListView::~StripToolListView()
{
}



QModelIndex StripToolListView::selectedIndex() const
{
	if (selectionModel()->selectedIndexes().size() > 0)
		return selectionModel()->selectedIndexes().at(0);
	else
		return QModelIndex();
}



void StripToolListView::setSelectedIndex(const QModelIndex &newSelection)
{
	if (newSelection != selectedIndex()) {

		// an empty QModelIndex indicates something has been deselected elsewhere.
		if (newSelection == QModelIndex()) {
			selectionModel()->clearSelection();
			qDebug() << "StripToolListView :: List item deselected.";

		} else if (newSelection.isValid()) {
			selectionModel()->select(QItemSelection(newSelection, newSelection), QItemSelectionModel::ClearAndSelect);
			qDebug() << "StripToolListView :: Index " << newSelection.row() << " selected.";
		}
	}
}



QColor StripToolListView::colorPicker()
{
	QColorDialog colors;
	QColor color = colors.getColor();
	return color;
}



void StripToolListView::updateContextMenu(const QPoint &position)
{
	QMenu menu(this);

	menu.addAction(edit_);
	menu.addAction(remove_);
	menu.addAction(delete_);
	menu.addSeparator();
	menu.addAction(importAutomatically_);
	menu.addSeparator();
	menu.addAction(plotDerivative_);
	menu.addSeparator();
	menu.addAction(color_);

	menu.exec(mapToGlobal(position));
}



void StripToolListView::editSelection()
{
	foreach (const QModelIndex &index, selectionModel()->selectedIndexes()) {
		emit itemToEdit(index);
	}
}



void StripToolListView::removeSelection()
{
	foreach (const QModelIndex &index, selectionModel()->selectedIndexes()) {
		emit itemToRemove(index);
	}
}



void StripToolListView::deleteSelection()
{
	foreach(const QModelIndex &index, selectionModel()->selectedIndexes()) {
		emit itemToDelete(index);
	}
}



void StripToolListView::colorSelection()
{
	QColor newColor = colorPicker();

	foreach(const QModelIndex &index, selectionModel()->selectedIndexes())
		emit itemToColor(index, newColor);
}



void StripToolListView::findSelectionDerivative()
{
	foreach( const QModelIndex &index, selectionModel()->selectedIndexes()) {
		emit itemToFindDerivative(index);
	}
}



void StripToolListView::onSelectionChanged(QItemSelection newSelection, QItemSelection oldSelection)
{
	if (newSelection == oldSelection) {
		return;
	}

	QList<QModelIndex> selectedIndices = newSelection.indexes();
	emit listSelectionChanged(selectedIndices.at(0));
}



void StripToolListView::buildComponents()
{
	edit_ = new QAction("Edit", this);
	remove_ = new QAction("Remove", this);
	delete_ = new QAction("Delete", this);
	importAutomatically_ = new QAction("Automatic import", this);
	importAutomatically_->setEnabled(false);
	color_ = new QAction("Line color", this);
	plotDerivative_ = new QAction("Plot derivative", this);
	/*
	plotDerivative_->setEnabled(false);
	*/
}



void StripToolListView::makeConnections()
{
	connect( edit_, SIGNAL(triggered()), this, SLOT(editSelection()) );
	connect( remove_, SIGNAL(triggered()), this, SLOT(removeSelection()) );
	connect( delete_, SIGNAL(triggered()), this, SLOT(deleteSelection()) );
	connect( color_, SIGNAL(triggered()), this, SLOT(colorSelection()) );
	connect( plotDerivative_, SIGNAL(triggered()), this, SLOT(findSelectionDerivative()) );

	connect( this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(updateContextMenu(const QPoint &)) );
}



void StripToolListView::defaultSettings()
{
	setViewMode(QListView::ListMode);
	setSelectionMode(QListView::SingleSelection);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	setMaximumWidth(200);
	setContextMenuPolicy(Qt::CustomContextMenu);
}
