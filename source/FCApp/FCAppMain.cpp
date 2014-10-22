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

#include <QApplication>
#include "FCApp.h"
//#include "sidebar.h"


int main(int argc, char *argv[])
{
    QApplication::setStyle("motif");
    QApplication a(argc, argv);

    FCApp app;

    QPalette winPal_(a.palette());



    winPal_ = app.palette();
    app.setAutoFillBackground(true);

    winPal_.setColor(winPal_.Background, QColor(12, 12, 29));
    app.setPalette(winPal_);


    app.show();


/*
    SideBar w;

    w.addAction("Shutters", QIcon("/home/david/Desktop/147860-Sidebar/Sidebar/icons/light61.png"));
    w.addAction("Beam", QIcon("/home/david/Desktop/147860-Sidebar/Sidebar/icons/monitoring.png"));
    w.addAction("Exit", QIcon("/home/david/Desktop/147860-Sidebar/Sidebar/icons/exit13.png"));
*/

    //w.show();

    return a.exec();
}
