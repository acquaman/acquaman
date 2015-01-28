#include "BioXASMainMonochromatorCrystalChangeView.h"
#include <QDebug>

BioXASMainMonochromatorCrystalChangeView::BioXASMainMonochromatorCrystalChangeView(BioXASMainMonochromator *mono, QWidget *parent) :
    QWidget(parent)
{
    // Initialize variables.

    mono_ = 0;

    // Create UI elements.

    //     The future home of all the mono crystal change feedback elements.

    // Current settings.

    setMono(mono);
}

BioXASMainMonochromatorCrystalChangeView::~BioXASMainMonochromatorCrystalChangeView()
{

}

void BioXASMainMonochromatorCrystalChangeView::setMono(BioXASMainMonochromator *newMono)
{
    if (mono_ != newMono)
        emit monoChanged(mono_ = newMono);
}
