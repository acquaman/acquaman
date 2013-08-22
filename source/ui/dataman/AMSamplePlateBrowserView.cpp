#include "AMSamplePlateBrowserView.h"

#include <QListView>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

#include "dataman/AMSamplePlate.h"

AMSamplePlateBrowserView::AMSamplePlateBrowserView(AMSamplePlateBrowser *samplePlateBrowser, QWidget *parent) :
	QWidget(parent)
{
	samplePlateBrowser_ = samplePlateBrowser;
	samplePlatesListView_ = new QListView();
	samplePlatesListView_->setModel(samplePlateBrowser_);
	samplePlatesListView_->setAlternatingRowColors(true);

	selectButton_ = new QPushButton("Select");
	cancelButton_ = new QPushButton("Cancel");

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(samplePlatesListView_);

	QHBoxLayout *hl = new QHBoxLayout();
	hl->addWidget(selectButton_);
	hl->addWidget(cancelButton_);
	vl->addLayout(hl);

	setLayout(vl);

	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(close()));
	connect(selectButton_, SIGNAL(clicked()), this, SLOT(onSelectButtonClicked()));
}

void AMSamplePlateBrowserView::onSelectButtonClicked(){
	QModelIndex selectedPlate = samplePlatesListView_->selectionModel()->currentIndex();
	if(selectedPlate.isValid()){
		AMSamplePlate *selectedSamplePlate = qobject_cast<AMSamplePlate*>(samplePlateBrowser_->data(selectedPlate, AM::PointerRole).value<QObject*>());
		if(selectedSamplePlate)
			emit samplePlateSelected(selectedSamplePlate);
	}
}

AMSamplePlateCreationDialog::AMSamplePlateCreationDialog(QWidget *parent) :
	QDialog(parent)
{
	samplePlateNameLineEdit_ = new QLineEdit("New Sample Plate");
	QLabel *samplePlateNameLabel = new QLabel("Enter Sample Plate Name:");

	okButton_ = new QPushButton("Ok");
	cancelButton_ = new QPushButton("Cancel");

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(samplePlateNameLabel);
	vl->addWidget(samplePlateNameLineEdit_);

	QHBoxLayout *hl = new QHBoxLayout();
	hl->addWidget(okButton_);
	hl->addWidget(cancelButton_);

	vl->addLayout(hl);

	setLayout(vl);

	okButton_->setEnabled(false);
	samplePlateNameLineEdit_->selectAll();
	samplePlateNameLineEdit_->setFocus();

	connect(okButton_, SIGNAL(clicked()), this, SLOT(accept()));
	connect(samplePlateNameLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(onSamplePlateNameLineEditTextChanged(QString)));
}

QString AMSamplePlateCreationDialog::samplePlateName() const{
	return samplePlateNameLineEdit_->text();
}

void AMSamplePlateCreationDialog::onSamplePlateNameLineEditTextChanged(const QString &name){
	if(!name.isEmpty())
		okButton_->setEnabled(true);
	else
		okButton_->setEnabled(false);
}
