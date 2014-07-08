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


#ifndef SGMDATAVIEWWITHACTIONBUTTONS_H
#define SGMDATAVIEWWITHACTIONBUTTONS_H

#include "ui/dataman/AMDataViewWithActionButtons.h"

class AMScanSearchView;

class SGMDataViewWithActionButtons : public AMDataViewWithActionButtons
{
Q_OBJECT
public:
	/// Default constructor calls parent
	explicit SGMDataViewWithActionButtons(AMDatabase* database = AMDatabase::database("user"), QWidget *parent = 0);

protected:
	/// Reimplemented to instantiate an SGMDataView
	virtual AMDataView* createDataView(AMDatabase *database);
};

class SGMDataView : public AMDataView
{
Q_OBJECT
public:
	SGMDataView(AMDatabase* database = AMDatabase::database("user"), QWidget *parent = 0);

public slots:
	/// Opens the Search Scans dialog
	void onSearchScansButtonClicked();

protected:
	// Dialog window through which users can search scans
	AMScanSearchView* searchDialog_;
};

#endif // SGMDATAVIEWWITHACTIONBUTTONS_H
