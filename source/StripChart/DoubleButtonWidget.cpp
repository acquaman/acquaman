#include "DoubleButtonWidget.h"

DoubleButtonWidget::DoubleButtonWidget(QWidget *parent) :
    QWidget(parent)
{
    leftButton_ = new QPushButton();
    connect( leftButton_, SIGNAL(clicked()), this, SIGNAL(leftButtonClicked()) );

    rightButton_ = new QPushButton();
    connect( rightButton_, SIGNAL(clicked()), this, SIGNAL(rightButtonClicked()) );

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(leftButton_);
    buttonLayout->addWidget(rightButton_);

    setLayout(buttonLayout);

}



DoubleButtonWidget::~DoubleButtonWidget()
{
}



void DoubleButtonWidget::setLeftButtonText(const QString &text)
{
    leftButton_->setText(text);
}



void DoubleButtonWidget::setLeftButtonDefault(bool isDefault)
{
    leftButton_->setDefault(isDefault);
}



void DoubleButtonWidget::setRightButtonText(const QString &text)
{
    rightButton_->setText(text);
}



void DoubleButtonWidget::setRightButtonDefault(bool isDefault)
{
    rightButton_->setDefault(isDefault);
}
