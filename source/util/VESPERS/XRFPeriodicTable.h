#ifndef XRFPERIODICTABLE_H
#define XRFPERIODICTABLE_H

#include <QObject>

#include "util/AMElement.h"
#include "util/AMPeriodicTable.h"

class XRFPeriodicTable : public QObject
{
	Q_OBJECT
public:
	/// Default constructor.  Can optionally provide upper and lower limits.  These will disable elements based on those numbers.
	explicit XRFPeriodicTable(double minEnergy = 0, double maxEnergy = 1e6, QObject *parent = 0);

	// Table convenience functions.
	/// Returns the list of elements.  \bug Should this be const? Right now it gives read-write access to the elements that are shared with the original table, via the AMElement pointers.
	QList<AMElement *> elements() const { return AMPeriodicTable::table()->elements(); }

	/// Returns the element specified by the given \em name.  Returns 0 if \em name doesn't exist.
	AMElement *elementByName(QString name) const { return AMPeriodicTable::table()->elementByName(name); }
	/// Returns the element specified by the given \em symbol.  Returns 0 if \em symbol doesn't exist.
	AMElement *elementBySymbol(QString symbol) const { return AMPeriodicTable::table()->elementBySymbol(symbol); }
	/// Returns the element specified by the given atomic number.  The number must be a valid atomic number between 1 <= atomicNumber <= 109.
	AMElement *elementByAtomicNumber(int number) const { return AMPeriodicTable::table()->elementByAtomicNumber(number); }

	// XRF table attributes
	/// Returns the list of checked elements.
	QList<QPair<int, QString> > checkedList() const { return checkedList_; }

	/// Returns the minimum energy.
	double minimumEnergy() const { return minimumEnergy_; }
	/// Returns the maximum energy.
	double maximumEnergy() const { return maximumEnergy_; }

signals:
	/// Notifier that the minimum energy has changed.  The new energy is passed on.
	void minimumEnergyChanged(double);
	/// Notifier that the maximum energy has changed.  The new energy is passed on.
	void maximumEnergyChanged(double);
	/// Notifier that an ROI has been added.
	void roiAdded(AMElement *el, QString line);
	/// Notifier that an ROI has been removed.
	void roiRemoved(AMElement *el, QString line);

public slots:
	/// Adds an element/line combo to the checked list, if it already doesn't reside there.
	void addToList(AMElement *el, QString line);
	/// Removes an element/line combo from the checked list, if it exists in the list.
	void removeFromList(AMElement *el, QString line);
	/// Clears the list of all ROIs.
	void clearAll();

	/// Sets the minimum energy.
	void setMinimumEnergy(double energy) { minimumEnergy_ = energy; emit minimumEnergyChanged(minimumEnergy_); }
	/// Sets the maximum energy.
	void setMaximumEnergy(double energy) { maximumEnergy_ = energy; emit maximumEnergyChanged(maximumEnergy_); }

protected:

	/// Holds a list with the atomic number paired with an emission line name.
	QList<QPair<int, QString> > checkedList_;

	/// Holds the minimum energy.  This is the lower limit and elements that don't have emission lines with energies higher then this are disabled.
	double minimumEnergy_;
	/// Holds the maximum energy.  This is the upper limit and elements that don't have emission lines with energies lower then this are disabled.
	double maximumEnergy_;

};

#endif // XRFPERIODICTABLE_H
