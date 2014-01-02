#ifndef STRIPTOOL_H
#define STRIPTOOL_H

#include "StripChart/StripToolModel.h"
#include "StripChart/StripToolDataController.h"
#include "StripChart/StripToolView.h"

/// This class is the basic application class for StripTool: it creates an instance of the model and passes it along to a new instance of the application view.

class StripTool : public QWidget
{
    Q_OBJECT

public:
    explicit StripTool(QWidget *parent = 0);
    virtual ~StripTool();
    
protected:
    StripToolModel *model_;
    StripToolDataController *dataController_;
    StripToolView *mainView_;
    
};

#endif // STRIPTOOL_H
