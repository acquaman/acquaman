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
    void editPV(const QModelIndex &index);
    void deletePV(const QModelIndex &index);
    void setPVUpdating(const QModelIndex &index, bool isUpdating);
    void incrementValuesDisplayed(const QModelIndex &index, int difference);
    void setAllValuesDisplayed(const QModelIndex &index);

protected:
    StripToolModel *model_;

    QAction *edit_;
    QAction *delete_;
    QAction *showLess_;
    QAction *showMore_;
    QAction *showAll_;
    QAction *pause_;
    QAction *resume_;

public:
    void setPVModel(StripToolModel *model);
    
protected:
    void createActions();

protected slots:
    void updateContextMenu(const QPoint &position);
    void editSelection();
    void deleteSelection();
    void showLessSelection();
    void showMoreSelection();
    void showAllSelection();
    void pauseSelection();
    void resumeSelection();
};

#endif // STRIPTOOLLISTVIEW_H
