#ifndef SGMGRATINGTRANSLATION_H
#define SGMGRATINGTRANSLATION_H

/*!
 * A class representing the grating translation on the SGM Beamline.
 */
class SGMGratingTranslation
{
public:

    enum GratingSelection {
        LowGrating,
        MediumGrating,
        HighGrating,
        UnknownGrating
    };

    SGMGratingTranslation();

    SGMGratingTranslation(GratingSelection gratingSelection);

    double spacing() const;

    double c1() const;

    double c2() const;

    double thetaM() const;

    double radiusCurvatureOffset() const;

    GratingSelection gratingSelection() const;

protected:

    GratingSelection gratingSelection_;
};

#endif // SGMGRATINGTRANSLATION_H
