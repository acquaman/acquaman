#ifndef AMENERGYLIST_H
#define AMENERGYLIST_H

#include <QObject>

/// This is a simple model that holds a list of energies.
class AMEnergyList : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMEnergyList(QObject *parent = 0);
	/// Copy constructor
	AMEnergyList(const AMEnergyList &original);

	/// Destructor.
	virtual ~AMEnergyList(){}

	/// Returns the number of energies in this list.
	int energyCount() const { return energies_.size(); }
	/// Returns the list of energies.
	QList<double> energies() const { return energies_; }
	/// Returns the energy at a given index.
	double energyAt(int index) const { return energies_.at(index); }

signals:
	/// Notifier that an energy was added.
	void energyAdded(double);
	/// Notifier that an energy was removed.
	void energyRemoved(double);
	/// Notifier that the energy changed at the provided index.
	void energyChanged(int);

public slots:
	/// Sets the energy at a particular index.
	void setEnergy(int index, double energy);
	/// Inserts an energy if it doesn't already exist.  Provide an index and desired energy.
	void insertEnergy(int index, double energy);
	/// Removes an energy from the list with the given index.
	void removeEnergy(int index);

protected:
	/// The list of energies.
	QList<double> energies_;
};

#endif // AMENERGYLIST_H
