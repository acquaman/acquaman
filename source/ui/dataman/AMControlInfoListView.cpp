#include "AMControlInfoListView.h"

#include <QLineEdit>
#include <QBoxLayout>

 AMControlInfoListView::~AMControlInfoListView(){}
AMControlInfoListView::AMControlInfoListView(const AMControlInfoList &infoList, QMap<int, QString> enumOverrides, QWidget *parent) :
	QGroupBox(parent)
{
	setTitle(infoList.name());

	QVBoxLayout *mainVL = new QVBoxLayout();
	mainVL->setContentsMargins(2,2,2,2);
	setLayout(mainVL);

	bool overridingEnums = false;
	if(enumOverrides.count() > 0)
		overridingEnums = true;

	QGroupBox *tempGroupBox;
	QLineEdit *tempLineEdit;
	QVBoxLayout *tempVL;
	for(int x = 0, size = infoList.count(); x < size; x++){
		tempGroupBox = new QGroupBox(infoList.at(x).description());
		QString lineEditString;
		if(overridingEnums)
			lineEditString = enumOverrides.value(infoList.at(x).value());
		else
			lineEditString = QString("%1").arg(infoList.at(x).value());
		tempLineEdit = new QLineEdit(lineEditString);
		tempVL = new QVBoxLayout();
		tempVL->addWidget(tempLineEdit);
		tempVL->setContentsMargins(2,2,2,2);
		tempGroupBox->setLayout(tempVL);

		mainVL->addWidget(tempGroupBox);
	}

	setFixedSize(300, 200);
}
