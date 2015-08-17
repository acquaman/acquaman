#ifndef BIOXASXASSCANCONFIGURATIONEDGEEDITOR_H
#define BIOXASXASSCANCONFIGURATIONEDGEEDITOR_H

#include <QToolButton>
#include <QComboBox>
#include <QLayout>

#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"
#include "ui/BioXAS/BioXASXASScanConfigurationView.h"

class AMElement;
class AMAbsorptionEdge;

class BioXASXASScanConfigurationEdgeEditor : public BioXASXASScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASXASScanConfigurationEdgeEditor(BioXASXASScanConfiguration *configuration, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASXASScanConfigurationEdgeEditor();

public slots:
	/// Sets the configuration being viewed.
	virtual void setConfiguration(BioXASXASScanConfiguration *newConfiguration);

	/// Clears the view.
	virtual void clear();
	/// Updates the view with the latest configuration information.
	virtual void update();
	/// Refreshes the view.
	virtual void refresh();

protected slots:
	/// Updates the energy button with the most recent configuration information.
	void updateElementButton();
	/// Sets the element button element.
	void setElement(AMElement *element);

	/// Updates the edges combo box with the most recent configuration information.
	void updateEdgeComboBox();
	/// Adds all absorption edges to the edge combo box associated with the given element.
	void addEdges(AMElement *element);
	/// Adds an absorption edge to the edges combo box.
	void addEdge(const AMAbsorptionEdge &edge);

	/// Updates the configuration edge and energy with the element and edge selections.
	void updateConfiguration();
	/// Updates the configuration's edge with the most recent element and edge selections.
	void updateConfigurationEdge();
	/// Updates the configuration's energy with the most recent edge selection.
	void updateConfigurationEnergy();

	/// Displays the element selection dialog and updates the configuration with the element selection.
	void onElementButtonClicked();

protected:
	/// Returns the current edge selected.
	QString currentEdge() const;
	/// Returns the edge at the given index.
	QString edgeAt(int index) const;

	/// Returns the current edge energy selected.
	double currentEdgeEnergy();
	/// Returns the edge energy at the given index.
	double edgeEnergyAt(int index);

protected:
	/// Button used to choose an element to scan over.
	QToolButton *elementButton_;
	/// The combo box that holds all the edges that can be chosen to scan over.
	QComboBox *edgeComboBox_;
};

#endif // BIOXASXASSCANCONFIGURATIONEDGEEDITOR_H
