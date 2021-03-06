#include "STVariableEditor.h"

STVariableEditor::STVariableEditor(STVariable *toEdit, QWidget *parent) : STEditor(parent)
{
    // Set variables.

    variable_ = toEdit;

    descriptionEdited_ = false;
    unitsEdited_ = false;
    colorEdited_ = false;
    showMarkersEdited_ = false;

    // Create UI.

    QGridLayout *mainLayout = new QGridLayout();

    QLabel *nameLabel = new QLabel("Name: ");
    name_ = new QLabel();
    nameLabel->setBuddy(name_);
    mainLayout->addWidget(nameLabel, 0, 0);
    mainLayout->addWidget(name_, 0, 1);

    QLabel *descriptionLabel = new QLabel("Description: ");
    descriptionEntry_ = new QLineEdit();
    descriptionLabel->setBuddy(descriptionEntry_);
    mainLayout->addWidget(descriptionLabel, 1, 0);
    mainLayout->addWidget(descriptionEntry_, 1, 1);

    QLabel *creationLabel = new QLabel("Created: ");
    creation_ = new QLabel();
    creationLabel->setBuddy(creation_);
    mainLayout->addWidget(creationLabel, 2, 0);
    mainLayout->addWidget(creation_, 2, 1);

    QLabel *connectedLabel = new QLabel("Connected: ");
    connected_ = new QLabel();
    connectedLabel->setBuddy(connected_);
    mainLayout->addWidget(connectedLabel, 3, 0);
    mainLayout->addWidget(connected_, 3, 1);

    QLabel *valueLabel = new QLabel("Latest value: ");
    value_ = new QLabel();
    valueLabel->setBuddy(value_);
    mainLayout->addWidget(valueLabel, 4, 0);
    mainLayout->addWidget(value_, 4, 1);

    QLabel *unitsLabel = new QLabel("Units: ");
    unitsEntry_ = new QLineEdit();
    unitsLabel->setBuddy(unitsEntry_);
    mainLayout->addWidget(unitsLabel, 5, 0);
    mainLayout->addWidget(unitsEntry_, 5, 1);

    QLabel *colorLabel = new QLabel("Color: ");
    colorButton_ = new STColorButton();
    colorLabel->setBuddy(colorButton_);
    mainLayout->addWidget(colorLabel, 6, 0);
    mainLayout->addWidget(colorButton_, 6, 1);

    showMarkers_ = new QCheckBox("Show plot markers");
    mainLayout->addWidget(showMarkers_, 7, 1);

    removeButton_ = new QPushButton("Remove");
    removeButton_->setEnabled(false);
    mainLayout->addWidget(removeButton_, 8, 1);

    setLayout(mainLayout);

    // Get current settings.

    descriptionEntry_->setEnabled(false);
    unitsEntry_->setEnabled(false);
    colorButton_->setEnabled(false);
    showMarkers_->setEnabled(false);
    removeButton_->setEnabled(false);

    getVariableInfo();

    // Make connections.

    connectUI();
    connectVariable();
}

STVariableEditor::~STVariableEditor()
{

}

STVariable* STVariableEditor::variable() const
{
    return variable_;
}

void STVariableEditor::setVariable(STVariable *toEdit)
{
    if (variable_) {
        disconnectUI();
        disconnectVariable();
        clearVariableInfo();
        variable_ = 0;
    }

    variable_ = toEdit;
    getVariableInfo();
    connectUI();
    connectVariable();

    emit variableChanged(variable_);
}

