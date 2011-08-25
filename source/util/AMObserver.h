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


#ifndef AMOBSERVER_H
#define AMOBSERVER_H

class AMObservable;

/// The AMObserver defines the interface for classes that can receive updates from AMObservables.  It's up to you to implement onObservableChanged(), and you can interpret the arguments as you like.  THIS CLASS IS NOT CURRENTLY USED.
class AMObserver {
public:
	AMObserver() {}

	virtual void onObservableChanged(AMObservable* source, int code, const char* msg, int payload = 0) = 0;

};

#endif // AMOBSERVER_H
