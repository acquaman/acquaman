#include "AMTESTCentralWidgetView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "ui/TimeEvolutionStripTool/AMTESTServerConnectionButton.h"
#include "util/TimeEvolutionStripTool/AMTESTStripTool.h"

AMTESTCentralWidgetView::AMTESTCentralWidgetView(QWidget *parent)
	: QWidget(parent)
{
	AMTESTServerConnectionButton *scalerButton = new AMTESTServerConnectionButton(AMTESTStripTool::stripTool()->serverConnectionByName("Scaler"));
	AMTESTServerConnectionButton *ampteksButton = new AMTESTServerConnectionButton(AMTESTStripTool::stripTool()->serverConnectionByName("Ampteks"));
	AMTESTServerConnectionButton *pvsButton = new AMTESTServerConnectionButton(AMTESTStripTool::stripTool()->serverConnectionByName("PVs"));
	pvsButton->setDisabled(true);

	QHBoxLayout *connectionButtonsLayout = new QHBoxLayout;
	connectionButtonsLayout->addWidget(scalerButton);
	connectionButtonsLayout->addWidget(ampteksButton);
	connectionButtonsLayout->addWidget(pvsButton);

	QVBoxLayout *centralWidgetLayout = new QVBoxLayout;
	centralWidgetLayout->addLayout(connectionButtonsLayout);
	centralWidgetLayout->addStretch();

	setLayout(centralWidgetLayout);
}

