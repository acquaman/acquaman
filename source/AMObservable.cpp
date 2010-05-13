#include "AMObservable.h"

#include "AMObserver.h"

void AMObservable::Emit(int code, const char* msg) {
	foreach(AMObserver* observer, observers_)
		observer->onObservableChanged(this, code, msg);
}
