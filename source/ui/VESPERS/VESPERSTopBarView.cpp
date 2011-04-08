#include "VESPERSTopBarView.h"
#include "ui/AMShutterButton.h"

#include <QHBoxLayout>

VESPERSTopBarView::VESPERSTopBarView(QWidget *parent) :
	QWidget(parent)
{
	// The shutter buttons.
	AMShutterButton *psh1 = new AMShutterButton("PSH1", "PSH1408-B20-01:state", "PSH1408-B20-01:opr:open", "PSH1408-B20-01:opr:close");
	AMShutterButton *psh2 = new AMShutterButton("PSH2", "PSH1408-B20-02:state", "PSH1408-B20-02:opr:open", "PSH1408-B20-02:opr:close");
	AMShutterButton *ssh1 = new AMShutterButton("SSH1", "SSH1408-B20-01:state");
	AMShutterButton *ssh2 = new AMShutterButton("SSH2", "SSH1607-1-B21-01:state");

	// Shutter layout.
	QHBoxLayout *shStatusLayout = new QHBoxLayout;
	shStatusLayout->addWidget(psh1);
	shStatusLayout->addWidget(psh2);
	shStatusLayout->addWidget(ssh1);
	shStatusLayout->addWidget(ssh2);

	QHBoxLayout *topBarLayout = new QHBoxLayout;
	topBarLayout->addStretch();
	topBarLayout->addLayout(shStatusLayout);

	setLayout(topBarLayout);
}
