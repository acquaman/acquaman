#include "StripTool.h"

StripTool::StripTool(QWidget *parent) : QWidget(parent)
{
    model_ = new StripToolModel(this);

    dataController_ = new StripToolDataController(this);
    dataController_->setModel(model_);
    dataController_->checkForPreviousPVs();

    mainView_ = new StripToolView(this, model_);

    QVBoxLayout *windowLayout = new QVBoxLayout();
    windowLayout->addWidget(mainView_);

    setLayout(windowLayout);
    resize(800, 500);
}



StripTool::~StripTool()
{
}
