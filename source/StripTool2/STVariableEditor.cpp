#include "STVariableEditor.h"

STVariableEditor::STVariableEditor(STVariable *toEdit, QWidget *parent) : STEditor(parent)
{
    // Set variables.

    variable_ = toEdit;

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
    connect( descriptionEntry_, SIGNAL(textChanged(QString)), this, SLOT(onDescriptionEntryChanged(QString)) );
    descriptionLabel->setBuddy(descriptionEntry_);
    mainLayout->addWidget(descriptionLabel, 1, 0);
    mainLayout->addWidget(descriptionEntry_, 1, 1);

    QLabel *creationLabel = new QLabel("Created: ", this);
    creation_ = new QLabel(this);
    creationLabel->setBuddy(creation_);
    mainLayout->addWidget(creationLabel, 2, 0);
    mainLayout->addWidget(creation_, 2, 1);

    QLabel *connectedLabel = new QLabel("Connected: ", this);
    connected_ = new QLabel(this);
    connectedLabel->setBuddy(connected_);
    mainLayout->addWidget(connectedLabel, 3, 0);
    mainLayout->addWidget(connected_, 3, 1);

    QLabel *valueLabel = new QLabel("Latest value: ", this);
    value_ = new QLabel(this);
    valueLabel->setBuddy(value_);
    mainLayout->addWidget(valueLabel, 4, 0);
    mainLayout->addWidget(value_, 4, 1);

    QLabel *unitsLabel = new QLabel("Units: ", this);
    unitsEntry_ = new QLineEdit(this);
    connect( unitsEntry_, SIGNAL(textChanged(QString)), this, SLOT(onUnitsEntryChanged(QString)) );
    unitsLabel->setBuddy(unitsEntry_);
    mainLayout->addWidget(unitsLabel, 5, 0);
    mainLayout->addWidget(unitsEntry_, 5, 1);

    setLayout(mainLayout);

    // Get current settings.

    getVariableInfo();
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
        disconnectVariable();
        clearVariableInfo();
        variable_ = 0;
    }

    if (variable_ != toEdit) {
        variable_ = toEdit;
        getVariableInfo();
        connectVariable();
        emit variableChanged(variable_);

    } else {
        emit variableChanged(0);
    }

}

void STVariableEditor::applyChanges()
{
    qDebug() << "Applying changes from variable editor.";

    if (variable_) {
        if (descriptionEdited_)
            variable_->setDescription(descriptionEntry_->text());

        if (unitsEdited_)
            variable_->setUnits(unitsEntry_->text());
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
}

void STVariableEditor::onUnitsEntryChanged(const QString &text)
{
    Q_UNUSED(text)

    unitsEdited_ = true;
}

void STVariableEditor::getVariableInfo()
{
    if (variable_) {
        name_->setText(variable_->name());
        descriptionEntry_->setText(variable_->description());
        unitsEntry_->setText(variable_->units());
        creation_->setText(variable_->created().toString());
        setConnectionState(variable_->isConnected());
        setLatestValue(variable_->value());
    }
}

void STVariableEditor::clearVariableInfo()
{
    name_->clear();
    descriptionEntry_->clear();
    unitsEntry_->clear();
    creation_->clear();
    connected_->clear();
    value_->clear();

    descriptionEdited_ = false;
    unitsEdited_ = false;
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
