#ifndef BIOXASXASSCANCONFIGURATIONENERGYEDITOR_H
#define BIOXASXASSCANCONFIGURATIONENERGYEDITOR_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QToolButton>
#include <QComboBox>
#include <QLayout>

#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"
#include "ui/acquaman/AMScanConfigurationView.h"

class AMElement;
class AMAbsorptionEdge;

class BioXASXASScanConfigurationEnergyEditor : public AMScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASXASScanConfigurationEnergyEditor(BioXASXASScanConfiguration *configuration, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASXASScanConfigurationEnergyEditor();

	/// Returns the scan configuration being viewed.
	const AMScanConfiguration* configuration() const;

signals:
	/// Notifier that the scan configuration being viewed has changed.
	void configurationChanged(BioXASXASScanConfiguration *newConfiguration);

public slots:
	/// Sets the scan configuration being viewed.
	void setConfiguration(BioXASXASScanConfiguration *newConfiguration);

	/// Clears the view.
	void clear();
	/// Updates the view.
	void update();
	/// Refreshes the view.
	void refresh();

protected slots:
	/// Sets the element information displayed.
	void setElement(AMElement *newElement);

	/// Appends the information for the given absorption edge to the line choice combobox.
	void addLineChoiceEdge(const AMAbsorptionEdge &edge);

	/// Updates the energy spinbox with the configuration's current energy.
	void updateEnergySpinBox();
	/// Updates the element choice button.
	void updateElementChoiceButton();
	/// Updates the line choice combobox.
	void updateLineChoiceComboBox();

	/// Updates the scan configuration energy to match the energy spinbox value.
	void updateConfigurationEnergy();
	/// Updates the scan configuration edge to match the line choice value.
	void updateConfigurationEdge();

	/// Updates the element to correspond to the configuration's current edge.
	void updateElement();

	/// Sets the configuration's energy.
	void setConfigurationEnergy(double newEnergy);
	/// Sets the configuration's edge.
	void setConfigurationEdge(const QString &edge);

	void onElementChoiceClicked();
	void onLineChoiceCurrentIndexChanged(int newIndex);

protected:
	/// Returns a string representing the line choice at the given index.
	QString lineChoiceAt(int index) const;
	/// Returns a string representing the current line choice.
	QString currentLineChoice() const;

	/// Returns the energy of the line choice at the given index.
	double lineChoiceEnergyAt(int index) const;
	/// Returns the energy corresponding to the current line choice.
	double currentLineChoiceEnergy() const;

	/// Returns the configuration's element (symbol).
	QString configurationElement() const;

protected:
	/// The scan configuration being viewed.
	BioXASXASScanConfiguration *configuration_;
	/// The element being viewed.
	AMElement *element_;

	/// Double spin box for changing the energy.
	QDoubleSpinBox *energy_;
	/// Button used to choose an element to scan over.
	QToolButton *elementChoice_;
	/// The combo box that holds all the lines that can be chosen to scan over.
	QComboBox *lineChoice_;
};

#endif // BIOXASXASSCANCONFIGURATIONENERGYEDITOR_H
