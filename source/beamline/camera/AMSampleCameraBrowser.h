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

protected slots:
	void onSamplePlateChanged(AMSamplePlate *samplePlate);

protected:
	AMSampleCamera *shapeDataSet_;
	AMSamplePlate *currentSamplePlate_;

	QString currentURL_;
};

#endif // AMSAMPLECAMERABROWSER_H
