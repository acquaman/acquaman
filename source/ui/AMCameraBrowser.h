#ifndef AMCAMERABROWSER_H
#define AMCAMERABROWSER_H

#include <QObject>

class QString;
class QColor;
class QPointF;
class QStringList;
class AMShapeDataSet;
class AMShapeDataSetView;
class AMSampleContainer;


class AMCameraBrowser : public QObject
{
    Q_OBJECT
public:
    AMCameraBrowser(QObject *parent = 0);
    /// Accessors
    QString currentURL();
    AMShapeDataSet *shapeDataSet();
    AMSampleContainer* sampleContainer();

    /// Mutators
    void setCurrentURL(QString currentURL);
public slots:
    void sampleIndexChanged(int);
    void shapeIndexChanged(int);
signals:
    void indexChanged(int);
    void changeSampleIndex(int);
protected slots:
protected:
    AMShapeDataSet *shapeDataSet_;

    AMSampleContainer* sampleContainer_;




    QString currentURL_;

};

#endif // AMCAMERABROWSER_H
