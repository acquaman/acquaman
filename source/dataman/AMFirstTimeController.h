#ifndef AMFIRSTTIMECONTROLLER_H
#define AMFIRSTTIMECONTROLLER_H



#include "AMErrorMonitor.h"
#include <AMSettings.h>

#include <dataman/AMDatabase.h>
#include <ui/AMFirstTimeWidget.h>




/// This controller supervises a process to ensure that user settings, the user data folder, and the database are ready for use. To use it, simply create an instance of this class and wait for its constructor to finish (ie: "AMFirstTimeController();" or "delete new AMFirstTimeController()" ).
class AMFirstTimeController : public QObject
{
Q_OBJECT
public:
	/// This controller supervises a process to ensure that user settings, the user data folder, and the database are ready for use. If there is a problem, the constructor calls onFirstTime() to rectify it.
	explicit AMFirstTimeController() ;

	void onFirstTime() ;

	/// create structures and tables for a new user database, from scratch
	void databaseInitialization() ;

	/// Check whether the user database is the most recent version, and migrate if required.
	void databaseUpgrade() ;

signals:

public slots:


protected:



};







#endif // AMFIRSTTIMECONTROLLER_H
