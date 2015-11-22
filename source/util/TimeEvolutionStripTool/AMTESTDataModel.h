#ifndef AMTESTDATAMODEL_H
#define AMTESTDATAMODEL_H

#include <QObject>

/// Basic data model.  Contains the data that can be visualized.
class AMTESTDataModel : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMTESTDataModel(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMTESTDataModel();

	/// Returns whether this is a scaler data model.
	virtual bool isSeriesDataModel() const { return false; }
	/// Returns whether this is an image data model.
	virtual bool isImageDataModel() const { return false; }

	/// Returns the name of the data model.
	QString name() const { return name_; }

signals:

public slots:
protected:
	/// The name of the data model.
	QString name_;
};

#endif // AMTESTDATAMODEL_H
