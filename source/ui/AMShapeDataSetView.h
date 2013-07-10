#ifndef AMCROSSHAIROVERLAYVIDEOWIDGET2_H
#define AMCROSSHAIROVERLAYVIDEOWIDGET2_H

#include "ui/AMOverlayVideoWidget2.h"
#include <QWidget>

class QTimer;
class QGraphicsLineItem;
class AMShapeDataSet;
class AMCameraConfiguration;
class QCheckBox;
class AMColorPickerButton2;
class QSlider;
class AMShapeDataSetGraphicsView;
class AMShapeDataView;
class QPushButton;
class QLineEdit;
class AMCameraConfigurationView;
class AMBeamConfigurationView;
class QColor;
class QTextDocument;

/// This class is a view for drawing 2D rectangles in 3D space with a configurable camera, with a video in the background
/*! The crosshair position is configurable using setCrosshairPosition() as a fraction of the video size, and referenced over top of the video, taking into account the proper aspect ratio and scaling/letterboxing.  Not only that, but you can observe the user's mouse interaction with the video display, via signals for mousePressed(), mouseReleased(), etc., which provide click positions in the same coordinate system.

For fun, you can connect the mouseDoubleClicked() signal to the setCrosshairPosition() slot to allow the user to re-position the crosshair by double-clicking. */




class AMShapeDataSetView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.
    explicit AMShapeDataSetView(AMShapeDataSet *shapeModel, QWidget *parent = 0, bool useOpenGlViewport = true);

	/// Returns the current pen used to draw the crosshair lines
	QPen crosshairPen() const;
	/// Returns the current position of the crosshair, in normalized coordinates. (ex: 0,0 = top left; 1,1 = bottom right)
    QPointF crosshairPosition() const;
	/// Returns whether the crosshair is currently visible
	bool crosshairVisible() const;

    /// returns whether the crosshair is locked
    bool crosshairLocked();



    /// Set the current motor coordinates
    void setMotorCoordinate(double x, double y, double z, double r);
    double motorRotation();
    double motorX();
    double motorY();
    double motorZ();

    /// Functions for modifying object information
    QString currentName();

    QString currentInfo();
    void setCurrentInfo(QString);

    /// Get the index of the currently selected object
    int currentIndex();

    double xCoordinate();
    double yCoordinate();
    double zCoordinate();
    double rotation();
    double tilt();







    void setMedia(QMediaContent url);
    void play();

    QMediaPlayer* mediaPlayer();

public slots:

        /// Set the crosshair color
        void setCrosshairColor(const QColor& color);

        /// Set the crosshair line thickness
        void setCrosshairLineThickness(int thickness);



        /// Disable the capability to move the cross-hair by double-clicking
        void setCrosshairLocked(bool doLock = true);

        void setCurrentName(QString);
         void setTilt(QString tilt);

         void setX(QString x);
         void setY(QString y);
         void setZ(QString z);
         void setRotation(QString rotation);

         void motorMoved();

         void setUseMotorCoordinate(bool);

         void setUseCameraMatrix(bool);

         void showCameraBeamWindow();

         void showShapeView();

         void setDrawOnShape();
         void setDrawOnShapeEnabled(bool enable);


signals:
	/// Emitted when the left mouse button is pressed down. (The position is reported as percentage of the video screen width and height; ie: from 0 to 1)
	void mousePressed(const QPointF& position);
	/// Emitted when the left mouse is released. (The position is reported as percentage of the video screen width and height; ie: from 0 to 1).  Not emitted for the second click of a double-click; see mouseDoubleClicked() instead.
	void mouseReleased(const QPointF& position);
	/// Emitted when the left mouse button is released from a double-click. (The position is reported as percentage of the video screen width and height; ie: from 0 to 1).
	void mouseDoubleClicked(const QPointF& position);

    /// Emitted when the mouse moves - in shapeoverlay
    void mouseMoved(const QPointF& position);
    /// Emitted when the right mouse button is pressed down
    void mouseRightClicked(const QPointF& position);
    /// Emitted when the left mouse is pressed down, in move mode
    void mouseMovePressed(const QPointF& position);
    /// Emitted when the right mouse is pressed down, in move mode
    void mouseMoveRightPressed(const QPointF& position);
    /// Emitted when the mouse is moved, while in move mode
    void mouseMovedMoveMode(const QPointF& position);
    /// Emitted when the currently selected item has changed
    void currentChanged();
    /// Emitted when the left mouse button is pressed, in edit mode
    void mouseEditPressed(const QPointF& position);
    /// Emitted when the right mouse button is pressed, in edit mode
    void mouseEditRightPressed(const QPointF& position);
    /// Emitted when the left mouse button is pressed, in shift mode
    void mouseShiftPressed(const QPointF& position);
    /// Emitted when the right mouse button is pressed, in shift mode
    void mouseShiftRightPressed(const QPointF& position);
    /// Emitted when the left mouse button is pressed, in operation mode
    void mouseOperationPressed(const QPointF& position, const QPointF& crossHairPosition);
    void mouseOperationSelect(const QPointF& position);
    /// Emitted when the left mouse button is pressed, in group mode
    void mouseGroupPressed(const QPointF& position);

    void mouseMultiDrawPressed(const QPointF& position);
    void mouseMultiDrawDoubleClicked(const QPointF& position);
    void enterMultiDraw();
    void modeChange();


    void setCoordinate(double x, double y, double z);

    // for beam configuring
    void oneSelect();
    void twoSelect();

    void updateShapes(int);


