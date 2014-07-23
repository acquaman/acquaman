#include "STVariableEditorPlot.h"

STVariableEditorPlot::STVariableEditorPlot(STVariable *toEdit, QWidget *parent) :
    STVariableEditor(toEdit, parent)
{
    // Set variables.

    colorEdited_ = false;
    showMarkersEdited_ = false;

    // Create UI.

    QGridLayout *mainLayout = new QGridLayout();

    QLabel *colorLabel = new QLabel("Color: ", this);
    colorButton_ = new STColorButton(this);
    colorLabel->setBuddy(colorButton_);
    mainLayout->addWidget(colorLabel, 0, 0);
    mainLayout->addWidget(colorButton_, 0, 1);

    showMarkers_ = new QCheckBox("Show markers", this);
    mainLayout->addWidget(showMarkers_, 1, 1);

    QVBoxLayout *stretch = new QVBoxLayout();
    stretch->addStretch();
    mainLayout->addLayout(stretch, 2, 0);

    setLayout(mainLayout);

    // Get current settings.

    colorButton_->setEnabled(false);
    showMarkers_->setEnabled(false);

    getVariableInfo();

    // Make connections.

    connectUI();
}

STVariableEditorPlot::~STVariableEditorPlot()
{

}

void STVariableEditorPlot::applyChanges()
{
    if (variable_) {
        if (colorEdited_) {
            variable_->setColor(colorButton_->color());
            colorEdited_ = false;
        }

        if (showMarkersEdited_) {
            variable_->enableMarkers(showMarkers_->checkState() == Qt::Checked);
            showMarkersEdited_ = false;
        }
    }

}

void STVariableEditorPlot::onColorButtonClicked()
{
    colorEdited_ = true;
    applyChanges();
}

void STVariableEditorPlot::onShowMarkersChanged()
{
    showMarkersEdited_ = true;
    applyChanges();
}

void STVariableEditorPlot::getVariableInfo()
{
    if (variable_) {
        colorButton_->setEnabled(true);
        colorButton_->setColor(variable_->color());

        showMarkers_->setEnabled(true);
        showMarkers_->setChecked(variable_->markersEnabled());
    }
}

void STVariableEditorPlot::clearVariableInfo()
{
    colorButton_->setColor(QColor());
    colorButton_->setEnabled(false);

    showMarkers_->setEnabled(false);

    colorEdited_ = false;
    showMarkersEdited_ = false;
}

void STVariableEditorPlot::connectUI()
{
    connect( colorButton_, SIGNAL(clicked()), this, SLOT(onColorButtonClicked()) );
    connect( showMarkers_, SIGNAL(clicked()), this, SLOT(onShowMarkersChanged()) );
}

void STVariableEditorPlot::disconnectUI()
{
    disconnect( colorButton_, SIGNAL(clicked()), this, SLOT(onColorButtonClicked()) );
    disconnect( showMarkers_, SIGNAL(clicked()), this, SLOT(onShowMarkersChanged()) );
}


