#ifndef ACQMAN_CHANNEL_H
#define ACQMAN_CHANNEL_H

#include <QObject>
#include <muParser/muParser.h>

#include "dataman/AMDataTree.h"

class AMScan;

class AMChannel : public QObject
{
Q_OBJECT
Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
	explicit AMChannel(AMScan* scan, const QString& name, const QString& expression);

    /// AMChannel name: (ex: "tey", "tfy_io", etc.)
    QString name() const { return name_; }

	/// The scan that owns this channel
	AMScan* scan() const { return scan_; }

	/// the expression currently set for this channel
	QString expression() const { return QString::fromStdString(parser_.GetExpr()); }

	/// the number of accessible values in this series
	unsigned count() const { return dataTree()->count(); }

	/// the value of this channel at index \c p
	double value(unsigned p) const;

signals:
    void nameChanged(QString);

public slots:
	/// Set channel name: ( \bug should this be allowed? how would it change the name used to access this channel within the scan?)
    void setName(const QString& newName) { name_ = newName; }
	/// Set the expression:
	bool setExpression(const QString& expression);

protected:
    QString name_;
	mu::Parser parser_;
	AMScan* scan_;
	mutable QVector<double> varStorage_;


	/// clears the parser variables and sets them to the names of the raw data columns
	bool setVariablesFromDataColumns();

public:
	/// access the data tree from scan object. \todo Should not be public. Why does it need to be to compile?  [friend of AMScan]
	AMDataTree* dataTree() const;


};

#endif // CHANNEL_H
