#include "StripChart/StripToolView.h"

StripToolView::StripToolView(QWidget *parent) : QWidget(parent)
{
    plotView_ = 0;
    sidePanel_ = 0;
    controlPanel_ = 0;

    buildComponents();
    makeConnections();
    defaultSettings();

    qDebug() << "StripToolView object created.";
}



StripToolView::~StripToolView() {
}



StripToolPlot* StripToolView::plotView() const {
    return plotView_;
}



StripToolListView* StripToolView::listView() const {
    return sidePanel()->listView();
}



EntryWidget* StripToolView::nameEntry() const {
    return controlPanel_->nameEntry();
}



StripToolSidePanel* StripToolView::sidePanel() const {
    return sidePanel_;
}



StripToolControlsPanel* StripToolView::controlPanel() const {
    return controlPanel_;
}



void StripToolView::toggleSidePanel()
{
    if (sidePanelShown_) {
        sidePanelShown_ = false;
        sidePanel()->hide();
        controlPanel()->sidebarButton()->setText("<");

    } else {
        sidePanelShown_ = true;
        sidePanel()->show();
        controlPanel()->sidebarButton()->setText(">");
    }
}



void StripToolView::buildComponents() {
    plotView_ = new StripToolPlot(this);
    sidePanel_ = new StripToolSidePanel(this);
    controlPanel_ = new StripToolControlsPanel(this);
}



void StripToolView::makeConnections()
{
    connect( controlPanel()->sidebarButton(), SIGNAL(clicked()), this, SLOT(toggleSidePanel()) );
}



void StripToolView::defaultSettings()
{
    sidePanelShown_ = true;
    toggleSidePanel();

    QHBoxLayout *upperLayout = new QHBoxLayout();
    upperLayout->addWidget(plotView());
    upperLayout->addWidget(sidePanel());

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(upperLayout);
    mainLayout->addWidget(controlPanel());

    setLayout(mainLayout);
}
