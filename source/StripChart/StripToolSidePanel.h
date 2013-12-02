#ifndef STRIPTOOLSIDEPANEL_H
#define STRIPTOOLSIDEPANEL_H

#include <QWidget>
#include <QtGui>

#include "StripChart/StripToolModel.h"
#include "StripChart/StripToolListView.h"

/// This class is a sort of optional sidebar that provides a number of ways for the user to interact with an added pv, as well as a quick way to add a new one.
class StripToolSidePanel : public QWidget
{
    Q_OBJECT

public:
    explicit StripToolSidePanel(QWidget *parent = 0);
    ~StripToolSidePanel();
    
signals:
    void pausePVs();
    void resumePVs();

protected:
    StripToolModel *model_;
    StripToolListView *listView_;

public:
    void setModel(StripToolModel *newModel);

};

#endif // STRIPTOOLSIDEPANEL_H
