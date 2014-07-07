#include "STPlotEditor.h"

STPlotEditor::STPlotEditor(STPlotWidget *plot, QWidget *parent) : STEditor(parent)
{
    // Set variables.

    plot_ = plot;

    nameChanged_ = false;
    nameShownChanged_ = false;
    timeValueChanged_ = false;
    timeUnitsChanged_ = false;

    // Create UI.

    QGridLayout *mainLayout = new QGridLayout(this);

    QLabel *nameLabel = new QLabel(this);
    nameLabel->setText("Name :");
    nameEntry_ = new QLineEdit(this);
    nameShown_ = new QCheckBox("Name shown", this);
    mainLayout->addWidget(nameLabel, 0, 0);
    mainLayout->addWidget(nameEntry_, 0, 1, 1, 2);
    mainLayout->addWidget(nameShown_, 1, 1, 1, 2);

    QLabel *timeLabel = new QLabel(this);
    timeLabel->setText("Time :");
    timeEntry_ = new QLineEdit(this);
    timeUnits_ = new QComboBox();
    mainLayout->addWidget(timeLabel, 2, 0, 1, 1);
    mainLayout->addWidget(timeEntry_, 2, 1, 1, 1);
    mainLayout->addWidget(timeUnits_, 2, 2, 1, 1);

    setLayout(mainLayout);

    // Get current settings.

    getPlotInfo();

    nameEntry_->setEnabled(false);
    nameShown_->setEnabled(false);

    // Make connections.

    connect( nameEntry_, SIGNAL(textChanged(QString)), this, SLOT(onNameEntryChanged()) );
    connect( nameShown_, SIGNAL(stateChanged(int)), this, SLOT(onNameShownChanged()) );
    connect( timeEntry_, SIGNAL(textChanged(QString)), this, SLOT(onTimeValueChanged()) );
    connect( timeUnits_, SIGNAL(currentIndexChanged(QString)), this, SLOT(onTimeUnitsChanged()) );
}

STPlotEditor::~STPlotEditor()
{

}

void STPlotEditor::applyChanges()
{
    if (plot_) {
        if (nameChanged_)
            plot_->setPlotName(nameEntry_->text());

        if (nameShownChanged_)
            plot_->showPlotName(nameShown_->checkState() == Qt::Checked);

        if (timeValueChanged_) {
            plot_->time()->setValue(timeEntry_->text().toInt());
        }

        if (timeUnitsChanged_) {
            plot_->time()->setUnits(STTime::stringToUnits(timeUnits_->currentText()));
        }

        if (plot_->timeFilteringEnabled() && timeEntry_->text() == "")
            plot_->enableTimeFiltering(false);

        else if (!plot_->timeFilteringEnabled() && timeEntry_->text() != "")
            plot_->enableTimeFiltering(true);
    }
}

void STPlotEditor::onNameEntryChanged()
{
    nameChanged_ = true;
}

void STPlotEditor::onNameShownChanged()
{
    nameShownChanged_ = true;
}

void STPlotEditor::onTimeValueChanged()
{
    if (timeEntry_->text() != "")
        timeValueChanged_ = true;
}

void STPlotEditor::onTimeUnitsChanged()
{
    timeUnitsChanged_ = true;
}

void STPlotEditor::getPlotInfo()
{
    if (plot_) {
        nameEntry_->setText(plot_->plotName());
        nameShown_->setChecked(plot_->plotNameVisible());

        if (plot_->timeFilteringEnabled())
            timeEntry_->setText( QString::number(plot_->time()->value()) );

        timeUnits_->addItems(STTime::unitsList());
        timeUnits_->setCurrentIndex(plot_->time()->units());
    }
}

void STPlotEditor::clearPlotInfo()
{
    nameEntry_->clear();
    nameShown_->setChecked(false);
    timeEntry_->clear();
    timeUnits_->clear();
}
