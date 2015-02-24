#ifndef BIOXASSSRLMONOCHROMATOR_H
#define BIOXASSSRLMONOCHROMATOR_H

#include <QObject>

class BioXASSSRLMonochromator : public QObject
{
	Q_OBJECT

public:
	/// Enumerates the possible mono region states. Each region state corresponds to a different active crystal pair.
	enum Region { None = 0, A, B };
	/// Constructor.
	explicit BioXASSSRLMonochromator(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromator();
	/// Returns true if the mono is connected, false otherwise.
	bool isConnected() const { return false; }
	/// Returns the current region.
	Region region() const { return None; }

signals:
	/// Notifier that the mono's connected state has changed.
	void connected(bool isConnected);

public slots:

};

#endif // BIOXASSSRLMONOCHROMATOR_H
