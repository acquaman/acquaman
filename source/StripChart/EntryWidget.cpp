#include "EntryWidget.h"

EntryWidget::EntryWidget(QWidget *parent) :
    QWidget(parent)
{
    lineEdit_ = 0;
    addButton_ = 0;

    buildComponents();
    makeConnections();
    defaultSettings();
}



EntryWidget::~EntryWidget()
{
}



void EntryWidget::onAddButtonClicked()
{
    QString textEntered = lineEdit_->text();

    if (textEntered == ""){
        QMessageBox errorBox;
        errorBox.setText("Entry cannot be empty.");
        errorBox.exec();

    } else {
        emit entryComplete(textEntered);
    }

    emit reset();
}



void EntryWidget::toTestSignal(const QString &textEntered)
{
    qDebug() << "NameEntryWidget :: name entered : " << textEntered;
}



void EntryWidget::buildComponents()
{
    lineEdit_ = new QLineEdit();
    addButton_ = new QPushButton("Add");
}



void EntryWidget::makeConnections()
{
    connect( lineEdit_, SIGNAL(returnPressed()), this, SLOT(onAddButtonClicked()) );
    connect( this, SIGNAL(reset()), lineEdit_, SLOT(clear()) );
    connect( addButton_, SIGNAL(clicked()), this, SLOT(onAddButtonClicked()) );
    connect( this, SIGNAL(entryComplete(QString)), this, SLOT(toTestSignal(QString)) );
}



void EntryWidget::defaultSettings()
{
    QHBoxLayout *entryLayout = new QHBoxLayout();
    entryLayout->addWidget(lineEdit_);
    entryLayout->addWidget(addButton_);

    setLayout(entryLayout);
}
