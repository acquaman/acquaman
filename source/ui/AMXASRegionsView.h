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
	  \param parent Pointer to QWidget to act as parent.
	  */
	explicit AMXASRegionsView(AMXASRegionsList *regions, QWidget *parent = 0);

signals:
	void addRegionClicked();

public slots:
	void setBeamlineEnergy(AMControl *beamlineEnergy){regions_->setEnergyControl(beamlineEnergy);}
	bool addRegion(int index, double start, double delta, double end){
		return regions_->addRegion(index, start, delta, end);
	}
	bool deleteRegion(){
		return regions_->deleteRegion(regions_->count()-1);
	}

protected:
	/// The table view used to interact with the group of AMXASRegion.
	QTableView *tv_;
	/// Button to add a region. \todo Figure out interface and hook up to something.
	QPushButton *addButton_;
	/// Button to delete a region. \todo Figure out interface and hook up to something.
	QPushButton *deleteButton_;
	/// The model created from the AMXASRegion group to be used with the table view.
	AMXASRegionsList *regions_;
};

#endif // AMXASREGIONSVIEW_H
