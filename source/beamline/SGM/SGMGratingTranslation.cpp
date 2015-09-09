#include "SGMGratingTranslation.h"

SGMGratingTranslation::SGMGratingTranslation()
{
    gratingSelection_ = UnknownGrating;
}

SGMGratingTranslation::SGMGratingTranslation(GratingSelection gratingSelection)
{
    gratingSelection_ = gratingSelection;
}

double SGMGratingTranslation::spacing() const
{
    switch(gratingSelection_) {
    case LowGrating:
        return 1.689233e-6;
    case MediumGrating:
        return 9.163583e-7;
    case HighGrating:
        return 5.903703e-7;
    default:
        return 0;
    }
}

double SGMGratingTranslation::c1() const
{
    switch(gratingSelection_) {
    case LowGrating:
        return 2.454768e-5;
    case MediumGrating:
        return 2.462036e-5;
    case HighGrating:
        return 2.456910e-5;
    default:
        return 0;
    }
}

double SGMGratingTranslation::c2() const
{
    switch(gratingSelection_) {
    case LowGrating:
        return -1.593923;
    case MediumGrating:
        return -1.590466;
    case HighGrating:
        return -1.594012;
    default:
        return 0;
    }
}

double SGMGratingTranslation::thetaM() const
{
    switch(gratingSelection_) {
    case LowGrating:
        return 3.055748;
    case MediumGrating:
        return 3.054777;
    case HighGrating:
        return 3.054579;
    default:
        return 0;
    }
}

double SGMGratingTranslation::radiusCurvatureOffset() const
{
    switch(gratingSelection_) {
    case LowGrating:
        return 5.0946788e2;
    case MediumGrating:
        return 5.112918e2;
    case HighGrating:
        return 5.104646e2;
    default:
        return 0;
    }
}

SGMGratingTranslation::GratingSelection SGMGratingTranslation::gratingSelection() const
{
    return gratingSelection_;
}

