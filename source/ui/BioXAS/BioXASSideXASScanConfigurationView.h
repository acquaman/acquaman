#ifndef BIOXASSIDEXASSCANCONFIGURATIONVIEW_H
#define BIOXASSIDEXASSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/BioXASSide/BioXASSideXASScanConfiguration.h"

class BioXASSideXASScanConfigurationView : public AMScanConfigurationView
{

Q_OBJECT

public:
    BioXASSideXASScanConfigurationView(BioXASSideXASScanConfiguration *configuration, QWidget *parent = 0);
    virtual ~BioXASSideXASScanConfigurationView();

    /// Getter for the configuration.
    const AMScanConfiguration *configuration() const { return configuration_; }

protected:
    BioXASSideXASScanConfiguration *configuration_;
};

#endif // BIOXASSIDEXASSCANCONFIGURATIONVIEW_H
