#include "StripTool.h"

StripTool::StripTool(QWidget *parent) : QWidget(parent)
{
    saveDirectory_ = QDir("/Users/helfrij/Desktop");

    model_ = new StripToolModel(this, saveDirectory_);

    mainView_ = new StripToolView(this, model_);

    QVBoxLayout *windowLayout = new QVBoxLayout();
    windowLayout->addWidget(mainView_);

    setLayout(windowLayout);
    resize(500, 500);
}



StripTool::~StripTool()
{
}
