#ifndef SGMHEXAPODMAPSCANACTIONCONTROLLERASSEMBLER_H
#define SGMHEXAPODMAPSCANACTIONCONTROLLERASSEMBLER_H

#include "acquaman/AMScanActionControllerScanAssembler.h"

/// Builds the list of actions that are specifically needed to run a 2D map scan using the Hexapod.
class SGMHexapodMapScanActionControllerAssembler : public AMScanActionControllerScanAssembler
{
	Q_OBJECT

public:
	/// Constructor.
	SGMHexapodMapScanActionControllerAssembler(QObject *parent = 0);
	/// Destructor.
	virtual ~SGMHexapodMapScanActionControllerAssembler(){}

protected:
	/// Builds the whole scan action tree to specifically take work around current design limitations.
	virtual bool generateActionTreeImplmentation();

	/// Generates a list of step positions that the outer "loop".
	QList<double> generateStepAxisPositions(AMScanAxisRegion *region) const;
	/// Generates a list of the start QVector3D for all the trajectory moves.
	QList<QVector3D> generateContinuousAxisStartTrajectories(double continuousStart, const QList<double> &yAxisSetpoints, const QVector3D &templatePosition) const;
	/// Generates a list of the end QVector3D for all the trajectory moves.
	QList<QVector3D> generateContinuousAxisEndTrajectories(double continuousEnd, const QList<double> &yAxisSetpoints, const QVector3D &templatePosition) const;
	/// Generates a continuous scan axis from the provided start and end trajectories.
	AMAction3 *generateContinuousAxisActions(AMScanAxis *continuousAxis, AMControl *hexapodControl, const QVector3D &startTrajectory, const QVector3D &endTrajectory) const;
};

#endif // SGMHEXAPODMAPSCANACTIONCONTROLLERASSEMBLER_H
