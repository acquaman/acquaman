/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
