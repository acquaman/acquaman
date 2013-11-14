#include "StripTool.h"

StripTool::StripTool(QWidget *parent) : QWidget(parent)
{

//    QDir home = QDir(QDir::toNativeSeparators(QDir::homePath()));
//    home.mkdir("StripTool");

//    appDirectory_ = QDir(home.filePath("StripTool"));

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
