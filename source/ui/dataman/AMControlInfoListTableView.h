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


#ifndef AMCONTROLINFOLISTTABLEVIEW_H
#define AMCONTROLINFOLISTTABLEVIEW_H

#include <QWidget>

class AMControlInfoList;
class QTableWidget;

/// This widget provides a simple table to display the name/description, value, and units of all controls in an AMControlInfoList. Because AMControlInfoList is a static, unchanging list, there is no event-driven updating of the view when the values change; simply call setFromInfoList() to update the view with the contents of any AMControlInfoList.
class AMControlInfoListTableView : public QWidget
{
    Q_OBJECT
public:
    /// Constructor.  The widget does not retain the reference to \c list, but simply uses it to initialize the table. If \c list is 0, the table is left empty.
 	virtual ~AMControlInfoListTableView();
    AMControlInfoListTableView(const AMControlInfoList* list = 0, QWidget *parent = 0);

    /// Update the view based on the contents of \c list. If \c list is 0, the table is cleared.
    void setFromInfoList(const AMControlInfoList* list);

signals:

public slots:

protected:
    QTableWidget* table_;


};

#endif // AMCONTROLINFOLISTTABLEVIEW_H
