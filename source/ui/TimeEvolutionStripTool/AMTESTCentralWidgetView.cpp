#include "AMTESTCentralWidgetView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "ui/TimeEvolutionStripTool/AMTESTServerConnectionButton.h"
#include "util/TimeEvolutionStripTool/AMTESTServerConfiguration.h"
#include "util/TimeEvolutionStripTool/AMTESTServerConnection.h"

AMTESTCentralWidgetView::AMTESTCentralWidgetView(QWidget *parent)
	: QWidget(parent)
{
	AMTESTServerConfiguration scaler("10.52.48.1", 28044);
	AMTESTServerConfiguration ampteks("10.52.48.40", 28044);
	AMTESTServerConfiguration pvs("", 0);

	AMTESTServerConnection *scalerConnection = new AMTESTServerConnection("Scaler", scaler);
	AMTESTServerConnection *ampteksConnection = new AMTESTServerConnection("Ampteks", ampteks);
	AMTESTServerConnection *pvsConnection = new AMTESTServerConnection("PVs", pvs);

	AMTESTServerConnectionButton *scalerButton = new AMTESTServerConnectionButton(scalerConnection);
	AMTESTServerConnectionButton *ampteksButton = new AMTESTServerConnectionButton(ampteksConnection);
	AMTESTServerConnectionButton *pvsButton = new AMTESTServerConnectionButton(pvsConnection);
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

