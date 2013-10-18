#include "StripTool.h"

StripTool::StripTool(QWidget *parent) : QWidget(parent)
{
    model_ = new StripToolModel(this);
    view_ = new StripToolView(this, model_);

    QVBoxLayout *windowLayout = new QVBoxLayout();
    windowLayout->addWidget(view_);

    setLayout(windowLayout);
}



StripTool::~StripTool()
{
}
