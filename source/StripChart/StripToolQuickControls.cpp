#include "StripToolQuickControls.h"

StripToolQuickControls::StripToolQuickControls(QWidget *parent) :
    QWidget(parent)
{
    model_ = 0;

    listView_ = new StripToolListView(this);

    pvNameLineEdit_ = new QLineEdit();
    pvNameLineEdit_->setFocus();
    connect( this, SIGNAL(nameEntryEnabled(bool)), pvNameLineEdit_, SLOT(setEnabled(bool)) );
    connect( this, SIGNAL(nameEntryFocus()), pvNameLineEdit_, SLOT(setFocus()) );
    connect( this, SIGNAL(clearName()), pvNameLineEdit_, SLOT(clear()) );
    connect( pvNameLineEdit_, SIGNAL(returnPressed()), this, SLOT(addClicked()) );
    connect( pvNameLineEdit_, SIGNAL(textEdited(QString)), this, SIGNAL(clearMessage()) );

    addButton_ = new QPushButton("Add");
    connect( addButton_, SIGNAL(clicked()), this, SLOT(addClicked()) );
    connect( this, SIGNAL(buttonEnabled(bool)), addButton_, SLOT(setEnabled(bool)) );

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(pvNameLineEdit_);
    buttonLayout->addWidget(addButton_);

    message_ = new QLabel("");
    connect( this, SIGNAL(clearMessage()), message_, SLOT(clear()) );
    connect( this, SIGNAL(error(QString)), this, SLOT(displayMessage(QString)) );

    QVBoxLayout *controlsLayout = new QVBoxLayout();
    controlsLayout->addWidget(listView_);
    controlsLayout->addLayout(buttonLayout);
    controlsLayout->addWidget(message_);

    setLayout(controlsLayout);
    setMaximumWidth(200);

    connect( this, SIGNAL(pvConnected(bool)), this, SLOT(onPVConnected(bool)) );
    connect( this, SIGNAL(reset()), this, SLOT(resetControls()) );
}



void StripToolQuickControls::setModel(StripToolModel *newModel)
{
    model_ = newModel;

    connect( this, SIGNAL(addPV(QString)), model_, SLOT(toAddPV(QString)) );

    connect( model_, SIGNAL(pvValid(bool)), this, SLOT(resetControls()) );
    connect( model_, SIGNAL(errorMessage(QString)), this, SLOT(displayMessage(QString)) );

    listView_->setPVModel(model_);
}



void StripToolQuickControls::resetControls()
{
    emit clearName();
    emit buttonEnabled(true);
    emit nameEntryEnabled(true);
    emit nameEntryFocus();
}



void StripToolQuickControls::displayMessage(const QString &text)
{
    message_->setText(text);
    emit reset();
}



void StripToolQuickControls::addClicked()
{
    emit buttonEnabled(false);
    emit nameEntryEnabled(false);

    QString pvName = pvNameLineEdit_->text();

    if (pvName == "")
    {
        error("PV name cannot be blank.");

    } else {

        emit addPV(pvName);
    }
}
