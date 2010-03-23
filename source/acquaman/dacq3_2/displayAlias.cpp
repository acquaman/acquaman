// $Header: displayAlias.cpp 1.2.1.3 2009/03/04 15:22:17CST Glen Wright (wrightg) Exp  $
// Copyright Canadian Light Source, Inc. All rights reserved.
//
// Description:
//



#include "displayAlias.h"
#include <qstringlist.h>
#include <stdio.h>

#define DEBUG if(0)
//
// update function for indirect aliases. Reads the value from the connector, and sets the alias to the returned string.
//

void displayAlias::localUpdate(Connector *conp)
{
    displayAlias *ptr;
    char result[100];
    QString oldAlias;
    
    ptr = (displayAlias *)conp->private_data;
    con_val_to_string( conp, result, sizeof result, 0);
    oldAlias = ptr->alias;
    ptr->alias = result;
    
    emit ptr->newAlias( oldAlias, ptr->alias);
    
}

displayAlias * displayAlias::head;
//
displayAlias::displayAlias() : pv_name(NULL), alias(NULL), pv_desc(NULL), isIndirect(0), isAlias(0)
{
    if( head == NULL)
 head = prevp = nextp = this;
    else
    {
 nextp = head;
 prevp = head->prevp;
 nextp->prevp = this;
 prevp->nextp = this;
    }
}

displayAlias::~displayAlias()
{
    if( head == this)
    {
 if( head->nextp == this)
 {
     head = NULL;
     return;
 }
 head = head->nextp;
    }
    nextp->prevp = prevp;
    prevp->nextp = nextp;
}

void displayAlias::setPvName(const QString &pvn)
{
    QString descName;
    if( pvn == "" || pvn == pv_name)
	    return;
    pv_name = pvn;
}

void displayAlias::setAlias(const QString &aliasName)
{
	if( alias.isEmpty() && ! aliasName.isEmpty() && isIndirect)
	{
		// not setting the alias, but the PV name that will get the alias
		if( !indirectName.isEmpty())
		{
			// sanity check - possible multiple names?
			return;
		}
		indirectName = aliasName;
		pv_desc = build_connector( this, indirectName.toAscii().data() );
		pv_desc->update = localUpdate;
		if( pv_desc->chan && pv_desc->chan->chan->valid)
			localUpdate( pv_desc);
		return;
	}
	
	alias = aliasName;
	emit haveAlias(aliasName);
}

void displayAlias::setIndirect()
{
	if( isIndirect)
		return;
	isIndirect = 1;
	if( alias.isEmpty() )
		return;
	indirectName = alias;
	alias = "";
	pv_desc = build_connector( this, indirectName.toAscii().data() );
	pv_desc->update = localUpdate;
	if( pv_desc->chan && pv_desc->chan->chan->valid)
		localUpdate( pv_desc);
}

void displayAlias::setCategory( int whichCategory)
{
    iCategory.push_back(whichCategory);
}

void displayAlias::setCategory( const QString &category)
{
    sCategory.push_back( category);
    if( category == "indirect")
	    setIndirect();
}

void displayAlias::setCategoryList( const QString &category)
{
	
//	QStringList qlist = QStringList::split(",", category);
    QStringList qlist = category.split(",");
    for( QStringList::Iterator it = qlist.begin(); it != qlist.end(); ++it)
	{
		bool ok;
		int icat = (*it).toInt(&ok);
		if( ok)
			setCategory(icat);
		else
			setCategory(*it);
	}
}



displayAlias  *displayAlias::find(const QString &name, int searchCategory)
{
    displayAlias *ptr;
    
    for( ptr=displayAlias::first(searchCategory); ptr; ptr=ptr->next(searchCategory) )
    {
	 if( ptr->pv_name == name || ptr->alias == name)
	 {
		     return ptr;
	 }
    }
    return NULL;
    
}

bool
displayAlias::testCategory(int icat)
{
	
     for(unsigned int idx=0; idx < iCategory.size(); idx++)
	if( iCategory[idx] == icat)
		return TRUE;
     return FALSE;
}

bool
displayAlias::testCategory(const QString &scat)
{
     for(unsigned int idx=0; idx < sCategory.size(); idx++)
	if( sCategory[idx] == scat)
		return TRUE;
     return FALSE;
     
}

displayAlias *displayAlias::find(const QString &name, const QString &searchCategory)
{
    displayAlias *ptr;
    
    for( ptr=first(searchCategory); ptr; ptr=ptr->next(searchCategory) )
    {
	 if( ptr->pv_name == name || ptr->alias == name)
	 {
	     return ptr;
	 }
    }
    return NULL;
}

displayAlias *displayAlias::first(int searchCategory)
{
    displayAlias *ptr;
    if( searchCategory < 0)
 return displayAlias::head;
    for( ptr=displayAlias::head; ptr ; ptr =(ptr->nextp == displayAlias::head?NULL:ptr->nextp) )
    {
	    if( ptr->testCategory( searchCategory) )
	     return ptr;
    }
    return NULL;
}

displayAlias *displayAlias::next(int searchCategory)
{
    if( searchCategory < 0)
	 return fastNext();
    
    for( displayAlias *ptr=fastNext(); ptr ; ptr = ptr->fastNext() )
    {
	    if( ptr->testCategory(searchCategory))
	     return ptr;
    }
    return NULL;
}

displayAlias *displayAlias::first(const QString & searchCategory)
{
    displayAlias *ptr;

    for( ptr=head; ptr ; ptr = ptr->fastNext() )
    {
	    if( ptr->testCategory( searchCategory) )
	     return ptr;
    }
    return NULL;
}

displayAlias *displayAlias::next(const QString & searchCategory)
{
    
    for( displayAlias *ptr=this->fastNext(); ptr ; ptr = ptr->fastNext() )
    {
	    if( ptr->testCategory(searchCategory))
	     return ptr;
    }
    return NULL;
}

displayAlias *displayAlias::nextInCategory()
{
    if( this == NULL)
 return NULL;
    
    return next(this->iCategory[0]);
}

QString displayAlias::getName()
{
    if( !alias.isEmpty() )
	 return alias;
    return pv_name;
}

QString displayAlias::getPvName()
{
    return pv_name;
}

QString displayAlias::getExpandedPvName()
{
	char result[100];
	if( pv_name.contains('$') == FALSE)
		return pv_name;
	macro_expand( pv_name.toAscii().data(), result, NULL);
	return QString(result);
}

QString displayAlias::getAlias()
{
    return alias;
}

Connector * displayAlias::get_connector( void *private_data, const QString &pv_name)
{
	QString realName;
	displayAlias *ptr;
	Connector *result;
	
	ptr = displayAlias::find(pv_name, "alias");
	if( ptr)
		realName = ptr->getPvName();
	else
		realName = pv_name;
	result = build_connector(private_data, realName.toAscii().data() );
	DEBUG printf("get_connector(%s) uses '%s' finds %p build %p\n", pv_name.toAscii().data(), realName.toAscii().data(), ptr, result);
	return result;
}

Connector *get_connector( void *private_data, const QString &pv_name)
{
	return displayAlias::get_connector(private_data, pv_name);
}

// $Log: displayAlias.cpp  $
// Revision 1.2.1.3 2009/03/04 15:22:17CST Glen Wright (wrightg) 
// Updates for: use of color rules; version 3.x acquisition library.
// Revision 1.2 2007/05/19 12:12:24CST Elder Matias (matiase) 
// Added header/log


