#include "BioXASSideMonoCrystalChangeView.h"

BioXASSideMonoCrystalChangeView::BioXASSideMonoCrystalChangeView(BioXASSideMonochromator *toView, QWidget *parent) :
    QWidget(parent)
{
    // Set variables.

    mono_ = 0;

    // Create UI.

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);



    setLayout(layout);

    // Current conditions.

    setMono(toView);
}

BioXASSideMonoCrystalChangeView::~BioXASSideMonoCrystalChangeView()
{

}

BioXASSideMonochromator* BioXASSideMonoCrystalChangeView::mono() const
{
    return mono_;
}

void BioXASSideMonoCrystalChangeView::setMono(BioXASSideMonochromator *newMono)
{
    if (mono_ != newMono) {

        if (mono_)
            disconnect( mono_, 0, this, 0 );

        mono_ = newMono;

        if (mono_) {
            connect( mono_, SIGNAL(connected(bool)), this, SLOT(onMonoConnectedChanged()) );
        }

        onMonoConnectedChanged();
    }
}

void BioXASSideMonoCrystalChangeView::onMonoConnectedChanged()
{
    if (mono_ && mono_->isConnected()) {
        // if mono is connected...

    } else {
        // if mono is disconnected...
    }
}
