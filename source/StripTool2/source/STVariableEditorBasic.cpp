#include "STVariableEditorBasic.h"

STVariableEditorBasic::STVariableEditorBasic(STVariable *toEdit, QWidget *parent) :
    STVariableEditor(toEdit, parent)
{
    // Set variables.

    descriptionEdited_ = false;
    unitsEdited_ = false;

    // Create UI.

    QGridLayout *mainLayout = new QGridLayout();

    QLabel *nameLabel = new QLabel("Name: ", this);
    name_ = new QLabel(this);
    nameLabel->setBuddy(name_);
    mainLayout->addWidget(nameLabel, 0, 0);
    mainLayout->addWidget(name_, 0, 1);

    QLabel *descriptionLabel = new QLabel("Description: ", this);
    descriptionEntry_ = new QLineEdit(this);
    descriptionLabel->setBuddy(descriptionEntry_);
    mainLayout->addWidget(descriptionLabel, 1, 0);
    mainLayout->addWidget(descriptionEntry_, 1, 1);

    showDescription_ = new QCheckBox("Show description", this);
    mainLayout->addWidget(showDescription_, 2, 1);

    QLabel *creationLabel = new QLabel("Created: ", this);
    creation_ = new QLabel(this);
    creationLabel->setBuddy(creation_);
    mainLayout->addWidget(creationLabel, 3, 0);
    mainLayout->addWidget(creation_, 3, 1);

    QLabel *connectedLabel = new QLabel("Connected: ", this);
    connected_ = new QLabel(this);
    connectedLabel->setBuddy(connected_);
    mainLayout->addWidget(connectedLabel, 4, 0);
    mainLayout->addWidget(connected_, 4, 1);

    QLabel *valueLabel = new QLabel("Latest value: ", this);
    value_ = new QLabel(this);
    valueLabel->setBuddy(value_);
    mainLayout->addWidget(valueLabel, 5, 0);
    mainLayout->addWidget(value_, 5, 1);

    QLabel *unitsLabel = new QLabel("Units: ", this);
    unitsEntry_ = new QLineEdit(this);
    unitsLabel->setBuddy(unitsEntry_);
    mainLayout->addWidget(unitsLabel, 6, 0);
    mainLayout->addWidget(unitsEntry_, 6, 1);

    removeButton_ = new QPushButton("Remove", this);
    removeButton_->setEnabled(false);
    mainLayout->addWidget(removeButton_, 7, 1);

    setLayout(mainLayout);

    // Get current settings.

    descriptionEntry_->setEnabled(false);
    showDescription_->setEnabled(false);
    unitsEntry_->setEnabled(false);
    removeButton_->setEnabled(false);

    getVariableInfo();

    // Make connections.

    connectUI();
    connectVariable();
}

STVariableEditorBasic::~STVariableEditorBasic()
{

}

void STVariableEditorBasic::applyChanges()
{
    if (variable_) {
        if (descriptionEdited_) {
            variable_->setDescription(descriptionEntry_->text());
            descriptionEdited_ = false;
        }

        if (showDescriptionEdited_) {
            variable_->setShowDescription(showDescription_->checkState() == Qt::Checked);
            showDescriptionEdited_ = false;
        }

        if (unitsEdited_) {
            variable_->setUnits(unitsEntry_->text());
            unitsEdited_ = false;
        }
    }
}

void STVariableEditorBasic::setConnectionState(bool isConnected)
{
    if (isConnected)
        connected_->setText("Connected");
    else
        connected_->setText("Not connected");
}

void STVariableEditorBasic::setLatestValue(double newValue)
{
    value_->setText(QString::number(newValue));
}

void STVariableEditorBasic::onDescriptionEntryChanged(const QString &text)
{
    Q_UNUSED(text)

    descriptionEdited_ = true;
    applyChanges();
}

void STVariableEditorBasic::onShowDescriptionChanged()
{
    showDescriptionEdited_ = true;
    applyChanges();
}

void STVariableEditorBasic::onUnitsEntryChanged(const QString &text)
{
    Q_UNUSED(text)

    unitsEdited_ = true;
    applyChanges();
}

void STVariableEditorBasic::getVariableInfo()
{
    if (variable_) {
        name_->setText(variable_->name());

        descriptionEntry_->setEnabled(true);
        descriptionEntry_->setText(variable_->description());

        showDescription_->setEnabled(true);
        showDescription_->setChecked(variable_->showDescription());

        creation_->setText(variable_->created().toString());

        setConnectionState(variable_->isConnected());

        setLatestValue(variable_->value());

        unitsEntry_->setEnabled(true);
        unitsEntry_->setText(variable_->units());

        removeButton_->setEnabled(true);
    }
}

void STVariableEditorBasic::clearVariableInfo()
{
    name_->clear();

    descriptionEntry_->clear();
    descriptionEntry_->setEnabled(false);

    showDescription_->setChecked(false);
    showDescription_->setEnabled(false);

    creation_->clear();

    connected_->clear();

    value_->clear();

    unitsEntry_->clear();
    unitsEntry_->setEnabled(false);

    removeButton_->setEnabled(false);

    descriptionEdited_ = false;
    showDescriptionEdited_ = false;
    unitsEdited_ = false;
}

void STVariableEditorBasic::connectVariable()
{
    if (variable_) {
        connect( variable_, SIGNAL(connected(bool)), this, SLOT(setConnectionState(bool)) );
        connect( variable_, SIGNAL(valueChanged(double)), this, SLOT(setLatestValue(double)) );
    }
}

void STVariableEditorBasic::disconnectVariable()
{
    if (variable_) {
        disconnect( variable_, SIGNAL(connected(bool)), this, SLOT(setConnectionState(bool)) );
        disconnect( variable_, SIGNAL(valueChanged(double)), this, SLOT(setLatestValue(double)) );
    }
}

void STVariableEditorBasic::connectUI()
{
    connect( descriptionEntry_, SIGNAL(textChanged(QString)), this, SLOT(onDescriptionEntryChanged(QString)) );
    connect( showDescription_, SIGNAL(clicked()), this, SLOT(onShowDescriptionChanged()) );
    connect( unitsEntry_, SIGNAL(textChanged(QString)), this, SLOT(onUnitsEntryChanged(QString)) );
    connect( removeButton_, SIGNAL(clicked()), this, SIGNAL(removeButtonClicked()) );
}

void STVariableEditorBasic::disconnectUI()
{
    disconnect( descriptionEntry_, SIGNAL(textChanged(QString)), this, SLOT(onDescriptionEntryChanged(QString)) );
    disconnect( showDescription_, SIGNAL(clicked()), this, SLOT(onShowDescriptionChanged()) );
    disconnect( unitsEntry_, SIGNAL(textChanged(QString)), this, SLOT(onUnitsEntryChanged(QString)) );
    disconnect( removeButton_, SIGNAL(clicked()), this, SIGNAL(removeButtonClicked()) );
}
