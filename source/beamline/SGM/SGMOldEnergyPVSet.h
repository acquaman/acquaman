#ifndef SGMOLDENERGYPVSET_H
#define SGMOLDENERGYPVSET_H

#include <QObject>
class AMControlSet;
class AMControl;

class SGMOldEnergyPVSet : public QObject
{
    Q_OBJECT
public:
    explicit SGMOldEnergyPVSet(QObject *parent = 0);

	AMControl* undulatorHarmonic() const;

	/// Whether all the controls contained within the set are connected.
	bool isConnected() const;
signals:

	/// Signal which indicates that the connected state of the contained controls
	/// has been altered.
	void connected(bool isConnected);
public slots:
protected:
	AMControlSet* controlSet_;
};

#endif // SGMOLDENERGYPVSET_H
