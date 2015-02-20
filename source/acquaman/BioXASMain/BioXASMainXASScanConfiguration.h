#ifndef BIOXASMAINXASSCANCONFIGURATION_H
#define BIOXASMAINXASSCANCONFIGURATION_H

#include <QObject>

#include "acquaman/AMStepScanConfiguration.h"

class BioXASMainXASScanConfiguration : public AMStepScanConfiguration
{
    Q_OBJECT

    Q_CLASSINFO("AMDbObject_Attributes", "description=BioXAS Side XAS Scan Configuration")

public:
    /// Constructor.
    Q_INVOKABLE BioXASMainXASScanConfiguration(QObject *parent = 0);
    /// Copy Constructor.
    BioXASMainXASScanConfiguration(const BioXASMainXASScanConfiguration &original);
    ///Destructor.
    virtual ~BioXASMainXASScanConfiguration();
    /// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
    virtual AMScanConfiguration* createCopy() const;
    /// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
    virtual AMScanController* createController();
    /// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
    virtual AMScanConfigurationView* createView();
    /// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
    virtual QString detailedDescription() const;
    /// returns the edge energy.
    double edgeEnergy() const { return edgeEnergy_; }
    /// Returns the name of the current edge.
    QString edge() const { return edge_; }

signals:
    /// Notifier that the edge energy has changed.
    void edgeEnergyChanged(double edgeEnergy);
    /// Notifier that the current edge has changed.
    void edgeChanged(const QString &edge);

public slots:
    /// Sets the edge energy.
    void setEdgeEnergy(double edgeEnergy);
    /// Sets the current edge for the scan.
    void setEdge(QString edgeName);

protected:
    /// The edge energy.
    double edgeEnergy_;
    /// The edge being scanned.
    QString edge_;

};

#endif // BIOXASMAINXASSCANCONFIGURATION_H
