#ifndef AMDATAMANAPPCONTROLLERFORACTIONS2_H
#define AMDATAMANAPPCONTROLLERFORACTIONS2_H

#include "application/AMDatamanAppController.h"

/// This version of AMDatamanAppController is suitable for apps using the 'actions2' actions framework
class AMDatamanAppControllerForActions2 : public AMDatamanAppController
{
    Q_OBJECT
public:
    explicit AMDatamanAppControllerForActions2(QObject *parent = 0);

	/// Re-implemented to register common database classes, and the common actions2 database objects.
	virtual bool startupRegisterDatabases();

signals:

public slots:

};

#endif // AMDATAMANAPPCONTROLLERFORACTIONS2_H
