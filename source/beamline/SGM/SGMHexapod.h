#ifndef SGMHEXAPOD_H
#define SGMHEXAPOD_H

#include <QObject>
#include "beamline/AMControlSet.h"
/*!
  * A class which represents the AMPIC887 Controlled hexapod within the SGM
  * endstation.
  */
class AMControl;
class SGMHexapod : public QObject
{
    Q_OBJECT
public:
	/*!
	  * Creates an instance of an SGMHexapod
	  */
    explicit SGMHexapod(QObject *parent = 0);

	/*!
	  * Virtual destructor for an SGMHexapod
	  */
	~SGMHexapod() {}	

	/*!
	  * The connected state of all the hexapod controls.
	  * \returns True if all hexapod controls are connected, false otherwise.
	  */
	bool isConnected();

	AMControl* xAxisPrimeControl() const;

	AMControl* yAxisPrimeControl() const;

	AMControl* zAxisPrimeControl() const;

	AMControl* xAxisPrimeTrajectoryControl() const;

	AMControl* yAxisPrimeTrajectoryControl() const;

	AMControl* zAxisPrimeTrajectoryControl() const;

	/*!
	  * The system velocity Control.
	  */
	AMControl* systemVelocity() const;

	/*!
	  * The stop all Control.
	  */
	AMControl* stopAll() const;

	/*!
	  * The trajectory move start control.
	  */
	AMControl* trajectoryStart() const;

	/*!
	  * The trajectory reset control.
	  */
	AMControl* trajectoryReset() const;
signals:

	/*!
	  * Signal which indicates that the connected state of the controls in the
	  * hexapod have been altered.
	  * \param connected ~ The connected state of the hexapod controls. True if
	  * all hexapod controls are connected, false otherwise.
	  */
	void connected(bool connected);
public slots:
protected:

	AMControl* xAxisPrimeControl_;
	AMControl* yAxisPrimeControl_;
	AMControl* zAxisPrimeControl_;

	AMControl* xAxisPrimeTrajectoryControl_;
	AMControl* yAxisPrimeTrajectoryControl_;
	AMControl* zAxisPrimeTrajectoryControl_;

	AMControl* systemVelocity_;
	AMControl* stopAll_;


	AMControl* trajectoryStart_;
	AMControl* trajectoryReset_;

	AMControlSet* allControls_;

};

#endif // SGMHEXAPOD_H
