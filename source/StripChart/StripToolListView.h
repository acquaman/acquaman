#ifndef STRIPTOOLLISTVIEW_H
#define STRIPTOOLLISTVIEW_H

#include <QListView>
#include <QtGui>

#include "StripChart/StripToolModel.h"

/// This class displays the pvs added by the user in list form. Each list entry is displays information about a single pv (either its name or description, if entered) and is checkable. The user can edit the pv's description by double clicking on it.

class StripToolListView : public QListView
{
    Q_OBJECT

public:
    explicit StripToolListView(QWidget *parent = 0);
    ~StripToolListView();
    
signals:
    void setSelected(QItemSelection, QItemSelectionModel::SelectionFlags);
    void setDeselected(QModelIndex, QItemSelectionModel::SelectionFlags);
    void editPV(QList<QModelIndex> indicesToEdit);
    void deletePV(const QModelIndex &index);
    void setPVUpdating(const QModelIndex &index, bool isUpdating);
    void incrementValuesDisplayed(const QModelIndex &index, int difference);
    void colorPV(const QModelIndex &index, const QColor &color);

protected:
    StripToolModel *model_;
    QModelIndex modelSelection_;

    QAction *edit_;
    QAction *delete_;
    QAction *showLess_;
    QAction *showMore_;
    QAction *pause_;
    QAction *resume_;
    QAction *setColor_;

public:
    /// This function sets the view's model and makes sure all the connections are correct.
    void setPVModel(StripToolModel *model);
    
protected:
    /// Creates the actions that a user can use to manipulate a pv: edit, delete, show less, show more, pause, resume.
    void createActions();
    /// To view a color picker.
    QColor colorPicker();

protected slots:
    /// Creates the menu that appears when a user right-clicks on a pv, displays the available actions.
    void updateContextMenu(const QPoint &position);
    /// Emits the editPV(QList<QModelIndex>) signal, letting the model know that items need to be edited.
    void editSelection();
    /// Emits the deletePV(QModelIndex) signal, letting the model know that this pv needs to be removed.
    void deleteSelection();
    /// Emits the incrementValuesDisplayed(QModelIndex, int) signal, letting the model know that the number of values displayed for a given pv should be decreased by a given amount.
    void showLessSelection();
    /// Emits the incrementValuesDisplayed(QModelIndex, int) signal, letting the model know that the number of values displayed for a given pv should be increased by a given amount.
    void showMoreSelection();
    /// Emits the setPVUpdating(QModelIndex, false) signal, letting the model know that the given pv's series should not be updated.
    void pauseSelection();
    /// Emits the setPVUpdating(QModelIndex, true) signal, letting the model know that the given pv's series should be updating.
    void resumeSelection();
    /// An attempt to get the plot's selection to dictate which pv is selected in the view--not fully implemented.
    void setViewSelection();
    /// Sets the series color for all of the selected pvs, using the color selected from color picker.
    void toSetPVColor();

};

#endif // STRIPTOOLLISTVIEW_H
