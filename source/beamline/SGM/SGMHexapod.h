#ifndef SGMHEXAPOD_H
#define SGMHEXAPOD_H

#include <QObject>
#include "beamline/AMControlSet.h"

#define HEXAPOD_POSITION_TOLERANCE 0.008
/*!
  * A class which represents the AMPIC887 Controlled hexapod within the SGM
  * endstation.
  */
class AMControl;
class AMWaveformBinningSinglePVControl;
class SGMHexapodTransformedAxis;
class AM3DRotatedSystemControl;
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

	/// Returns the hexapod control set.
	AMControlSet *allHexapodControls() const;

	/*!
	  * The motor for the X Axis of the hexapod
	  */
	AMControl* xAxisPrimeControl() const;

	/*!
	  * The motor for the Y Axis of the hexapod
	  */
	AMControl* yAxisPrimeControl() const;

	/*!
	  * The motor for the Z Axis of the hexapod
	  */
	AMControl* zAxisPrimeControl() const;

	/*!
	  * The setpoint for the X Axis component of a trajectory move.
	  */
	AMControl* xAxisPrimeTrajectoryControl() const;

	/*!
	  * The setpoint for the Y Axis component of a trajectory move.
	  */
	AMControl* yAxisPrimeTrajectoryControl() const;

	/*!
	  * The setpoint for the Z Axis component of a trajectory move.
	  */
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

	/*!
	  * The control for the active status of the data recorder. NOTE: When active
	  * the data recorder will cause motions to pause at their final position
	  * for a noticable period of time before setting their status to MOVE DONE.
	  */
	AMControl* dataRecorderStatus() const;

	/*!
	  * The control for the sample rate (in Hz) which the data recorder samples
	  * the axis positions at.
	  */
	AMControl* dataRecorderRate() const;

	/*!
	  * The waveform control containing the data recorded for the X axis during
	  * the last move which the recorder was active for.
	  * NOTE: The data contained within is for the global, untransformed X Axis.
	  * See transformVectors() for a way of converting this data to the current
	  * rotated axes of the hexapod.
	  */
	AMWaveformBinningSinglePVControl* xGlobalRecorderControl() const;

	/*!
	  * The waveform control containing the data recorded for the Y axis during
	  * the last move which the recorder was active for.
	  * NOTE: The data contained within is for the global, untransformed Y Axis.
	  * See transformVectors() for a way of converting this data to the current
	  * rotated axes of the hexapod.
	  */
	AMWaveformBinningSinglePVControl* yGlobalRecorderControl() const;

	/*!
	  * The waveform control containing the data recorded for the Z axis during
	  * the last move which the recorder was active for.
	  * NOTE: The data contained within is for the global, untransformed Z Axis.
	  * See transformVectors() for a way of converting this data to the current
	  * rotated axes of the hexapod.
	  */
	AMWaveformBinningSinglePVControl* zGlobalRecorderControl() const;

	/*!
	  * The time data control containing the delta time data recorded during the
	  * last move which the recorder was active for.
	  */
	AMWaveformBinningSinglePVControl* timeRecorderControl() const;

	/*!
	  * Rotates the coordinate system used by the hexapod by the provided values,
	  * applied as Euler angles in order rX, rY, rZ.
	  * \param rX ~ The amount to rotate the system about the X Axis (1st)
	  * \param rY ~ The amount to rotate the system about the Y Axis (2nd)
	  * \param rZ ~ The amount to rotate the system about the Z Axis (3rd)
	  */
	void rotateSystem(double rX, double rY, double rZ);

	/*!
	  * Resets the system of the hexapod back to the plane of the base plate.
	  */
	void resetSystem();

	/*!
	  * Transforms the provided QVector of 3D Vectors from the global coordinates
	  * to the current rotated system of the hexapod.
	  * \param coordinates ~ The vector of coordinates which are to be transformed
	  * from the global system to the rotated hexapod's prime system.
	  */
	QList<QVector3D> transformVectors(const QList<QVector3D>& coordinates);

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

	SGMHexapodTransformedAxis* xAxisPrimeControl_;
	SGMHexapodTransformedAxis* yAxisPrimeControl_;
	SGMHexapodTransformedAxis* zAxisPrimeControl_;

	AM3DRotatedSystemControl* xAxisPrimeTrajectoryControl_;
	AM3DRotatedSystemControl* yAxisPrimeTrajectoryControl_;
	AM3DRotatedSystemControl* zAxisPrimeTrajectoryControl_;

	AMControl* systemVelocity_;
	AMControl* stopAll_;


	AMControl* trajectoryStart_;
	AMControl* trajectoryReset_;

	AMControl* dataRecorderRate_;
	AMControl* dataRecorderStatus_;

	AMWaveformBinningSinglePVControl* xGlobalRecorderControl_;
	AMWaveformBinningSinglePVControl* yGlobalRecorderControl_;
	AMWaveformBinningSinglePVControl* zGlobalRecorderControl_;
	AMWaveformBinningSinglePVControl* timeRecorderControl_;


	AMControlSet* allControls_;

};

#endif // SGMHEXAPOD_H
