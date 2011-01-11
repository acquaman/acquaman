#ifndef AMORDEREDSETSIGNALSOURCE_H
#define AMORDEREDSETSIGNALSOURCE_H

#include <QObject>

/// Template classes cannot have signals or slots, so this class provides a proxy to emit signals from any AMOrderedSet<key,value> class.  Simply connect to the AMOrderedSet::signalSource() object.  You should never need to create an instance of this object directly.

class AMOrderedSetSignalSource : public QObject {
	Q_OBJECT

public:
	AMOrderedSetSignalSource();

	/// Do not call this function; it should be considered only available to AMOrderedSet.
	void emitItemAboutToBeAdded(int atIndex) { emit itemAboutToBeAdded(atIndex); }
	/// Do not call this function; it should be considered only available to AMOrderedSet.
	void emitItemAdded(int atIndex) { emit itemAdded(atIndex); }
	/// Do not call this function; it should be considered only available to AMOrderedSet.
	void emitItemAboutToBeRemoved(int atIndex) { emit itemAboutToBeRemoved(atIndex); }
	/// Do not call this function; it should be considered only available to AMOrderedSet.
	void emitItemRemoved(int atIndex) { emit itemRemoved(atIndex); }
	/// Do not call this function; it should be considered only available to AMOrderedSet
	void emitItemChanged(int index) { emit itemChanged(index); }

signals:
	/// Emitted just before a new item is added; the new item will end up at \c index, but it's not there yet.
	void itemAboutToBeAdded(int index);
	/// Emitted after a new item is added at \c index
	void itemAdded(int index);
	/// Emitted before an item is removed from the position at \c index
	void itemAboutToBeRemoved(int index);
	/// Emitted after an item is removed from the position at \c index. It's no longer there.
	void itemRemoved(int index);
	/// Emitted after the item at \c index is replaced, or (possibly) changed in some way. Currently only signalling on replacement is guaranteed.
	void itemChanged(int index);
};

#endif // AMORDEREDSETSIGNALSOURCE_H
