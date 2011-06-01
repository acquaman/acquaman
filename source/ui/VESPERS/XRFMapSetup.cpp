#include "XRFMapSetup.h"

#include "ui/AMTopFrame.h"

#include <QPushButton>
#include <QGroupBox>
#include <QProcess>
#include <QTextEdit>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>

XRFMapSetup::XRFMapSetup(QWidget *parent) :
	QWidget(parent)
{
	dim2D_ = new QRadioButton("2D");
	dim3D_ = new QRadioButton("3D");
	QButtonGroup *dimGroup = new QButtonGroup;
	dimGroup->addButton(dim2D_);
	dimGroup->addButton(dim3D_);
	dimGroup->setExclusive(true);

	QVBoxLayout *dimLayout = new QVBoxLayout;
	dimLayout->addWidget(dim2D_, 0, Qt::AlignCenter);
	dimLayout->addWidget(dim3D_, 0, Qt::AlignCenter);
	QGroupBox *dimBox = new QGroupBox("Map Dimension");
	dimBox->setLayout(dimLayout);

	vortex1El_ = new QRadioButton("1-el Vortex");
	vortex4El_ = new QRadioButton("4-el Vortex");
	QButtonGroup *detectorGroup = new QButtonGroup;
	detectorGroup->addButton(vortex1El_);
	detectorGroup->addButton(vortex4El_);
	detectorGroup->setExclusive(true);

	QVBoxLayout *vortexLayout = new QVBoxLayout;
	vortexLayout->addWidget(vortex1El_, 0, Qt::AlignCenter);
	vortexLayout->addWidget(vortex4El_, 0, Qt::AlignCenter);
	QGroupBox *vortexBox = new QGroupBox("XRF Detector Choice");
	vortexBox->setLayout(vortexLayout);

	ccd_ = new QCheckBox("Use Roper CCD");
	multipleImages_ = new QCheckBox("Multiple Images per File");
	multipleImages_->setEnabled(false);

	QVBoxLayout *ccdLayout = new QVBoxLayout;
	ccdLayout->addWidget(ccd_);
	ccdLayout->addWidget(multipleImages_);
	QGroupBox *ccdBox = new QGroupBox("CCD options");
	ccdBox->setLayout(ccdLayout);

	QPushButton *start = new QPushButton("Launch Mapping Software");
	connect(start, SIGNAL(clicked()), this, SLOT(launchNDMapper()));

	QVBoxLayout *optionsLayout = new QVBoxLayout;
	optionsLayout->addWidget(dimBox);
	optionsLayout->addWidget(vortexBox);
	optionsLayout->addWidget(ccdBox);
	optionsLayout->addWidget(start);

	QTextEdit *message = new QTextEdit;
	message->setPlainText(QString("This launches the XRF mapping software used on the VESPERS beamline.  It will launch the appropriate version based on the "
									   "selections of different options found to the left.\n\nThe choices of dimension are either a 2D map or a 3D map.  Note that "
									   "a 3D map requires the CCD to be enabled.\n\nThe choice of XRF detector is primarily based on the geometry of the endstation "
									   "and the beamline staff will instruct you on which detector you will be using for your experiment.\n\nIf you are also doing "
									   "a diffraction map, check off the Use Ropper CCD check box.  Currently the Multiple Images per File check box is under "
									   "development and should be left un-checked for now.\n\nOnce everything is setup, click the Launch Mapping Software button."));
	message->setReadOnly(true);

	QHBoxLayout *setupLayout = new QHBoxLayout;
	setupLayout->addStretch();
	setupLayout->addLayout(optionsLayout);
	setupLayout->addWidget(message);
	setupLayout->addStretch();

	AMTopFrame *topFrame = new AMTopFrame("Mapping Software Setup");
	topFrame->setIcon(QIcon(":/utilities-system-monitor.png"));

	QVBoxLayout *masterLayout = new QVBoxLayout;
	masterLayout->addWidget(topFrame);
	masterLayout->addStretch();
	masterLayout->addLayout(setupLayout);
	masterLayout->addStretch();

	setLayout(masterLayout);

	// Some auto connections based on choices.
	connect(dim3D_, SIGNAL(toggled(bool)), ccd_, SLOT(setChecked(bool)));
	connect(dim3D_, SIGNAL(toggled(bool)), ccd_, SLOT(setDisabled(bool)));
	connect(ccd_, SIGNAL(toggled(bool)), multipleImages_, SLOT(setEnabled(bool)));

	dim2D_->setChecked(true);
	vortex1El_->setChecked(true);
}

void XRFMapSetup::launchNDMapper()
{
	if (dim2D_->isChecked() && !ccd_->isChecked()){

		if (vortex1El_->isChecked())
			QProcess::startDetached("konsole -vt_sz 5x5 -e /home/vespers/bin/runNDMapper 2 1 --useCCD=n --multiImage=n");
		else
			QProcess::startDetached("konsole -vt_sz 5x5 -e /home/vespers/bin/runNDMapper 2 2 --useCCD=n --multiImage=n");
	}
	else if (dim2D_->isChecked() && ccd_->isChecked()){

		if (vortex1El_->isChecked()){

			if (multipleImages_->isChecked())
				QProcess::startDetached("konsole -vt_sz 5x5 -e /home/vespers/bin/runNDMapper 2 1 --useCCD=y --multiImage=y");
			else
				QProcess::startDetached("konsole -vt_sz 5x5 -e /home/vespers/bin/runNDMapper 2 1 --useCCD=y --multiImage=n");
		}
		else{

			if (multipleImages_->isChecked())
				QProcess::startDetached("konsole -vt_sz 5x5 -e /home/vespers/bin/runNDMapper 2 2 --useCCD=y --multiImage=y");
			else
				QProcess::startDetached("konsole -vt_sz 5x5 -e /home/vespers/bin/runNDMapper 2 2 --useCCD=y --multiImage=n");
		}
	}
	else if (dim3D_->isChecked() && vortex1El_->isChecked()){

		if (multipleImages_->isChecked())
			QProcess::startDetached("konsole -vt_sz 5x5 -e /home/vespers/bin/runNDMapper 3 1 --useCCD=y --multiImage=y");
		else
			QProcess::startDetached("konsole -vt_sz 5x5 -e /home/vespers/bin/runNDMapper 3 1 --useCCD=y --multiImage=n");
	}
	else if (dim3D_->isChecked() && vortex4El_->isChecked()){

		if (multipleImages_->isChecked())
			QProcess::startDetached("konsole -vt_sz 5x5 -e /home/vespers/bin/runNDMapper 3 2 --useCCD=y --multiImage=y");
		else
			QProcess::startDetached("konsole -vt_sz 5x5 -e /home/vespers/bin/runNDMapper 3 2 --useCCD=y --multiImage=n");
	}
}