public slots:

	/// Set the crosshair position on the video display, using normalized coordinates. (ex: 0,0 = top left; 1,1 = bottom right)
    void setCrosshairPosition(const QPointF& pos);

	/// Set the pen used to draw the crosshair lines
	void setCrosshairPen(const QPen& pen);

	/// Enable or disable the crosshair
	void setCrosshairVisible(bool crosshairVisible = true);

    void setCameraModel(AMCameraConfiguration*);

    void intersection();

    void hideCameraParameters(bool hide);
protected slots:
    void updateCurrentShape();


    /// Helper function to (re-)position the crosshair lines when the view is resized or the lines are moved
    void reviewCrosshairLinePositions();

    /// handles mouse events on the graphics view
    void mousePressHandler(QPointF position);
    void mouseRightClickHandler(QPointF position);
    void mouseLeftReleaseHandler(QPointF position);
    void mouseRightReleaseHandler(QPointF position);
    void mouseDoubleClickHandler(QPointF position);

     void toggleDistortion();

     /// manages changing the selection
     void currentSelectionChanged();




    /// added mouse move event
    void mouseMoveHandler(QPointF position);


    /// Used to set the current mode selected on the toolbar

    void setDrawMode();
    void setMoveMode();
    void setEditMode();
    void setShiftMode();
    void setOperationMode();
    void setGroupMode();
    void setConfigurationMode();
    void setMultiDrawMode();

    void setMotorCoordinatePressed();

    void enableMotorMovement(bool isEnabled);

    void enableMotorTracking(bool isEnabled);

    void showConfigurationWindow();

    void setPoint(QPointF position, int point);
    void selectPoint(int);
    void selectPointOne();
    void selectPointTwo();
    void selectPointThree();
    void selectPointFour();
    void selectPointFive();
    void selectPointSix();//    void moveCurrentToCoordinate();

    void runCameraConfiguration();

    void deleteCalibrationPoints();

    void stopTimer();

    void changeDrawButtonText();

    void updateItemName();




protected:

    /// Add and remove shapes from the scene
    void addNewShape();
    void deleteShape();



    void createGroupRectangle();

    void destroyGroupRectangle();








	/// We catch resize events and re-position the crosshair lines as required
	void resizeEvent(QResizeEvent *event);


    void createIntersectionShapes(QVector<QPolygonF>);

    void clearIntersections();







protected:

    enum selectMode{DRAW, MOVE, EDIT, SHIFT, OPERATION, GROUP, CONFIGURE, MULTIDRAW};

    QGraphicsLineItem* crosshairXLine_, *crosshairYLine_;

    AMShapeDataSetGraphicsView *shapeScene_;

    AMShapeDataView *shapeView_;

    QFrame *configurationWindow_;

    AMCameraConfigurationView *cameraConfiguration_;

    AMBeamConfigurationView *beamConfiguration_;

    QColor borderColour_;
    QColor activeBorderColour_;



    /// Map of QGraphicsPolygonItem, corresponds to a map of AMShapeData2
    QMap<int,QGraphicsPolygonItem*> shapes_;

    /// The model for all the shapes displayed (except the crosshair)
    AMShapeDataSet* shapeModel_;

    QGraphicsPolygonItem* groupRectangle_;

    bool groupRectangleActive_;

    /// the max index in use (number of rectangles - 1)
    int index_;
    /// current mode selected by toolbar
    selectMode mode_;
    /// the index of the currently selected item
    int current_;

    /// Indicates that the second press of a double-click has happened
    bool doubleClickInProgress_;

    QVector<QGraphicsPolygonItem*> intersections_;


    /// crosshair control bar

    QCheckBox* showCrosshairCheckBox_, *lockCrosshairCheckBox_;
    AMColorPickerButton2* crosshairColorPicker_;
    QSlider* crosshairThicknessSlider_;

    /// Mode buttons
    QPushButton* drawButton_;
    QPushButton* moveButton_;
    QPushButton* editButton_;
    QPushButton* shiftButton_;
    QPushButton* operationButton_;
    QPushButton* groupButton_;
    /// Motor coordinate control
    QLineEdit* motorXEdit_;
    QLineEdit* motorYEdit_;
    QLineEdit* motorZEdit_;
    QLineEdit* motorREdit_;
    /// Motor coordinate set button
    QPushButton* setMotorCoordinate_;

    /// Motor move enable
    QCheckBox* enableMotorMovement_;

    /// motor tracking enable
    QCheckBox* enableMotorTracking_;

    /// camera configuration button
    QPushButton* configureCameraButton_;
    /// camera configuration window items
    QLineEdit* pointLineEdit_[12];
    QLineEdit* coordinateLineEdit_[18];
    QPushButton* pointPushButton_[6];
    QFrame* cameraConfigurationWindow_;
    QPushButton* startCameraConfiguration_;
    QCheckBox* motorCoordinateCheckBox_;
    QPushButton* deleteCalibrationPoints_;
    QCheckBox* cameraMatrixCheckBox_;

    QPushButton* configurationWindowButton_;

    int pointToSelect_;
    bool useMotorCoordinate_;
    bool useCameraMatrix_;

    QPushButton* drawOnShapePushButton_;
    QLineEdit* drawOnShapeLineEdit_;
    QCheckBox* drawOnShapeCheckBox_;
    QPushButton* showShapeView_;

    QTimer* pressTimer_;

    QGraphicsTextItem* textItem_;

    QTextDocument* document_;

};

#endif // AMCROSSHAIROVERLAYVIDEOWIDGET2_H
