#ifndef XRFPERIODICTABLE_H
#define XRFPERIODICTABLE_H

#include <QObject>

#include "util/VESPERS/XRFElement.h"

class XRFPeriodicTable : public QObject
{
	Q_OBJECT
public:
	/// Default constructor.  Can optionally provide upper and lower limits.  These will disable elements based on those numbers.
	explicit XRFPeriodicTable(double minEnergy = 0, double maxEnergy = 1e6, QObject *parent = 0);
	/// Destructor.
	~XRFPeriodicTable();

	// Table convenience functions.
	/// Returns the list of XRF elements.
	QList<XRFElement *> elements() const { return xrfTable_; }

	/// Returns the element specified by the given \em name.  Returns 0 if \em name doesn't exist.
	XRFElement *elementByName(QString name) const
	{
		for (int i = 0; i < xrfTable_.size(); i++)
			if (QString::compare(xrfTable_.at(i)->name(), name) == 0)
				return xrfTable_.at(i);

		return 0;
	}
	/// Returns the element specified by the given \em symbol.  Returns 0 if \em symbol doesn't exist.
	XRFElement *elementBySymbol(QString symbol) const
	{
		for (int i = 0; i < xrfTable_.size(); i++)
			if (QString::compare(xrfTable_.at(i)->symbol(), symbol) == 0)
				return xrfTable_.at(i);

		return 0;
	}
	/// Returns the element specified by the given atomic number.  The number must be a valid atomic number between 1 <= atomicNumber <= 109.
	XRFElement *elementByAtomicNumber(int number) const { return xrfTable_.at(number-1); }

	// XRF table attributes
	/// Returns the current element being examined.
	XRFElement *currentElement() const { return current_; }
	/// Returns the minimum energy.
	double minimumEnergy() const { return minimumEnergy_; }
	/// Returns the maximum energy.
	double maximumEnergy() const { return maximumEnergy_; }
	/// Returns the list of selected elements.
	QList<XRFElement *> selectedElements() const { return selectedElements_; }

signals:
	/// Notifier that the minimum energy has changed.  The new energy is passed on.
	void minimumEnergyChanged(double);
	/// Notifier that the maximum energy has changed.  The new energy is passed on.
	void maximumEnergyChanged(double);

	/// Notifies when the current element has changed.  Passes a reference to the new element.
	void currentElementChanged(XRFElement *);
	/// Notifier that a region of interest has been added.  Passes a reference to the element and the line that was added.
	void addedRegionOfInterest(XRFElement *, QString);
	/// Notifier that a region of interest has been removed.  Passes a reference to the element and the line that was added.
	void removedRegionOfInterest(XRFElement *, QString);

public slots:
	/// Sets the current element that is being examined.
	void setCurrentElement(XRFElement *el) { current_ = el; emit currentElementChanged(current_); }
	/// Adds an element line to the current element to the checked list, if it already doesn't reside there.
	void addLineToList(QString line);
	/// Removes an element line from the current element from the checked list, if it exists in the list.
	void removeLineFromList(QString line);
	/// Removes all selected lines.
	void removeAll();

	/// Sets the minimum energy.
	void setMinimumEnergy(double energy) { minimumEnergy_ = energy; emit minimumEnergyChanged(minimumEnergy_); }
	/// Sets the maximum energy.
	void setMaximumEnergy(double energy) { maximumEnergy_ = energy; emit maximumEnergyChanged(maximumEnergy_); }

protected:

	/// Holds the minimum energy.  This is the lower limit and elements that don't have emission lines with energies higher then this are disabled.
	double minimumEnergy_;
	/// Holds the maximum energy.  This is the upper limit and elements that don't have emission lines with energies lower then this are disabled.
	double maximumEnergy_;

	/// The periodic table using XRFElements instead.
	QList<XRFElement *> xrfTable_;
	/// List of elements that have been selected.
	QList<XRFElement *> selectedElements_;
	/// The current element that is being viewed.
	XRFElement *current_;

};

#endif // XRFPERIODICTABLE_H
