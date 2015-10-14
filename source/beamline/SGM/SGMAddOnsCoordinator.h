#ifndef SGMADDONSCOORDINATOR_H
#define SGMADDONSCOORDINATOR_H

#include <QObject>

class AMControl;
class SGMAddOnsCoordinator : public QObject
{
    Q_OBJECT
public:
    explicit SGMAddOnsCoordinator(QObject *parent = 0);

signals:

public slots:

protected:
	AMControl* energyFeedback_;
	AMControl* energySetpoint_;
	AMControl* energyStatus_;
	AMControl* energyStop_;

	AMControl* undulatorDetune_;

	AMControl* undulatorHarmonicFeedback_;
	AMControl* undulatorHarmonicSetpoint_;
	AMControl* undulatorHarmonicStatus_;
	AMControl* undulatorHarmonicStop_;

	AMControl* undulatorTracking_;

	AMControl* gratingTranslationOptimizationFeedback_;
	AMControl* gratingTranslationOptimizationSetpoint_;

	AMControl* gratingTranslationSetpoint_;
	AMControl* gratingTranslationFeedback_;
	AMControl* gratingTranslationStatus_;
	AMControl* gratingTranslationStop_;

	AMControl* exitSlitTracking_;
};

#endif // SGMADDONSCOORDINATOR_H
