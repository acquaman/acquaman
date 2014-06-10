/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMBeamlineCameraBrowser.h"

#include <QComboBox>
#include <QBoxLayout>
#include <QFrame>
#include <QLabel>

#include <QMessageBox>
#include <QStringBuilder>

#include "ui/AMCrosshairOverlayVideoWidget.h"

 AMBeamlineCameraBrowser::~AMBeamlineCameraBrowser(){}
AMBeamlineCameraBrowser::AMBeamlineCameraBrowser(QWidget *parent, bool useOpenGlViewport) :
	AMBeamlineCameraWidget(parent, useOpenGlViewport)
{
	setWindowTitle("Video");

	// GUI setup:
	//////////////////////////

	QFrame* sourceFrame = new QFrame();
	QHBoxLayout* shl = new QHBoxLayout();
	shl->setContentsMargins(12,4,12,4);
	shl->addWidget(new QLabel("Video URL:"), 0);
	shl->addWidget(sourceComboBox_ = new QComboBox(), 2);
	sourceComboBox_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	sourceFrame->setLayout(shl);

	qobject_cast<QVBoxLayout*>(layout())->addWidget(sourceFrame, 0);

	// configure source combo box
	///////////////////////////
	sourceComboBox_->setEditable(true);
	sourceComboBox_->setInsertPolicy(QComboBox::InsertAtTop);
	sourceComboBox_->setMaxCount(20);

	// Make conections:
	//////////////////////////
	connect(sourceComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onSourceComboBoxChanged(int)));

	#ifdef AM_MOBILITY_VIDEO_ENABLED
	connect(videoWidget_->mediaPlayer(), SIGNAL(error(QMediaPlayer::Error)), this, SLOT(onMediaPlayerError()));
	#endif
}


void AMBeamlineCameraBrowser::onSourceComboBoxChanged(int index)
{
	if(index < 0) {
		setWindowTitle("AcquaCam");
		#ifdef AM_MOBILITY_VIDEO_ENABLED
		videoWidget_->mediaPlayer()->setMedia(QMediaContent());
		#endif
	}

	else {
		QString stringUrl = sourceComboBox_->itemText(index);
		QUrl url = QUrl::fromUserInput(stringUrl);

		if(url.isValid()) {
			if(url.toString() != stringUrl)
				sourceComboBox_->setItemText(index, url.toString());
			setWindowTitle(url.toString());
			// start and play it:
			playSource(url);
		}
		else {
			QMessageBox::warning(this, "Invalid camera source address", "Sorry! That doesn't look like a valid URL for a camera or media source:\n\n" % stringUrl);
			sourceComboBox_->removeItem(index);
		}
	}
}

QStringList AMBeamlineCameraBrowser::previousSourceURLs() const
{
	QStringList rv;
	for(int i=0; i<sourceComboBox_->count(); i++) {
		rv << sourceComboBox_->itemText(i);
	}
	return rv;
}

void AMBeamlineCameraBrowser::setPreviousSourceURLs(const QStringList &sourceURLs)
{
	QString currentText = sourceComboBox_->currentText();
	int currentIndex = sourceComboBox_->currentIndex(); // could be -1 if nothing in the list

	// remove existing items, except for current
	for(int i=0; i<currentIndex; i++)
		sourceComboBox_->removeItem(0);// remove all up to current item
	while(sourceComboBox_->count() > 1)
		sourceComboBox_->removeItem(sourceComboBox_->count()-1);// remove all beyond current

	// add new items (unless duplicated)
	foreach(QString url, sourceURLs) {
		if(url != currentText)
			sourceComboBox_->addItem(url);
	}
}

QString AMBeamlineCameraBrowser::currentSourceURL() const
{
	return sourceComboBox_->currentText();
}

void AMBeamlineCameraBrowser::setCurrentSourceURL(const QString &sourceURL)
{
	if(sourceURL.isEmpty())
		return;

	int existingIndex = sourceComboBox_->findText(sourceURL);
	if(existingIndex >= 0) {
		sourceComboBox_->setCurrentIndex(existingIndex);
	}
	else {
		sourceComboBox_->insertItem(0, sourceURL);
		sourceComboBox_->setCurrentIndex(0);
	}
}

void AMBeamlineCameraBrowser::onMediaPlayerError()
{
	QMessageBox::warning(this, "AcquaCam Error", "Sorry! There was an error trying to open that media URL:\n\n   " + videoWidget_->mediaPlayer()->errorString());
	sourceComboBox_->removeItem(sourceComboBox_->currentIndex());
}



