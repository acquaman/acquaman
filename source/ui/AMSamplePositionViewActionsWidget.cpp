#include "AMSamplePositionViewActionsWidget.h"
#include "ui_AMSamplePositionViewActionsWidget.h"

AMSamplePositionViewActionsWidget::AMSamplePositionViewActionsWidget(int row, QWidget *parent) :
	QFrame(parent),
	ui(new Ui::AMSamplePositionViewActionsWidget)
{
	row_ = row;

	ui->setupUi(this);

	QStyle* style = QApplication::style();
	ui->markButton->setIcon(style->standardIcon(QStyle::SP_DialogSaveButton));
	ui->moveToButton->setIcon(style->standardIcon(QStyle::SP_ArrowRight));
	ui->removeButton->setIcon(style->standardIcon(QStyle::SP_TitleBarCloseButton));

	ui->moveToButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	ui->markButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

	ui->markButton->setToolTip("Memorize the current position and associate it with this sample");
	ui->moveToButton->setToolTip("Move to this sample's stored position now");
	ui->removeButton->setToolTip("Remove this sample from the sample plate");


	connect(ui->markButton, SIGNAL(clicked()), this, SLOT(onMarkButtonPressed()));
	connect(ui->moveToButton, SIGNAL(clicked()), this, SLOT(onMoveToButtonPressed()));
	connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(onRemoveButtonPressed()));
}

AMSamplePositionViewActionsWidget::~AMSamplePositionViewActionsWidget()
{
	delete ui;
}
