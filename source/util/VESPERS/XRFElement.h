#ifndef XRFELEMENT_H
#define XRFELEMENT_H

#include "util/AMElement.h"

#include <QStringList>
#include <QMap>

class XRFElement : public AMElement
{
	Q_OBJECT
public:
	/// Constructor.  Takes in an AMElement and builds a copy of it with the inclusion of selected lines for regions of interest.
	XRFElement(AMElement *el, double minimumEnergy = 0, double maximumEnergy = 1e6, QObject *parent = 0);

	/// Returns the list of checked off lines.  Returns the name of the line (ie: Ka1).
	QStringList linesSelected() const { return selected_; }
	/// Boolean used to know whether there are any checked lines in the element.
	bool hasLinesSelected() const { return !selected_.isEmpty(); }
	/// Returns the number of selected lines in the element.
	int numberLinesSelected() { return selected_.size(); }

	/// Returns the energy of a given emission line.  Returns 0 if given an invalid line name.
	double lineEnergy(QString line) { return lineMap_.value(line); }

	/// Returns the minimum energy for the element.
	double minimumEnergy() const { return minimumEnergy_; }
	/// Returns the maximum energy for the element.
	double maximumEnergy() const { return maximumEnergy_; }

public slots:
	/// Add a selected line to the element.
	void addLine(QString line);
	/// Removes a selected line from the element.
	void removeLine(QString line);
	/// Removes all selected lines from the element.  Emits signal lineRemoved after each line has been removed.
	void removeAllLines();

	/// Sets the minimum energy for the element.
	void setMinimumEnergy(double energy) { minimumEnergy_ = energy; emit minimumEnergyChanged(energy); }
	/// Sets the maximum energy for the element.
	void setMaximumEnergy(double energy) { maximumEnergy_ = energy; emit maximumEnergyChanged(energy); }

signals:
	/// Notifier that a line has been added.  Passes a reference to itself and the line that was added.
	void lineAdded(XRFElement *, QString);
	/// Notifier that a line has been removed.  Passes a reference to itself and the line that was removed.
	void lineRemoved(XRFElement *, QString);

	/// Notifier that the minimum energy has changed.  The new energy is passed on.
	void minimumEnergyChanged(double);
	/// Notifier that the maximum energy has changed.  The new energy is passed on.
	void maximumEnergyChanged(double);

protected:
	/// Helper function that returns true if a given energy is acceptable to be added.
	bool energyAcceptable(double energy) { return (energy >= minimumEnergy_ && energy <= maximumEnergy_) ? true : false; }

	/// The minimum energy that is allowed for the lines to be selected.
	double minimumEnergy_;
	/// The maximum energy that is allowed for the lines to be selected.
	double maxixumEnergy_;

	/// The list of the selected lines.
	QStringList selected_;
	/// Maps the line energy to the name.  This is meant as a convenience rather than searching through emission lines manuall.
	QMap<QString, double> lineMap_;
};

#endif // XRFELEMENT_H
