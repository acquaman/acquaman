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
    
protected:
    StripToolModel *model_;
    StripToolView *mainView_;
    
};

#endif // STRIPTOOL_H
