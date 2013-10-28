#ifndef STRIPTOOLLISTVIEW_H
#define STRIPTOOLLISTVIEW_H

#include <QListView>
#include <QtGui>

#include "StripChart/StripToolModel.h"

class StripToolListView : public QListView
{
    Q_OBJECT

public:
    explicit StripToolListView(QWidget *parent = 0);
    ~StripToolListView();
    friend class StripToolView;
    
signals:
    void newSelection(const QModelIndex, QItemSelectionModel::SelectionFlags);
    void editPV(QList<QModelIndex> indicesToEdit);
    void deletePV(const QModelIndex &index);
    void setPVUpdating(const QModelIndex &index, bool isUpdating);
    void incrementValuesDisplayed(const QModelIndex &index, int difference);
    void save(const QModelIndex &index);

protected:
    StripToolModel *model_;
    QModelIndex *modelSelection_;

    QAction *edit_;
    QAction *delete_;
    QAction *showLess_;
    QAction *showMore_;
    QAction *pause_;
    QAction *resume_;
    QAction *save_;

public:
    void setPVModel(StripToolModel *model);
    
protected:
    void createActions();

protected slots:
    void toSetSelection(const QModelIndex &index);
    void updateContextMenu(const QPoint &position);
    void editSelection();
    void deleteSelection();
    void showLessSelection();
    void showMoreSelection();
    void pauseSelection();
    void resumeSelection();
    void saveSelection();
};

#endif // STRIPTOOLLISTVIEW_H
