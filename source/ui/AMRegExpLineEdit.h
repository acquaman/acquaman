#ifndef AMREGEXPLINEEDIT_H
#define AMREGEXPLINEEDIT_H

#include <QLineEdit>
#include <QRegExp>

class AMRegExpLineEdit : public QLineEdit
{
	Q_OBJECT
	QRegExp regExp_;
	QString warningMessage_;
	bool isValid_;
	bool validIfMatches_;
public:
	explicit AMRegExpLineEdit(const QString& pattern,Qt::CaseSensitivity = Qt::CaseSensitive, const QString& warningMessage = QString(), QWidget *parent = 0);

	void setValidIfMatches(bool validIfMatches);
	bool isValid();
protected:
	void validate();
	void setPattern(const QString& pattern);

signals:
	void validStateChanged(bool isNowValid);
public slots:
protected slots:
	void onTextChanged(const QString&);
};


#endif // AMREGEXPLINEEDIT_H
