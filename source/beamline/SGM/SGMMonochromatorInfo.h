#ifndef SGMMONOCHROMATORINFO_H
#define SGMMONOCHROMATORINFO_H

#include <QObject>
#include <QHash>
#include "beamline/SGM/SGMGratingTranslation.h"
class SGMMonochromatorInfo : public QObject
{
    Q_OBJECT
public:

    enum UndulatorHarmonic {
        FirstHarmonic = 1,
        ThirdHarmonic = 3
    };

    explicit SGMMonochromatorInfo(QObject *parent = 0);

	QHash<QString, double> buildValues() const;

    void testValue(const QString &name, double requiredEnergy, SGMGratingTranslation::GratingSelection gratingSelection, double expectedAngleEncoderTarget, double expectedExitSlitPosition, double expectedUndulatorPosition) const;

	void testValues() const;

protected:
   static double energyFromGrating(SGMGratingTranslation gratingTranslation,
                                   double gratingAngleEncoderTarget);

   static double gratingAngleEncoderTargetFromEnergy(SGMGratingTranslation gratingTranslation,
                                                     double energy);

   static double undulatorPosition(double energy, UndulatorHarmonic undulatorHarmonic, double undulatorOffset);

   static double exitSlitPosition(SGMGratingTranslation gratingTranslation,
                                  double energy);

};

#endif // SGMMONOCHROMATORINFO_H
