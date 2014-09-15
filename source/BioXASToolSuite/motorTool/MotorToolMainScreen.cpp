#include "MotorToolMainScreen.h"
#include "ui_MotorToolMainScreen.h"

MotorToolMainScreen::MotorToolMainScreen(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::MotorToolMainScreen)
{
    ui->setupUi(this);
}

MotorToolMainScreen::~MotorToolMainScreen()
{
    delete ui;
}
