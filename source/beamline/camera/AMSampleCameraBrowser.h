#ifndef AMSAMPLECAMERABROWSER_H
#define AMSAMPLECAMERABROWSER_H

#include <QObject>

class QString;
class QColor;
class QPointF;
class QStringList;
class AMSampleCamera;
class AMSampleCameraView;
class AMSamplePlate;

#include "dataman/database/AMDbObject.h"

class AMSampleCameraURL : public AMDbObject
{
Q_OBJECT

Q_PROPERTY(QString urlString READ urlString WRITE setURLString)

public:
	Q_INVOKABLE AMSampleCameraURL(const QString &urlString = QString(), QObject *parent = 0);

	QString urlString() const;

public slots:
	void setURLString(const QString &urlString);

protected:
	QString urlString_;
};

class AMSampleCameraBrowser : public QObject
{
	Q_OBJECT
public:
	AMSampleCameraBrowser(QObject *parent = 0);
	/// Accessors
	QString currentURL();
	AMSampleCamera *shapeDataSet();

	/// Mutators
	void setCurrentURL(QString currentURL);

	void addURL(const QString &urlString);
	void removeURL(const QString &urlString);

	QList<AMSampleCameraURL*> urls();

protected slots:
	void onSamplePlateChanged(AMSamplePlate *samplePlate);
	void onCameraShapesChanged();

protected:
	AMSampleCamera *shapeDataSet_;
	AMSamplePlate *currentSamplePlate_;

	QString currentURL_;
	QList<AMSampleCameraURL*> urls_;
};

#endif // AMSAMPLECAMERABROWSER_H
