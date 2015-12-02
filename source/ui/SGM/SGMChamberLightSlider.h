#ifndef SGMCHAMBERLIGHTSLIDER_H
#define SGMCHAMBERLIGHTSLIDER_H

#include <QWidget>
#include <QSlider>
class AMControl;
/*!
  * A ui class for controlling/visualizing the SGM Chamber light Control
  */
class SGMChamberLightSlider : public QSlider
{
    Q_OBJECT
public:
    /*!
      * Creates an instance of an SGM Chamber Light Slider which will visualize the provided chamber
      * light control. Defaults to vertical orientation
      */
    explicit SGMChamberLightSlider(AMControl* chamberLight, QWidget *parent = 0);

    /*!
      * Creates an instance of an SGM Chamber Light Slider which will visualize the provied chamber
      * light control with the specified orientation.
      */
    SGMChamberLightSlider(AMControl* chamberLight, Qt::Orientation orientation, QWidget* parent = 0);
signals:

public slots:

    /// Handles the chamber light connection changing
    void onControlConnected(bool isConnected);

    /// Handles the chamber light's moving state changing
    void onControlMovingChanged(bool isMoving);

    /// Handles the chamber light's value changing
    void onControlValueChanged(double);

    /// Handles the user releasing the slider
    void onSliderReleased();
protected:

    /// Initializes the widget's data
    void setupData();

    /// Sets up connections between the slider and the control
    void setupConnections();


    AMControl* chamberLight_;

    bool connectedOnce_;
};

#endif // SGMCHAMBERLIGHTSLIDER_H
