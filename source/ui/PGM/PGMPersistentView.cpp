#include "PGMPersistentView.h"

#include "beamline/PGM/PGMBeamline.h"
#include "ui/beamline/AMExtendedControlEditor.h"

#include <QHBoxLayout>
#include <QGroupBox>
#include <QVBoxLayout>

PGMPersistentView::PGMPersistentView(QWidget *parent) :
    QWidget(parent)
{
    // Main layout
    QVBoxLayout *mainPanelLayout = new QVBoxLayout;

    // Note: Lucia perfers her beamline to be referreed to by it's full acroynm. It's not nessisary to use
    // the full acroynm in the code but whenever it's displayed to user please try to use VLS-PGM.
    QGroupBox *persistentPanel = new QGroupBox("VLS-PGM Beamline");
    persistentPanel->setLayout(mainPanelLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(persistentPanel);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setMaximumHeight(1000);
    setMinimumHeight(800);
    setMaximumWidth(400);
    setMinimumWidth(400);

    setLayout(mainLayout);

}
