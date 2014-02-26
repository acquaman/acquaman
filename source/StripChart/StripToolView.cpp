#include "StripChart/StripToolView.h"

StripToolView::StripToolView(QWidget *parent, StripToolModel *model) : QWidget(parent) {
    model_ = model;

    plotView_ = 0;
    sidePanel_ = 0;
    controlPanel_ = 0;

    buildUI();
}



StripToolView::~StripToolView() {
}



StripToolModel* StripToolView::model() const {
    return model_;
}



StripToolPlot* StripToolView::plotView() const {
    return plotView_;
}



StripToolSidePanel* StripToolView::sidePanel() const {
    return sidePanel_;
}



StripToolControlsPanel* StripToolView::controlPanel() const {
    return controlPanel_;
}



void StripToolView::buildUI() {
    plotView_ = new StripToolPlot(this);
    plotView()->setModel(model());
    connect( plotView(), SIGNAL(itemSelected(MPlotItem*)), this, SIGNAL(plotSelectionChanged(MPlotItem*)) );
    connect( plotView(), SIGNAL(seriesDeselected()), this, SLOT(onSeriesDeselected()) );

    qDebug() << "StripToolView :: plot should now be fully built.";

    sidePanel_ = new StripToolSidePanel(this);
    sidePanel()->setModel(model());
    sidePanel()->hide();
    connect( sidePanel(), SIGNAL(listSelectionChanged(QModelIndex)), this, SIGNAL(listSelectionChanged(QModelIndex)) );

    QHBoxLayout *upperLayout = new QHBoxLayout();
    upperLayout->addWidget(plotView());
    upperLayout->addWidget(sidePanel());

    controlPanel_ = new StripToolControlsPanel(this);
    controlPanel()->setModel(model());
    connect( controlPanel(), SIGNAL(showSidebar()), sidePanel(), SLOT(show()) );
    connect( controlPanel(), SIGNAL(hideSidebar()), sidePanel(), SLOT(hide()) );

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(upperLayout);
    mainLayout->addWidget(controlPanel());

    setLayout(mainLayout);
}



void StripToolView::setPlotLeftAxisName(const QString &newName) {
    plotView()->setLeftAxisName(newName);
}



void StripToolView::setPlotBottomAxisName(const QString &newName) {
    plotView()->setBottomAxisName(newName);
}



void StripToolView::setPlotSelection(MPlotItem *newSelection) {
    plotView()->setSelectedItem(newSelection);
}



MPlotItem* StripToolView::plotSelection() {
    return plotView()->selectedItem();
}



void StripToolView::addPlotItem(MPlotItem *newItem) {
    qDebug() << "StripToolView :: adding to item to the plot...";
    plotView()->addPlotItem(newItem);
}



void StripToolView::removePlotItem(MPlotItem *oldItem) {
    plotView()->removePlotItem(oldItem);
}



void StripToolView::setListSelection(const QModelIndex &selectedIndex) {
    sidePanel()->setSelectedIndex(selectedIndex);
}



QModelIndex StripToolView::listSelection() const {
    return sidePanel()->selectedIndex();
}



void StripToolView::onSeriesDeselected() {
    emit plotSelectionChanged(0);
}
