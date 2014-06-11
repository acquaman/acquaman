#ifndef STRIPTOOL_H
#define STRIPTOOL_H

#include "source/analysis/AM1DDerivativeAB.h"

#include "StripChart/StripToolModel.h"
#include "StripChart/StripToolDataController.h"
#include "StripChart/StripToolView.h"
#include "StripChart/StripToolVariableInfoEditor.h"
#include "StripChart/StripToolVariableInfoImporter.h"
#include "StripChart/StripToolVariableInfoExporter.h"

/// This class is the basic application class for StripTool: it creates an instance of the model and passes it along to a new instance of the application view.

class StripToolBasicVariable;

class StripTool : public QWidget
{
    Q_OBJECT

public:
    explicit StripTool(QWidget *parent = 0);
    virtual ~StripTool();

signals:

public:
    StripToolModel* model() const;
    StripToolView* mainView() const;
    StripToolVariableInfoEditor* editor() const;
    StripToolVariableInfoImporter* importer() const;
    StripToolVariableInfoExporter* exporter() const;
    QDir appDirectory() const;
    QDir infoImportDirectory() const;
    QDir infoExportDirectory() const;
    QDir dataExportDirectory() const;


public slots:
    void setAppDirectory(const QDir &newDir);

protected:
    StripToolVariable* createDerivativeFromVariable(StripToolVariable *variable);


protected slots:
    void onModelItemAdded(const QModelIndex &parent, int rowStart, int rowFinish);
    void onModelItemRemoved(const QModelIndex &parent, int rowStart, int rowFinish);
    void onModelSelectionChanged();
    void onPlotSelectionChanged(MPlotItem *plotSelection);
    void onListSelectionChanged(const QModelIndex &listSelection);
    void onModelSelectionInfoChanged();
    void toImportVariables();
    void toExportVariables(StripToolVariableInfo* toExport);
    void toEditListItem(const QModelIndex &listItem);
    void toRemoveListItem(const QModelIndex &listItem);
    void toDeleteListItem(const QModelIndex &listItem);
    void toAddDerivativeListItem(const QModelIndex &listItem);
    void onModelVariableCheckStateChanged(const QModelIndex &index);
    void addVariableToModel(const QString &name);
    void onTimeUnitsChanged(TimeEntryWidget::TimeUnits units);
    void toChangeVariableColor(const QModelIndex &index, const QColor &newColor);

private:
    void buildComponents();
    void makeConnections();
    void defaultSettings();
    
private:
    QDir appDirectory_;

    StripToolModel *model_;
    StripToolView *mainView_;

    StripToolVariableInfoImporter *importer_;
    StripToolVariableInfoExporter *exporter_;
    StripToolVariableInfoEditor *editor_;
    //StripToolVariableDataSaver *dataSaver_;


};

#endif // STRIPTOOL_H
