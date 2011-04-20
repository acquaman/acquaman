#include "AMFolderPathLineEdit.h"

#include <QCompleter>
#include <QFileSystemModel>
#include <QDir>

AMFolderPathLineEdit::AMFolderPathLineEdit(QWidget *parent) :
	QLineEdit(parent)
{
	QCompleter* folderCompleter = new QCompleter(this);
	QFileSystemModel* folderModel = new QFileSystemModel(this);
	folderModel->setRootPath(QDir::rootPath());
	folderModel->setFilter(QDir::AllDirs);
	folderCompleter->setModel(folderModel);
	setCompleter(folderCompleter);

	AMFolderPathValidator* v = new AMFolderPathValidator(this);
	setValidator(v);

	connect(v, SIGNAL(validChanged(bool)), this, SLOT(onValidChanged(bool)));
}

void AMFolderPathLineEdit::onValidChanged(bool isValid)
{
	QPalette p = palette();
	if(isValid)
		p.setColor(QPalette::Text, Qt::black);
	else
		p.setColor(QPalette::Text, Qt::red);

	setPalette(p);
}

QValidator::State AMFolderPathValidator::validate(QString &input, int &pos) const
{
	Q_UNUSED(pos)

	QDir dir(input);
	if(dir.exists()) {
		emit validChanged(true);
		return QValidator::Acceptable;
	}

	emit validChanged(false);
	return QValidator::Intermediate;
}
