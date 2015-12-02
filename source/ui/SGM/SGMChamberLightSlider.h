#ifndef SGMCHAMBERLIGHTSLIDER_H
#define SGMCHAMBERLIGHTSLIDER_H

#include <QWidget>
#include <QSlider>
class AMControl;
class SGMChamberLightSlider : public QSlider
{
    Q_OBJECT
public:
    explicit SGMChamberLightSlider(AMControl* chamberLight, QWidget *parent = 0);

    SGMChamberLightSlider(AMControl* chamberLight, Qt::Orientation orientation, QWidget* parent = 0);
signals:

public slots:

    void onControlConnected(bool isConnected);

    void onControlMovingChanged(bool isMoving);

    void onControlValueChanged(double);

    void onSliderReleased();
protected:

    void setupData();

    void setupConnections();


    AMControl* chamberLight_;

    bool connectedOnce_;
};

#endif // SGMCHAMBERLIGHTSLIDER_H
