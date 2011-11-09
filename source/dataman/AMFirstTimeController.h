/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMFIRSTTIMECONTROLLER_H
#define AMFIRSTTIMECONTROLLER_H



#include "util/AMErrorMonitor.h"
#include "util/AMSettings.h"
#include "dataman/database/AMDatabase.h"
#include "ui/dataman/AMFirstTimeWidget.h"




/// This controller supervises a process to ensure that user settings, the user data folder, and the database are ready for use. To use it, call AMFirstTimeController::firstTimeCheck() to automatically check and conduct any initialization that needs to happen.  Alternatively, you can call isFirstTime() to find out what the situation is, and call onFirstTime() or onEveryTime() accordingly.
class AMFirstTimeController : public QObject
{
Q_OBJECT
public:

	/// It's recommended to call this function on application startup.  It checks if this is the first time the application has run, and calls onFirstTime() if that's true, otherwise it calls onEveryTime().  Returns true of everything that needs to happen is completed successfully.
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

	/// Call this function on every other program startup (ie: when onFirstTime() is not called). Returns true on success
	static bool onEveryTime();


protected:
	/// register all standard database objects with the database system.
	static void registerAllDatabaseClasses() ;


};







#endif // AMFIRSTTIMECONTROLLER_H
