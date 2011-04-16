#ifndef AMEXPORTEROPTIONGENERALASCII_H
#define AMEXPORTEROPTIONGENERALASCII_H

#include "dataman/AMExporterOptionGeneral.h"

class AMExporterOptionGeneralAscii : public AMExporterOptionGeneral
{
	Q_OBJECT

	Q_PROPERTY(QString columnDelimiter READ columnDelimiter WRITE setColumnDelimiter)
	Q_PROPERTY(QString newlineDelimiter READ newlineDelimiter WRITE setNewlineDelimiter)

public:
	explicit AMExporterOptionGeneralAscii(QObject *parent = 0);

	/// The delimiter to use between columns
	QString columnDelimiter() const { return columnDelimiter_; }
	/// The delimiter to use between lines (newline character)
	QString newlineDelimiter() const { return newlineDelimiter_; }



	virtual QWidget* createEditorWidget();


signals:

public slots:
	/// Set delimiter to use between columns
	void setColumnDelimiter(const QString& t) { columnDelimiter_ = t; setModified(true); }
	/// Set the delimiter to use between lines (newline character)
	void setNewlineDelimiter(const QString& t) { newlineDelimiter_ = t; setModified(true); }

protected:
	/// The delimiter to use between columns
	QString columnDelimiter_;
	/// The delimiter to use between lines (newline character)
	QString newlineDelimiter_;

};

#endif // AMEXPORTEROPTIONGENERALASCII_H
