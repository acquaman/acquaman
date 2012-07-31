#ifndef AMDATAMANAPPCONTROLLERFORACTIONS3_H
#define AMDATAMANAPPCONTROLLERFORACTIONS3_H

#include "application/AMDatamanAppController.h"

/// This version of AMDatamanAppController is suitable for apps using the 'actions3' actions framework
class AMDatamanAppControllerForActions3 : public AMDatamanAppController
{
    Q_OBJECT
public:
    explicit AMDatamanAppControllerForActions3(QObject *parent = 0);

	/// Re-implemented to create the user database and actions database
	virtual bool startupCreateDatabases();
	/// Re-implemented to register common database classes, and the actions3 database objects.
	virtual bool startupRegisterDatabases();

signals:

public slots:

};

#endif // AMDATAMANAPPCONTROLLERFORACTIONS3_H
