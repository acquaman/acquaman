/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMSTARTSCREEN_H
#define AMSTARTSCREEN_H


#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include "ui/dataman/AMRunSelector.h"
#include "dataman/database/AMDatabase.h"

class AMStartScreen : public QWidget
{
	Q_OBJECT
public:
	AMStartScreen(QWidget *parent = 0);

signals:

public slots:
	bool storeCurrentRun();

protected:
	AMRunSelector *runSelector_;

	virtual void closeEvent(QCloseEvent *);

};




#endif // AMSTARTSCREEN_H
