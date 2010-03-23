// $Header: displayAlias.h 1.2.1.3 2009/03/04 15:22:07CST Glen Wright (wrightg) Exp  $
// Copyright Canadian Light Source, Inc. All rights reserved.
//
// Description:
//    display Alias support
//	Multigroup membership for tracking relationships between two strings.
//	special string categories: "alias" ID matches are checked for a match when a connector is generated.
//		"indirect" means that the alias is not taken from string two, but from interpreting the PV
//		named in string2.
//

#ifndef QDISPLAYALIAS_H
#define QDISPLAYALIAS_H 1

#include <qobject.h>
#include "epicsConnect.h"
#include <vector>

using namespace std;

class displayAlias : public QObject
{
    typedef vector<QString> groupIDstring;
    typedef vector<int> groupIDint;
    Q_OBJECT
    
signals:
    void newAlias(const QString &oldDesc, const QString &newDesc); // notifies of changes to the description
    void haveAlias(const QString &); // notifies of changes to the alias
    
public slots:
    void setPvName( const QString &); // sets the PV name, and asks for a description string
    void setAlias(const QString &); // sets the alias.
    void setIndirect();		 // sets the alias to be the value of the named PV (.DESC or other string field)
    void setCategory(int);	// adds to the category list
    void setCategory(const QString &);	// adds to the category list
    void setCategoryList(const QString &);	// adds to the category list
public:
    displayAlias();
    ~displayAlias();
    // returns the element matching the name
    // if searchCategory is >=0 , then the search is restricted to matching categories
    static displayAlias *find(const QString&,int searchCategory=-1);
    static displayAlias *find(const QString&,const QString & searchCategory);
    static displayAlias *first(int searchCategory=-1);
    static displayAlias *first(const QString & searchCategory);
    displayAlias *next(int searchCategory=-1);
    displayAlias *next(const QString &searchCategory);
    displayAlias *nextInCategory();
    QString getName();  // returns the preferred name for the PV
    QString getPvName();  // returns the actual PV name
    QString getExpandedPvName();  // checks for name macro expansion!
    QString getAlias();   // returns the alias
    static Connector *get_connector(void *private_data, const QString &pvName);
    static void localUpdate(Connector *conp);
private:
    static displayAlias *head;  // pointer to 1st alias
    displayAlias *nextp, *prevp;  // doubly linked list of all aliases
    QString pv_name;
    QString alias;
    QString indirectName;		// name of the PV used for indirection
    groupIDint iCategory;
    groupIDstring sCategory;
  
    Connector *pv_desc; // pointer to the description PV
    bool isIndirect;	// 0: normal associative value. 1: second value is a PV name, go fetch.
    bool isAlias;		// 0: simple list. 1: use when building a PV connection
    bool haveIndirect;	// true when an indirect value has been returned.
  
    bool testCategory(int);
    bool testCategory(const QString &);
    displayAlias *fastNext() 
	    {
		    if(nextp == head) return NULL;
		    return nextp;
	    };
    displayAlias *fastPrev() 
	    {
		    if(prevp == head) return NULL;
		    return prevp;
	    };
};

#endif

//  $Log: displayAlias.h  $
//  Revision 1.2.1.3 2009/03/04 15:22:07CST Glen Wright (wrightg) 
//  Updates for: use of color rules; version 3.x acquisition library.
//  Revision 1.2 2007/05/19 12:25:31CST Elder Matias (matiase) 
//  Added header/log info
//
