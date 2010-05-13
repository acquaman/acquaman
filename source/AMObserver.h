#ifndef AMOBSERVER_H
#define AMOBSERVER_H

#include "AMObservable.h"

/// The AMObserver defines the interface for classes that can receive updates from AMObservables.  It's up to you to implement onObservableChanged(), and you can interpret the arguments as you like.
class AMObserver {
public:
	AMObserver() {}

	virtual void onObservableChanged(AMObservable* source, int code, const char* msg) = 0;

};

#endif // AMOBSERVER_H
