#ifndef AMFIRSTTIMECONTROLLER_H
#define AMFIRSTTIMECONTROLLER_H



#include "AMErrorMonitor.h"
#include <AMSettings.h>
#include <dataman/AMDatabase.h>
#include <ui/AMFirstTimeWidget.h>




/// This controller supervises a process to ensure that user settings, the user data folder, and the database are ready for use. To use it, call AMFirstTimeController::firstTimeCheck() to automatically check and conduct any initialization that needs to happen.  Alternatively, you can call isFirstTime() to find out what the situation is, and call onFirstTime() or onEveryTime() accordingly.
class AMFirstTimeController : public QObject
{
Q_OBJECT
public:

	/// It's recommended to call this function on application startup.  It checks if this is the first time the application has run, and calls onFirstTime() if that's true; otherwise it calls onEveryTime().  Returns true of everything that needs to happen is completed successfully.
	static bool firstTimeCheck() {
		if(isFirstTime())
			return onFirstTime();
		else
			return onEveryTime();
	}





	/// This function returns true if the application has ran before, and returns false if this looks to be the first time (and some initialization is required)
	static bool isFirstTime();

	/// This function supervises a process to ensure that user settings, the user data folder, and the database are ready for use. Returns true on success.
	static bool onFirstTime();

	/// Call this function on every other program startup (ie: whenever onFirstTime() is not called). Returns true on success
	static bool onEveryTime();


protected:
	/// create structures and tables for a new user database, from scratch
	static bool databaseInitialization() ;

	/// Check whether the user database is the most recent version, and migrate if required.
	static bool databaseUpgrade();

};







#endif // AMFIRSTTIMECONTROLLER_H
