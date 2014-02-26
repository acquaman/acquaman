#include "StripToolSidePanel.h"

StripToolSidePanel::StripToolSidePanel(QWidget *parent) : QWidget(parent) {
    model_ = 0;

    listView_ = new StripToolListView(this);
    connect( listView(), SIGNAL(listSelectionChanged(QModelIndex)), this, SIGNAL(listSelectionChanged(QModelIndex)) );

    QVBoxLayout *controlsLayout = new QVBoxLayout();
    controlsLayout->addWidget(listView_);

    QGroupBox *controlsGroup = new QGroupBox();
    controlsGroup->setLayout(controlsLayout);

    QVBoxLayout *contentLayout = new QVBoxLayout();
    contentLayout->addWidget(controlsGroup);

    setLayout(contentLayout);
    setMaximumWidth(205);
}



StripToolSidePanel::~StripToolSidePanel() {
}



void StripToolSidePanel::setModel(StripToolModel *newModel) {
    model_ = newModel;

    connect( this, SIGNAL(pausePVs()), model_, SLOT(toPausePVs()) );
    connect( this, SIGNAL(resumePVs()), model_, SLOT(toResumePVs()) );

    listView_->setModel(model_);
}



StripToolModel* StripToolSidePanel::model() const {
    return model_;
}



StripToolListView* StripToolSidePanel::listView() const {
    return listView_;
}



void StripToolSidePanel::setSelectedIndex(const QModelIndex &newSelection) {
    listView()->setSelectedIndex(newSelection);
}



QModelIndex StripToolSidePanel::selectedIndex() const {
    return listView()->selectedIndex();
}
