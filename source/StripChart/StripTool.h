#ifndef STRIPTOOL_H
#define STRIPTOOL_H

#include <QWidget>

#include <StripChart/StripToolModel.h>
#include <StripChart/StripToolView.h>

/// This class is the basic application class for StripTool: it creates an instance of the model and passes it along to a new instance of the application view.

class StripTool : public QWidget
{
    Q_OBJECT

public:
    explicit StripTool(QWidget *parent = 0);
    ~StripTool();

public:
    QDir appDirectory_;
    QString pvFileName_;
    
protected:
    StripToolModel *model_;
    StripToolView *mainView_;

protected:
    QDir saveDirectory();
    
};

#endif // STRIPTOOL_H
