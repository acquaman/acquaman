#ifndef STRIPTOOLLISTVIEW_H
#define STRIPTOOLLISTVIEW_H

#include <QListView>
#include <QtGui>

#include "StripChart/StripToolModel.h"

/// This class displays the variables added by the user in list form. Each list entry is displays information about a single pv (either its name or description, if entered) and is checkable. The user can edit the pv's description by double clicking on it.

class StripToolListView : public QListView
{
    Q_OBJECT

public:
    explicit StripToolListView(QWidget *parent = 0);
    ~StripToolListView();
    
signals:
    void itemToEdit(const QModelIndex &index);
    void itemToRemove(const QModelIndex &index);
    void itemToDelete(const QModelIndex &index);
    void itemToColor(const QModelIndex &index, const QColor &color);
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
