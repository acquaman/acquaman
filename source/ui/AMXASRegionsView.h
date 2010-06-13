#ifndef AMXASREGIONSVIEW_H
#define AMXASREGIONSVIEW_H

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMenu>
#include <QCursor>
#include "acquaman/AMRegionsList.h"

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
	~AMXASRegionsView();

signals:
	void addRegionClicked();

public slots:
	void setBeamlineEnergy(AMControl *beamlineEnergy){regions_->setEnergyControl(beamlineEnergy);}
	bool addRegion(){
		if(addRegionMenu_)
			delete addRegionMenu_;
		addRegionMenu_ = new QMenu(this);
		QAction *tmpAction;
		tmpAction = addRegionMenu_->addAction("Front");
		tmpAction->setData(0);
		connect(tmpAction, SIGNAL(hovered()), this, SLOT(setInsertIndex()));
		connect(tmpAction, SIGNAL(triggered()), this, SLOT(insertRegion()));
		QString tmpStr;
		for(int x = 0; x < regions_->count(); x++){
			tmpStr.clear();
			tmpStr.setNum(x+1);
			tmpAction = addRegionMenu_->addAction(tmpStr.prepend("After Region "));
			tmpAction->setData(x+1);
			connect(tmpAction, SIGNAL(hovered()), this, SLOT(setInsertIndex()));
			connect(tmpAction, SIGNAL(triggered()), this, SLOT(insertRegion()));
		}
		addRegionMenu_->popup(QCursor::pos());
		addRegionMenu_->show();
		return true;
	}
	bool deleteRegion(){
		if(deleteRegionMenu_)
			delete deleteRegionMenu_;
		deleteRegionMenu_ = new QMenu(this);
		QAction *tmpAction;
//		tmpAction = deleteRegionMenu_->addAction("Front");
//		tmpAction->setData(0);
//		connect(tmpAction, SIGNAL(hovered()), this, SLOT(setInsertIndex()));
//		connect(tmpAction, SIGNAL(triggered()), this, SLOT(insertRegion()));
		QString tmpStr;
		for(int x = 0; x < regions_->count(); x++){
			tmpStr.clear();
			tmpStr.setNum(x);
			tmpAction = deleteRegionMenu_->addAction(tmpStr.prepend("Region "));
			tmpAction->setData(x);
			connect(tmpAction, SIGNAL(hovered()), this, SLOT(setRemoveIndex()));
			connect(tmpAction, SIGNAL(triggered()), this, SLOT(removeRegion()));
		}
		deleteRegionMenu_->popup(QCursor::pos());
		deleteRegionMenu_->show();
		return true;
//		return regions_->deleteRegion(regions_->count()-1);
	}
	void setInsertIndex(){
		QAction *tmpAction;
		tmpAction = addRegionMenu_->activeAction();
		insertIndex_ = tmpAction->data().toInt();
	}
	void setRemoveIndex(){
		QAction *tmpAction;
		tmpAction = deleteRegionMenu_->activeAction();
		removeIndex_ = tmpAction->data().toInt();
	}

	bool insertRegion(){
		double nextStart, nextEnd, prevStart, prevEnd;
		double sensibleStart = 200;
		double sensibleEnd = 2000;
		if(regions_->count() == 0){
			return regions_->addRegion(insertIndex_, sensibleStart, 1, sensibleEnd);
		}
		else if(insertIndex_ == 0){
			nextStart = regions_->start(insertIndex_);
			return regions_->addRegion(insertIndex_, sensibleStart, 1, nextStart);
		}
		else if(insertIndex_ == regions_->count()){
			prevEnd = regions_->end(insertIndex_-1);
			return regions_->addRegion(insertIndex_, prevEnd, 1, sensibleEnd);
		}
		else{
			prevStart = regions_->start(insertIndex_-1);
			prevEnd = regions_->end(insertIndex_-1);
			nextStart = regions_->start(insertIndex_);
			nextEnd = regions_->end(insertIndex_);
			return regions_->addRegion(insertIndex_, prevStart+(prevEnd-prevStart)/2, 1, nextStart+(nextEnd-nextStart)/2 );
		}

	}
	bool removeRegion(){
		bool retVal = regions_->deleteRegion(removeIndex_);
		if(retVal){
			if( (removeIndex_ != 0) && (removeIndex_ != regions_->count()+1) ){
				double prevEnd, nextStart;
				prevEnd = regions_->end(removeIndex_-1);
				nextStart = regions_->start(removeIndex_);
				regions_->setEnd(removeIndex_-1, prevEnd+(nextStart-prevEnd)/2);
			}
		}
		return retVal;
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
	QMenu *addRegionMenu_;
	QMenu *deleteRegionMenu_;
	int insertIndex_;
	int removeIndex_;
};

#endif // AMXASREGIONSVIEW_H
