#ifndef BIOXASXASSCANCONFIGURATIONVIEW_H
#define BIOXASXASSCANCONFIGURATIONVIEW_H

#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"
#include "ui/acquaman/AMScanConfigurationView.h"
#include "util/AMPeriodicTable.h"
#include "util/AMAbsorptionEdge.h"
#include "util/AMElement.h"

class BioXASXASScanConfigurationView : public AMScanConfigurationView
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASXASScanConfigurationView(QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASXASScanConfigurationView();

	/// Returns the configuration being viewed.
	const virtual AMScanConfiguration* configuration() const;

signals:
	/// Notifier that the configuration has changed.
	void configurationChanged(BioXASXASScanConfiguration *newConfiguration);

public slots:
	/// Sets the scan configuration.
	virtual void setConfiguration(BioXASXASScanConfiguration *newConfiguration) = 0;

	/// Clears the view.
	virtual void clear() = 0;
	/// Updates the view.
	virtual void update() = 0;
	/// Refreshes the view.
	virtual void refresh() = 0;

protected slots:
	/// Sets the configuration name.
	void setConfigurationName(BioXASXASScanConfiguration *configuration, const QString &newName);
	/// Sets the configuration's edge.
	void setConfigurationEdge(BioXASXASScanConfiguration *configuration, const AMAbsorptionEdge &newEdge);
	/// Sets the configuration's edge.
	void setConfigurationEdge(BioXASXASScanConfiguration *configuration, const QString &newEdgeString);
	/// Sets the configuration's energy.
	void setConfigurationEnergy(BioXASXASScanConfiguration *configuration, double newEnergy);

protected:
	/// Returns the configuration's current element.
	AMElement* configurationElement(BioXASXASScanConfiguration *configuration) const;
	/// Returns a string representation of the configuration's current edge.
	QString configurationEdge(BioXASXASScanConfiguration *configuration) const;

protected:
	/// The configuration being viewed.
	BioXASXASScanConfiguration *configuration_;
};

#endif // BIOXASXASSCANCONFIGURATIONVIEW_H
