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


#ifndef DOUBLEBUTTONWIDGET_H
#define DOUBLEBUTTONWIDGET_H

#pragma clang diagnostic ignored "-Wunused-private-field"
#include <QtGui>
#pragma clang diagnostic warning "-Wunused-private-field"

#include <QWidget>

class DoubleButtonWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DoubleButtonWidget(QWidget *parent = 0);
    ~DoubleButtonWidget();

signals:
    void leftButtonClicked();
    void rightButtonClicked();

protected:
    QPushButton *leftButton_;
    QPushButton *rightButton_;

public:
    void setLeftButtonText(const QString &text);
    void setLeftButtonDefault(bool isDefault);
    void setRightButtonText(const QString &text);
    void setRightButtonDefault(bool isDefault);

    // here is a change so I can commit something!
};

#endif // DOUBLEBUTTONWIDGET_H
