/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#ifndef AMOBSERVABLE_H
#define AMOBSERVABLE_H

#include <QList>

#include "AMObserver.h"

class AMObservable {

	/// AMObservable is a base class that can be inherited to provide a standard interface for notifying AMObserver's of changes to its state.  It's not as safe or as powerful as Qt's signals and slots, but it avoids the problem with the inability to do multiple-inheritance of QObjects.
public:
	AMObservable() {}

public:

	virtual ~AMObservable() {}

  /// connect an observer (ie: subscribe)
  void addObserver ( AMObserver * newguy) {
	  if(!observers_.contains(newguy))
		  observers_ << newguy;
  }

  /// get a reference to the list of subscribers
  const QList<AMObserver*>& observers() const { return observers_; }

  /// remove an observer (ie: unsubscribe)
  void removeObserver(AMObserver * removeMe) {
	  observers_.removeAll(removeMe);
  }

  /// send an update message to all subscribers. (use the arguments for whatever you want, but be careful ; )
  virtual void Emit(int code, const char* msg = "", int payload = 0) {
	  foreach(AMObserver* observer, observers_)
		  observer->onObservableChanged(this, code, msg, payload);
  }


protected:

  QList<AMObserver*> observers_;

};

#endif // AMOBSERVABLE_H
