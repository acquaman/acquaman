#ifndef STRIPTOOL_H
#define STRIPTOOL_H

#include "StripChart/StripToolModel.h"
#include "StripChart/StripToolDataController.h"
#include "StripChart/StripToolView.h"

/// This class is the basic application class for StripTool: it creates an instance of the model and passes it along to a new instance of the application view.

class StripToolBasicVariable;

class StripTool : public QWidget
{
    Q_OBJECT

public:
    explicit StripTool(QWidget *parent = 0);
    ~StripTool();

signals:

protected:
    StripToolModel* model() const;
    StripToolView* mainView() const;

private:
    void buildComponents();
    void defaultSettings();

private slots:
    void onModelItemAdded(const QModelIndex &parent, int rowStart, int rowFinish);
    void onModelItemRemoved(const QModelIndex &parent, int rowStart, int rowFinish);
    void onModelSelectionChanged();
    void onPlotSelectionChanged(MPlotItem *newSelection);
    void onListSelectionChanged(const QModelIndex &newSelection);
    void onModelSelectionInfoChanged();
    
private:
    StripToolModel *model_;
    //StripToolVariableImporter *importer_;
    //StripToolVariableInfoEditor *editor_;
    //StripToolDataSaver *dataSaver_;
    StripToolDataController *dataController_;
    StripToolView *mainView_;
    
};

#endif // STRIPTOOL_H
