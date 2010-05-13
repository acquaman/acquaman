#ifndef AMOBSERVABLE_H
#define AMOBSERVABLE_H

#include <QList>

class AMObserver;

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
  virtual void Emit(int code, const char* msg = "");

protected:

  QList<AMObserver*> observers_;

};

#endif // AMOBSERVABLE_H
