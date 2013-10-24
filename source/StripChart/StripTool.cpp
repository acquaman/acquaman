#include "StripTool.h"

StripTool::StripTool(QWidget *parent) : QWidget(parent)
{
    model_ = new StripToolModel(this);
    mainView_ = new StripToolView(this, model_);

    QVBoxLayout *windowLayout = new QVBoxLayout();
    windowLayout->addWidget(mainView_);

    setLayout(windowLayout);
    resize(500, 500);
}



StripTool::~StripTool()
{
}
