#ifndef AMCAMERABROWSER_H
#define AMCAMERABROWSER_H

#include <QObject>

class QString;
class QColor;
class QPointF;
class QStringList;
class AMShapeDataSet;
class AMShapeDataSetView;


class AMCameraBrowser : public QObject
{
    Q_OBJECT
public:
    AMCameraBrowser(QObject *parent = 0);


    /// Is the crosshair locked?
    bool crosshairLocked() const { return crosshairLocked_; }
    /// Returns the crosshair color
    QColor crosshairColor() const;
    /// Returns the crosshair line thickness
    int crosshairLineThickness() const;
    /// Is the crosshair visible?
    bool crosshairVisible() const;
    /// Returns the crosshair position (relative X-Y position on the video display, from (0,0)[top left] to (1,1)[bottom right] )
    QPointF crosshairPosition() const;


    /// Accessors
    QString currentURL();
    AMShapeDataSet *shapeDataSet();

    /// Mutators
    void setCurrentURL(QString currentURL);


public slots:

    /// \todo move to shapeDataset
    /// Set the crosshair position (relative X-Y position on the video display, from (0,0)[top left] to (1,1)[bottom right] )
    void setCrosshairPosition(const QPointF& pos) const;

    void setShapeDataView(AMShapeDataSetView *);


    //signals
protected slots:
    /// \todo move to shapeDataset
    /// Called when double-clicking on the video widget (move the cursor if not locked)
    void onVideoWidgetDoubleClicked(const QPointF& clickPoint);
    //protected fns
protected:
    AMShapeDataSet *shapeDataSet_;


    /// \todo move out
    AMShapeDataSetView *shapeDataSetView_;

    QString currentURL_;

    /// \todo move to shapeDataset
    bool crosshairLocked_;
};

#endif // AMCAMERABROWSER_H
