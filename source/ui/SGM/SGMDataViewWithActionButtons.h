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
