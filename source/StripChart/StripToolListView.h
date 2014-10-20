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


#ifndef STRIPTOOLLISTVIEW_H
#define STRIPTOOLLISTVIEW_H

#pragma clang diagnostic ignored "-Wunused-private-field"
#include <QtGui>
#pragma clang diagnostic warning "-Wunused-private-field"

#include <QListView>

#include "StripChart/StripToolModel.h"

/// This class displays the variables added by the user in list form. Each list entry is displays information about a single pv (either its name or description, if entered) and is checkable. The user can edit the pv's description by double clicking on it.

class StripToolListView : public QListView
{
    Q_OBJECT

public:
    explicit StripToolListView(QWidget *parent = 0);
    virtual ~StripToolListView();
    
signals:
    void itemToEdit(const QModelIndex &index);
    void itemToRemove(const QModelIndex &index);
    void itemToDelete(const QModelIndex &index);
    void itemToColor(const QModelIndex &index, const QColor &color);
    void itemToFindDerivative(const QModelIndex &index);
    void listSelectionChanged(const QModelIndex &index);

public:
    QModelIndex selectedIndex() const;

public slots:
    void setSelectedIndex(const QModelIndex &newSelection);

protected:
    /// Get new color from a color picker.
    QColor colorPicker();

protected slots:
    /// Creates the menu that appears when a user right-clicks on a pv, displays the available actions.
    void updateContextMenu(const QPoint &position);
    /// Triggered by edit_ action. Emits the itemToEdit(QList<QModelIndex>) signal, letting the model know that items need to be edited.
    void editSelection();
    void removeSelection();
    /// Triggered by delete_ action. Emits the itemToDelete(QModelIndex) signal, letting the model know that this pv needs to be removed.
    void deleteSelection();
    /// Triggered by color_ action. Sets the series color for all of the selected pvs, using the color selected from color picker.
    void colorSelection();
    void findSelectionDerivative();

    void onSelectionChanged(QItemSelection newSelection, QItemSelection oldSelection);

private:
    void buildComponents();
    void makeConnections();
    void defaultSettings();

private:
    QAction *edit_;
    QAction *remove_;
    QAction *delete_;
    QAction *importAutomatically_;
    QAction *color_;
    QAction *plotDerivative_;
};

#endif // STRIPTOOLLISTVIEW_H
