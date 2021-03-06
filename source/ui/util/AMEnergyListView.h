#ifndef AMENERGYLISTVIEW_H
#define AMENERGYLISTVIEW_H

#include <QWidget>

#include "util/AMEnergyList.h"
#include "util/AMRange.h"

#include <QDoubleSpinBox>
#include <QMap>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QToolButton>

/// View that holds an energy.
class AMEnergyListElementView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Builds a view for a single energy.
	explicit AMEnergyListElementView(double energy, QWidget *parent = 0);

	/// Returns the energy of view.
	double energy() const { return energy_; }

	/// Returns the energy spinbox of view
	QDoubleSpinBox *energySpinBox() const { return energySpinBox_; }

signals:
	/// Notifier that the energy has changed.
	void energyChanged(double);

public slots:
	/// Set the energy spin box.
	void setEnergySpinBox(double energy);
	/// Set the range for the spin box.
	void setEnergyRange(const AMRange &newRange);

protected slots:
	/// Handles updating the energy when it changes.
	void onEnergyUpdated();

protected:
	/// The energy.
	double energy_;
	/// The spin box that holds the energy.
	QDoubleSpinBox *energySpinBox_;
};

/// Builds view for a list of energies.  Has a simple add/remove context.
class AMEnergyListView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMEnergyListView(const QString &title, const AMEnergyList &list, QWidget *parent = 0);

	/// Returns the energy list.
	AMEnergyList energies() const { return energyList_; }

signals:

public slots:
	/// Adds a region to the axis.
	void insertEnergy(int index, double energy);
	/// Removes a region from the given index.
	void removeEnergy(int index);

	/// Sets the energy range for the spin boxes.
	void setRange(double minimum, double maximum);

protected slots:
	/// Handles adding a new region.
	void onAddEnergyButtonClicked();
	/// Handles removing a step scan axis element view after a user clicks the delete button.
	void onDeleteButtonClicked(QAbstractButton *button);
	/// Handles updating the list of energies.  Brute force goes over all items.
	void onElementEnergyChanged();

protected:
	/// Helper method that builds and connects all the key aspects around a given an energy element.
	void buildEnergyElementView(int index, double energy);

	/// Reset the tab orders of the EnergyElementView
	void resetEnergyElementViewTabOrder(int index);

	/// Holds the list of energies.
	AMEnergyList energyList_;
	/// List specifically for locking the regions together properly.
	QList<AMEnergyListElementView *> energyElementViewList_;
	/// A mapping that maps the delete button to the region it is associated with.
	QMap<QAbstractButton *, AMEnergyListElementView *> energyMap_;
	/// A map that holds the layout that holds the delete button and the element view for memory management.
	QMap<QAbstractButton *, QLayout *> layoutMap_;
	/// The main layout that is added to and removed from.
	QVBoxLayout *energyViewLayout_;
	/// The button group for mapping a delete button to a element view.
	QButtonGroup *deleteButtonGroup_;
	/// The add region button.  Used for proper placement of the popup menu when adding regions.
	QToolButton *addRegionButton_;
	/// Hold the range for the spin boxes.
	AMRange energyRange_;
};

#endif // AMENERGYLISTVIEW_H
