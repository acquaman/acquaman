#ifndef AMXASREGIONSVIEW_H
#define AMXASREGIONSVIEW_H

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFormLayout>
#include "acquaman/AMRegion.h"

/// An AMXASRegionsView is a means of generating a default view for a group of AMXASRegion
/*!
  This default view utilizes a QTableView to interface with a group of AMXASRegion.
  By calling the constructor while passing a QList of AMXASRegion, the AMXASRegionsView:
  - creates add region and delete region buttons; and, \todo Hook these buttons up to something
  - uses the QList of AMXASRegion to make an AMXASRegionModel (the programmer need not do this nor understand how it works); and,
  - creates a table view on the newly created model.
  \todo This should most likely be an AMRegionsView ... there's no real need to have it at the AMXASRegion level. I'll fix this, I just needed to figure out if it would work in the first place.
  */
class AMXASRegionsView : public QWidget
{
Q_OBJECT
public:
	/// Constructor
	/*!
	  \param regions Pointer to a QList of AMXASRegions. Can be easily retreived using regionsPtr() function on an AMXASScanConfiguration or child.
	  */
	explicit AMXASRegionsView(QList<AMXASRegion*> *regions, QWidget *parent = 0);

signals:

public slots:

protected:
	QTableView *tv_;
	QPushButton *addButton_;
	QPushButton *deleteButton_;
	AMXASRegionModel *rm_;
};

#endif // AMXASREGIONSVIEW_H
