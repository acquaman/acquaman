#include "STVariableEditorPlot.h"

STVariableEditorPlot::STVariableEditorPlot(STVariable *toEdit, QWidget *parent) :
    STVariableEditor(toEdit, parent)
{
    // Set variables.

    colorEdited_ = false;
    lineThicknessEdited_ = false;
    showMarkersEdited_ = false;

    // Create UI.

    QGridLayout *mainLayout = new QGridLayout();

    QLabel *colorLabel = new QLabel("Color: ", this);
    colorButton_ = new STColorButton(this);
    colorLabel->setBuddy(colorButton_);
    mainLayout->addWidget(colorLabel, 0, 0);
    mainLayout->addWidget(colorButton_, 0, 1);

    QLabel *thicknessLabel = new QLabel("Thickness: ", this);
    lineThickness_ = new QSpinBox(this);
    lineThickness_->setMinimum(0);
    lineThickness_->setMaximum(5);
    thicknessLabel->setBuddy(lineThickness_);\
    mainLayout->addWidget(thicknessLabel, 1, 0);
    mainLayout->addWidget(lineThickness_, 1, 1);

    QLabel *markersLabel = new QLabel("Markers: ", this);
    mainLayout->addWidget(markersLabel, 2, 0);

    showMarkers_ = new QCheckBox("Show markers", this);
    mainLayout->addWidget(showMarkers_, 3, 1);

    QVBoxLayout *stretch = new QVBoxLayout();
    stretch->addStretch();
    mainLayout->addLayout(stretch, 4, 0);

    setLayout(mainLayout);

    // Get current settings.

    colorButton_->setEnabled(false);
    lineThickness_->setEnabled(false);
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

        if (lineThicknessEdited_) {
            variable_->setLineThickness(lineThickness_->value());
            lineThicknessEdited_ = false;
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

void STVariableEditorPlot::onLineThicknessChanged()
{
    lineThicknessEdited_ = true;
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

        lineThickness_->setEnabled(true);
        lineThickness_->setValue(variable_->lineThickness());

        showMarkers_->setEnabled(true);
        showMarkers_->setChecked(variable_->markersEnabled());
    }
}

void STVariableEditorPlot::clearVariableInfo()
{
    colorButton_->setColor(QColor());
    colorButton_->setEnabled(false);

    lineThickness_->clear();
    lineThickness_->setEnabled(false);

    showMarkers_->setEnabled(false);

    colorEdited_ = false;
    lineThicknessEdited_ = false;
    showMarkersEdited_ = false;
}

void STVariableEditorPlot::connectUI()
{
    connect( colorButton_, SIGNAL(clicked()), this, SLOT(onColorButtonClicked()) );
    connect( lineThickness_, SIGNAL(valueChanged(int)), this, SLOT(onLineThicknessChanged()) );
    connect( showMarkers_, SIGNAL(clicked()), this, SLOT(onShowMarkersChanged()) );
}

void STVariableEditorPlot::disconnectUI()
{
    disconnect( colorButton_, SIGNAL(clicked()), this, SLOT(onColorButtonClicked()) );
    disconnect( lineThickness_, SIGNAL(valueChanged(int)), this, SLOT(onLineThicknessChanged()) );
    disconnect( showMarkers_, SIGNAL(clicked()), this, SLOT(onShowMarkersChanged()) );
}


