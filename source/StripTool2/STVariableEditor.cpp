#include "STVariableEditor.h"

STVariableEditor::STVariableEditor(STVariable *toEdit, QWidget *parent) : QWidget(parent)
{
    variable_ = toEdit;

    descriptionEdited_ = false;

    setupComponents();
    getVariableInfo();
}

STVariableEditor::~STVariableEditor()
{

}

STVariable* STVariableEditor::variable() const
{
    return variable_;
}

void STVariableEditor::setNameText(const QString &name)
{
    name_->setText(name);
}

void STVariableEditor::setDescriptionText(const QString &description)
{
    descriptionEntry_->setText(description);
}

void STVariableEditor::setCreationText(const QString &creation)
{
    creation_->setText(creation);
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

void STVariableEditor::setupComponents()
{
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

    setLayout(mainLayout);
}

void STVariableEditor::getVariableInfo()
{
    if (variable_) {
        setNameText(variable_->name());
        setDescriptionText(variable_->description());
        setCreationText(variable_->created().toString());
        setConnectionState(variable_->isConnected());
        setLatestValue(variable_->value());

        connect( variable_, SIGNAL(connected(bool)), this, SLOT(setConnectionState(bool)) );
        connect( variable_, SIGNAL(valueChanged(double)), this, SLOT(setLatestValue(double)) );
    }
}

void STVariableEditor::applyChangesToVariable()
{
    if (descriptionEdited_)
        variable_->setDescription(descriptionEntry_->text());
}
