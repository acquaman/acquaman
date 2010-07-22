#include "AMThumbnailScrollViewer.h"
#include "dataman/AMDatabaseDefinition.h"

#include <QVBoxLayout>

AMThumbnailScrollViewer::AMThumbnailScrollViewer(QWidget *parent) :
		QLabel(parent)
{

	setScaledContents(true);
	setFixedSize(240,180);

	title_ = new QLabel("Title");
	title_->setAlignment(Qt::AlignRight);
	subtitle_ = new QLabel("Subtitle");
	subtitle_->setAlignment(Qt::AlignRight);

	QVBoxLayout* vl = new QVBoxLayout();
	vl->addWidget(title_);
	vl->addWidget(subtitle_);
	vl->addStretch(1);
	setLayout(vl);

	setMouseTracking(true);
}

/// display the thumbnails from a given AMDbObject
void AMThumbnailScrollViewer::setSource(AMDbObject* source) {
	tIndex_ = 0;

	sourceIsDb_ = false;
	sourceObject_ = source;

	if(source && source->thumbnailCount() > 0)
		displayThumbnail(source->thumbnail(0));
	else {
		/// \todo clear pixmap to some nice blank
	}
}


/// Display thumbnails directly out of a database (from the thumbnail table, with id's given in \c ids)
void AMThumbnailScrollViewer::setSource(AMDatabase* db, QList<int> ids) {
	tIndex_ = 0;

	sourceIsDb_ = true;
	sourceDb_ = db;
	sourceIds_ = ids;

	if(sourceDb_ && sourceIds_.count() > 0)
		displayThumbnail(sourceDb_, sourceIds_.at(0));
	else {
		/// \todo clear pixmap to some nice blank
	}
}



void AMThumbnailScrollViewer::displayThumbnail(AMDbThumbnail t) {
	title_->setText(t.title);
	subtitle_->setText(t.subtitle);
	if(t.type == AMDbThumbnail::PNGType) {
		QPixmap p;
		p.loadFromData(t.thumbnail, "PNG");
		setPixmap(p);
	}
	else {
		QPixmap p(1,1);
		p.fill(Qt::white);
		setPixmap(p);
		/// \todo handle drawing nice blank
	}
}

void AMThumbnailScrollViewer::displayThumbnail(AMDatabase* db, int id) {

	QSqlQuery q = db->query();
	q.prepare(QString("SELECT title, subtitle, type, thumbnail FROM %1 WHERE id = ?").arg(AMDatabaseDefinition::thumbnailTableName()));
	q.bindValue(0, id);
	if(q.exec() && q.first()) {
		title_->setText(q.value(0).toString());
		subtitle_->setText(q.value(1).toString());
		if(q.value(2).toString() == QString("PNG")) {
			QPixmap p;
			p.loadFromData(q.value(3).toByteArray(), "PNG");
			setPixmap(p);

		}
		else {
			QPixmap p(1,1);
			p.fill(Qt::white);
			setPixmap(p);
			/// \todo handle drawing nice blank
		}
	}
}

#include <QMouseEvent>

void AMThumbnailScrollViewer::mouseMoveEvent ( QMouseEvent * event ) {

	double ratio = event->posF().x() / width();

	if(sourceIsDb_ && sourceDb_) {
		int index = sourceIds_.count()*ratio;
		if(index != tIndex_) {
			tIndex_ = index;
			displayThumbnail(sourceDb_, sourceIds_.at(tIndex_));
			update();
		}
	}

	if(!sourceIsDb_ && sourceObject_) {
		int index = sourceObject_->thumbnailCount()*ratio;
		if(index != tIndex_) {
			tIndex_ = index;
			displayThumbnail(sourceObject_->thumbnail(tIndex_));
			update();
		}
	}

	QLabel::mouseMoveEvent(event);
}
