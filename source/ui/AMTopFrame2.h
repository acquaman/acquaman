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


#ifndef AMTOPFRAME2_H
#define AMTOPFRAME2_H

#include <QWidget>
#include <QIcon>

namespace Ui {
    class AMTopFrame2;
}

/// Standard header bar widget for main window panes
class AMTopFrame2 : public QWidget
{
    Q_OBJECT

public:
	explicit AMTopFrame2(const QString& title, const QIcon& icon = QIcon(), QWidget *parent = 0);
    virtual ~AMTopFrame2();

	QString title() const;
	QString leftSubText() const;
	QString rightSubText() const;

public slots:
	void setTitle(const QString& title);
	void setIcon(const QIcon& icon);
	void setLeftSubText(const QString& subText);
	void setRightSubText(const QString& subText);

private:
    Ui::AMTopFrame2 *ui;
};

#endif // AMTOPFRAME2_H
