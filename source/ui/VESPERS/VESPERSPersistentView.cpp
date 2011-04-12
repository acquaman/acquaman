#include "VESPERSPersistentView.h"
#include "ui/AMShutterButton.h"
#include "ui/VESPERS/VESPERSSampleStageView.h"

#include <QGridLayout>
#include <QVBoxLayout>

VESPERSPersistentView::VESPERSPersistentView(QWidget *parent) :
	QWidget(parent)
{
	// The shutter buttons.
	AMShutterButton *psh1 = new AMShutterButton("PSH1", "PSH1408-B20-01:state", "PSH1408-B20-01:opr:open", "PSH1408-B20-01:opr:close");
	AMShutterButton *psh2 = new AMShutterButton("PSH2", "PSH1408-B20-02:state", "PSH1408-B20-02:opr:open", "PSH1408-B20-02:opr:close");
	AMShutterButton *ssh1 = new AMShutterButton("SSH1", "SSH1408-B20-01:state", "SSH1408-B20-01:opr:open", "SSH1408-B20-01:opr:close");
	AMShutterButton *ssh2 = new AMShutterButton("SSH2", "SSH1607-1-B21-01:state", "SSH1607-1-B21-01:opr:open", "SSH1607-1-B21-01:opr:close");

	// Sample stage widget.
	VESPERSSampleStageView *motors = new VESPERSSampleStageView;

	// Shutter layout.
	QGridLayout *shStatusLayout = new QGridLayout;
	shStatusLayout->setSpacing(1);
	shStatusLayout->addWidget(psh1, 0, 0);
	shStatusLayout->addWidget(psh2, 1, 0);
	shStatusLayout->addWidget(ssh1, 0, 1);
	shStatusLayout->addWidget(ssh2, 1, 1);

	QVBoxLayout *persistentLayout = new QVBoxLayout;
	persistentLayout->addLayout(shStatusLayout);
	persistentLayout->addWidget(motors);
	persistentLayout->addStretch();

	setLayout(persistentLayout);
}