void STVariableEditor::applyChanges()
{
    if (variable_) {
        if (descriptionEdited_) {
            variable_->setDescription(descriptionEntry_->text());
            descriptionEdited_ = false;
        }

        if (unitsEdited_) {
            variable_->setUnits(unitsEntry_->text());
            unitsEdited_ = false;
        }

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

void STVariableEditor::setConnectionState(bool isConnected)
{
    if (isConnected)
        connected_->setText("Connected");
    else
        connected_->setText("Not connected");
}

void STVariableEditor::setLatestValue(double newValue)
{
    value_->setText(QString::number(newValue));
}

void STVariableEditor::onDescriptionEntryChanged(const QString &text)
{
    Q_UNUSED(text)

    descriptionEdited_ = true;
    applyChanges();
}

void STVariableEditor::onUnitsEntryChanged(const QString &text)
{
    Q_UNUSED(text)

    unitsEdited_ = true;
    applyChanges();
}

void STVariableEditor::onColorButtonClicked()
{
    colorEdited_ = true;
    applyChanges();
}

void STVariableEditor::onShowMarkersChanged()
{
    showMarkersEdited_ = true;
    applyChanges();
}

void STVariableEditor::getVariableInfo()
{
    if (variable_) {
        name_->setText(variable_->name());

        descriptionEntry_->setEnabled(true);
        descriptionEntry_->setText(variable_->description());

        creation_->setText(variable_->created().toString());

        setConnectionState(variable_->isConnected());

        setLatestValue(variable_->value());

        unitsEntry_->setEnabled(true);
        unitsEntry_->setText(variable_->units());

        colorButton_->setEnabled(true);
        colorButton_->setColor(variable_->color());

        showMarkers_->setEnabled(true);
        showMarkers_->setChecked(variable_->markersEnabled());

        removeButton_->setEnabled(true);
    }
}

void STVariableEditor::clearVariableInfo()
{
    name_->clear();

    descriptionEntry_->clear();
    descriptionEntry_->setEnabled(false);

    creation_->clear();

    connected_->clear();

    value_->clear();

    unitsEntry_->clear();
    unitsEntry_->setEnabled(false);

    colorButton_->setColor(QColor());
    colorButton_->setEnabled(false);

    showMarkers_->setEnabled(false);

    removeButton_->setEnabled(false);

    descriptionEdited_ = false;
    unitsEdited_ = false;
    colorEdited_ = false;
}

void STVariableEditor::connectVariable()
{
    if (variable_) {
        connect( variable_, SIGNAL(connected(bool)), this, SLOT(setConnectionState(bool)) );
        connect( variable_, SIGNAL(valueChanged(double)), this, SLOT(setLatestValue(double)) );
    }
}

void STVariableEditor::disconnectVariable()
{
    if (variable_) {
        disconnect( variable_, SIGNAL(connected(bool)), this, SLOT(setConnectionState(bool)) );
        disconnect( variable_, SIGNAL(valueChanged(double)), this, SLOT(setLatestValue(double)) );
    }
}

void STVariableEditor::connectUI()
{
    connect( descriptionEntry_, SIGNAL(textChanged(QString)), this, SLOT(onDescriptionEntryChanged(QString)) );
    connect( unitsEntry_, SIGNAL(textChanged(QString)), this, SLOT(onUnitsEntryChanged(QString)) );
    connect( colorButton_, SIGNAL(clicked()), this, SLOT(onColorButtonClicked()) );
    connect( showMarkers_, SIGNAL(clicked()), this, SLOT(onShowMarkersChanged()) );
    connect( removeButton_, SIGNAL(clicked()), this, SIGNAL(removeButtonClicked()) );
}

void STVariableEditor::disconnectUI()
{
    disconnect( descriptionEntry_, SIGNAL(textChanged(QString)), this, SLOT(onDescriptionEntryChanged(QString)) );
    disconnect( unitsEntry_, SIGNAL(textChanged(QString)), this, SLOT(onUnitsEntryChanged(QString)) );
    disconnect( colorButton_, SIGNAL(clicked()), this, SLOT(onColorButtonClicked()) );
    disconnect( showMarkers_, SIGNAL(clicked()), this, SLOT(onShowMarkersChanged()) );
    disconnect( removeButton_, SIGNAL(clicked()), this, SIGNAL(removeButtonClicked()) );
}
