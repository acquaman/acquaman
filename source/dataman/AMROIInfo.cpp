#include "AMROIInfo.h"

AMROIInfo::AMROIInfo(const QString &name, double energy, double low, double high, double scale, QObject *parent)
	: AMDbObject(parent)
{
	setName(name);
	energy_ = energy;
	scale_ = scale;
	low_ = low;
	high_ = high;
}

AMROIInfo::AMROIInfo(const QString &name, double energy, double width, double scale, QObject *parent)
	: AMDbObject(parent)
{
	setName(name);
	energy_ = energy;
	scale_ = scale;
	low_ = energy*(1-width/2);
	high_ = energy*(1+width/2);
}

AMROIInfoList::AMROIInfoList(QObject *parent)
	: AMDbObject(parent), AMOrderedList<AMROIInfo>()
{
	connect(signalSource(), SIGNAL(itemAdded(int)), this, SLOT(onROIAdded(int)));
	connect(signalSource(), SIGNAL(itemRemoved(int)), this, SLOT(onROIRemoved(int)));
	connect(signalSource(), SIGNAL(itemChanged(int)), this, SLOT(onROIValuesChanged(int)));
}

AMROIInfoList::AMROIInfoList(const AMROIInfoList& other)
	: AMDbObject(other), AMOrderedList<AMROIInfo>(other)
{
	connect(signalSource(), SIGNAL(itemAdded(int)), this, SLOT(onROIAdded(int)));
	connect(signalSource(), SIGNAL(itemRemoved(int)), this, SLOT(onROIRemoved(int)));
	connect(signalSource(), SIGNAL(itemChanged(int)), this, SLOT(onROIValuesChanged(int)));
}



AMROIInfoList& AMROIInfoList::operator=(const AMROIInfoList& other) {
	// always: check for self-assignment
	if(this != &other) {
		AMOrderedList<AMROIInfo>::operator=(other);
		AMDbObject::operator=(other);

		// if the items in other have been modified, but the signal hasn't been emitted yet, other.modified_ will not be set to true yet. We know that things have changed, so ours should be true.
		if(other.signalSource()->delayedItemChangedScheduled())
			setModified(true);
	}

	return *this;
}

// Returns a list of pointers to the AMROIInfo objects we store, for use by the database system in storeToDb() / loadFromDb().
AMDbObjectList AMROIInfoList::dbReadROIInfos() {
	AMDbObjectList rv;
	for(int i=0; i<count(); i++)
		rv << &((*this)[i]);
	return rv;
}

// Called by the database system on loadFromDb() to give us our new set of AMROIInfo objects. We copy these ones into our internal list and then delete them.
void AMROIInfoList::dbLoadROIInfos(const AMDbObjectList& newROIInfos) {
	clear();	// get rid of our existing

	for(int i=0; i<newROIInfos.count(); i++) {
		AMROIInfo* newROIInfo = qobject_cast<AMROIInfo*>(newROIInfos.at(i));
		if(newROIInfo) {
			append(*newROIInfo);	// note: makes a copy of object pointed to by newROIInfo, and stores in our internal list.
		}

		delete newROIInfos.at(i);	// we're copying these; don't need to keep these ones around. They're our responsibility to delete.
	}
}

void AMROIInfoList::setValuesFrom(const AMROIInfoList &other)
{
	int otherCount = other.count();

	// remove any extras we have.
	/// \todo \bug Watch out... these are being orphaned in the database.
	while(count() > otherCount)
		remove(count()-1);

	// loop through and modify the ones we have in common. In the case where us and other have the same size, this will be all we do.  No new rows will be needed in the database.
	for(int i=0; i<count(); i++) {
		(*this)[i].setValuesFrom(other.at(i));
	}

	// add any extras
	for(int i=count(); i<otherCount; i++) {
		AMROIInfo newROIInfo;
		newROIInfo.setValuesFrom(other.at(i));
		append(newROIInfo);
	}

	setModified(true);
}

void AMROIInfoList::sort()
{
	QList<AMROIInfo> list = toList();
	int smallest;

	for (int i = 0; i < list.size(); i++){

		smallest = i;

		for (int j = i; j < list.size(); j++)
			if (list.at(smallest).energy() > list.at(j).energy())
				smallest = j;

		if (i != smallest)
			list.swap(i, smallest);
	}

	for (int i = 0; i < count(); i++)
		replace(i, list.at(i));
}

int AMROIInfoList::indexOf(QString name)
{
	AMROIInfo temp;

	for (int i = 0; i < count(); i++){

		temp = at(i);

		if (name.compare(temp.name()) == 0)
			return i;
	}

	return -1;
}
