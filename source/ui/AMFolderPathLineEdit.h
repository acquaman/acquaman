#ifndef AMFOLDERPATHLINEEDIT_H
#define AMFOLDERPATHLINEEDIT_H

#include <QLineEdit>
#include <QValidator>


/// If you want to ensure that the content entered in a QLineEdit is a valid path to a folder, set an AMFolderPathValidator as its validator.  Beyond the QValidator functionality, this also gives you a validChanged() signal when the path becomes valid/not valid.
class AMFolderPathValidator : public QValidator {

	Q_OBJECT

public:
	AMFolderPathValidator(QObject* parent = 0) : QValidator(parent) {}
	State validate ( QString & input, int & pos ) const;

signals:
	void validChanged(bool isValid) const;
};



/// This is a replacement for QLineEdit that validates and auto-completes folder paths, as well as providing user feedback when the path entered is valid.
class AMFolderPathLineEdit : public QLineEdit
{
	Q_OBJECT
public:
	explicit AMFolderPathLineEdit(QWidget *parent = 0);

signals:

public slots:

protected slots:
	void onValidChanged(bool isValid);

};

#endif // AMFOLDERPATHLINEEDIT_H
