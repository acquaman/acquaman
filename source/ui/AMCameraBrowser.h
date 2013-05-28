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





public slots:

    /// Set the crosshair color
    void setCrosshairColor(const QColor& color);
    /// Set the crosshair line thickness
    void setCrosshairLineThickness(int thickness);
    /// Set whether the crosshair is visible or not
    void setCrosshairVisible(bool isVisible);
    /// Disable the capability to move the cross-hair by double-clicking
    void setCrosshairLocked(bool doLock = true);
    /// Set the crosshair position (relative X-Y position on the video display, from (0,0)[top left] to (1,1)[bottom right] )
    void setCrosshairPosition(const QPointF& pos) const;

    void setShapeDataView(AMShapeDataSetView *);


    //signals
protected slots:
    /// Called when double-clicking on the video widget (move the cursor if not locked)
    void onVideoWidgetDoubleClicked(const QPointF& clickPoint);
    //protected fns
protected:
    AMShapeDataSet *shapeDataSet_;


    /// \todo move out
    AMShapeDataSetView *shapeDataSetView_;


    bool crosshairLocked_;
};

#endif // AMCAMERABROWSER_H
