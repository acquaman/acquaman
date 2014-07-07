#ifndef STLISTVIEW_H
#define STLISTVIEW_H

#include <QListView>
#include <QMenu>
#include <QAction>

class STListView : public QListView
{
    Q_OBJECT
public:
    explicit STListView(QWidget *parent = 0);
    virtual ~STListView();

signals:
    void addVariable();

protected slots:
    virtual void onCustomContextMenuRequested(QPoint position);

};

#endif // STLISTVIEW_H
