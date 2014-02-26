#include "StripTool.h"

StripTool::StripTool(QWidget *parent) : QWidget(parent)
{
    model_ = 0;
    mainView_ = 0;
    dataController_ = 0;

    buildComponents();
    defaultSettings();

    connect( model(), SIGNAL(modelSelectionChange()), this, SLOT(onModelSelectionChanged()) );
    connect( model(), SIGNAL(selectedVariableInfoChanged()), this, SLOT(onModelSelectionInfoChanged()) );
    connect( model(), SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(onModelItemAdded(QModelIndex,int,int)) );
    connect( model(), SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), this, SLOT(onModelItemRemoved(QModelIndex, int, int)) );

    connect( mainView(), SIGNAL(viewSelectionChange()), this, SLOT(onViewSelectionChange()) );

    QVBoxLayout *windowLayout = new QVBoxLayout();
    windowLayout->addWidget(mainView_);

    setLayout(windowLayout);
    resize(800, 500);
}



StripTool::~StripTool() {
}



StripToolModel* StripTool::model() const {
    return model_;
}



StripToolView* StripTool::mainView() const {
    return mainView_;
}



void StripTool::buildComponents() {
    model_ = new StripToolModel(this);

    mainView_ = new StripToolView(this, model_);

    dataController_ = new StripToolDataController(this);
    dataController_->setModel(model_);
}



void StripTool::defaultSettings() {
    mainView()->setPlotBottomAxisName("Time []");
    mainView()->setPlotLeftAxisName("");
}



void StripTool::onModelItemAdded(const QModelIndex &parent, int rowStart, int rowFinish) {
    Q_UNUSED(parent)

    // the list view handles adding new items automatically.

    // but we need to deal with the plot view ourselves!
    // add each new item to the plot.

    bool addComplete = false;
    int row = rowStart;

    while (!addComplete && row <= rowFinish) {
        mainView()->addPlotItem(model()->plotItem(row));
        row++;

        qDebug() << "StripTool :: one item added.";
    }
}



void StripTool::onModelItemRemoved(const QModelIndex &parent, int rowStart, int rowFinish) {
    Q_UNUSED(parent)

    // the lsit view handles removing old items automatically.

    // but we need to deal with the plot view ourselves!
    // remove each item from the plot.

    for (int row = rowStart; row < rowFinish; row++) {
        mainView()->removePlotItem(model()->plotItem(row));
    }
}



void StripTool::onModelSelectionChanged() {
    // update the plot and list view's selection.
    MPlotItem *selectedItem = model()->selectedPlotItem();
    QModelIndex selectedIndex = model()->selectedIndex();

    if (selectedItem && selectedIndex != QModelIndex()) {
        QString leftAxisName = model()->selectedDescription() + " [" + model()->selectedUnits() + "]";
        mainView()->setPlotLeftAxisName(leftAxisName);
        mainView()->setPlotSelection(selectedItem);
        mainView()->setListSelection(selectedIndex);
    }
}



void StripTool::onPlotSelectionChanged(MPlotItem* newSelection) {
    // update the list view and the model's selection.
    model()->setSelectedVariable(newSelection);
}



void StripTool::onListSelectionChanged (const QModelIndex &newSelection) {
    // update the plot and model's selection.
    model()->setSelectedVariable(newSelection);
}



void StripTool::onModelSelectionInfoChanged() {

}



