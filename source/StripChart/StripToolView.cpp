#include "StripChart/StripToolView.h"

StripToolView::StripToolView(QWidget *parent, StripToolModel *model) : QWidget(parent)
{
    model_ = model;
    connect( model_, SIGNAL(showReloadDialog()), this, SLOT(reloadDialog()) );
    connect( this, SIGNAL(reloadPVs(bool)), model_, SLOT(reloadPVs(bool)) );

    createActions();
    buildUI();
}



StripToolView::~StripToolView()
{
}



void StripToolView::createActions()
{
    newPlotAction_ = new QAction("New plot", this);
    newPlotAction_->setEnabled(false);

    saveDataAction_ = new QAction("Save data", this);
    saveDataAction_->setEnabled(false);

    quitAction_ = new QAction("Quit", this);
    connect( quitAction_, SIGNAL(triggered()), qApp, SLOT(quit()) );
}



void StripToolView::buildUI()
{
    plotView_ = new StripToolPlot(this);
    plotView_->setModel(model_);

    QCheckBox *controlsToggle = new QCheckBox("Show controls", this);
    controlsToggle->setChecked(true);

    QVBoxLayout *plotLayout = new QVBoxLayout();
    plotLayout->addWidget(plotView_);
    plotLayout->addWidget(controlsToggle);

    quickControls_ = new StripToolQuickControls(this);
    quickControls_->setModel(model_);
    connect(controlsToggle, SIGNAL(stateChanged(int)), this, SLOT(toggleControls(int)) );

    QHBoxLayout *windowLayout = new QHBoxLayout();
    windowLayout->addLayout(plotLayout);
    windowLayout->addWidget(quickControls_);

    setLayout(windowLayout);
}



void StripToolView::reloadDialog()
{
    QMessageBox reloadDialog;
    reloadDialog.setText("Previous PVs Detected");
    reloadDialog.setInformativeText("Do you want to reload previous pvs?");
    reloadDialog.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    reloadDialog.setDefaultButton(QMessageBox::Ok);

    int dialogVal = reloadDialog.exec();

    if (dialogVal == QMessageBox::Ok)
        emit reloadPVs(true);

    else
        emit reloadPVs(false);
}



void StripToolView::toggleControls(int checkState)
{
    if (checkState == 0)
        quickControls_->hide();
    else
        quickControls_->show();
}
