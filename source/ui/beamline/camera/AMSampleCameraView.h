#ifndef AMCROSSHAIROVERLAYVIDEOWIDGET2_H
#define AMCROSSHAIROVERLAYVIDEOWIDGET2_H

#include "ui/AMOverlayVideoWidget2.h"
#include <QWidget>
#include <QVector3D>

class QTimer;
class QGraphicsLineItem;
class AMSampleCamera;
class AMCameraConfiguration;
class QCheckBox;
class AMColorPickerButton2;
class QSlider;
class AMSampleCameraGraphicsView;
class AMShapeDataView;
class QPushButton;
class QLineEdit;
class AMCameraConfigurationView;
class AMBeamConfigurationView;
class QColor;
class QToolBar;
class QAction;
class AMGraphicsTextItem;
class QCompleter;
class QStringListModel;
class QMediaPlayer;
class AMCameraConfigurationWizard;
class AMBeamConfigurationWizard;
class AMSamplePlateWizard;
class AMRotationWizard;
class AMShapeDataListView;
class AMShapeData;
class AMSamplePlate;


/// This class is a view for drawing 2D rectangles in 3D space with a configurable camera, with a video in the background
/*! The crosshair position is configurable using setCrosshairPosition() as a fraction of the video size, and referenced over top of the video,
 *taking into account the proper aspect ratio and scaling/letterboxing.  Not only that, but you can observe the user's mouse interaction with the video display,
 *via signals for mousePressed(), mouseReleased(), etc., which provide click positions in the same coordinate system.

For fun, you can connect the mouseDoubleClicked() signal to the setCrosshairPosition() slot to allow the user to re-position the crosshair by double-clicking. */




class AMSampleCameraView : public QWidget
{
	Q_OBJECT
public:
	enum ShapeColour{ACTIVEBORDER, BORDER, FILL, BACKWARDSFILL, INTERSECTION, HIDEINTERSECTION, SAMPLEPLATEINTERSECTION, SAMPLEFILL, SAMPLEBORDER};
	enum ViewType {DEBUG, CONDENSED};
	/// Constructor.
	explicit AMSampleCameraView(AMSampleCamera *shapeModel, ViewType viewType = CONDENSED, QWidget *parent = 0, bool useOpenGlViewport = true);

	/// Destructor
	~AMSampleCameraView();

	/// Returns the current pen used to draw the crosshair lines
	QPen crosshairPen() const;
	/// Returns the current position of the crosshair, in normalized coordinates. (ex: 0,0 = top left; 1,1 = bottom right)
	QPointF crosshairPosition() const;
	/// Returns whether the crosshair is currently visible
	bool crosshairVisible() const;

	/// returns whether the crosshair is locked
	bool crosshairLocked() const;



	/// Set the current motor coordinates
	void setMotorCoordinate(double x, double y, double z, double r);
	double motorRotation() const;
	double motorX() const;
	double motorY() const;
	double motorZ() const;

	/// current shape's name
	QString currentName() const;

	/// current shape's info
	QString currentInfo() const;
	/// set the current shape's info
	void setCurrentInfo(const QString &info);

	/// Get the index of the currently selected object
	int currentIndex();

	/// current shape's coordinates and rotation
	double xCoordinate();
	double yCoordinate();
	double zCoordinate();
	double rotation();
	double tilt();

	/// set the media to play
	void setMedia(QMediaContent url);
	/// start playing the current media
	void play();

	/// the media player
	QMediaPlayer* mediaPlayer() const;

	/// maps from other videos to this one
	QPointF mapPointToVideo(QPointF);

	/// checks for valid index
	bool isValid(int index) const;

	/// load last beam
	bool loadBeam();
	/// load last camera
	bool loadCamera();
	/// load last sample plate
	bool loadSamplePlate();

	AMSampleCamera* sampleCamera();

public slots:

	/// Set the crosshair color
	void setCrosshairColor(const QColor& color);

	/// Set the crosshair line thickness
	void setCrosshairLineThickness(int thickness);



	/// Disable the capability to move the cross-hair by double-clicking
	void setCrosshairLocked(bool doLock = true);

	/// set the current shape's name
	void setCurrentName(QString);
	/// set the current shape's x-axis rotation
	void setTilt(QString tilt);

	/// set coordinates and rotation for the current shape
	void setX(QString x);
	void setY(QString y);
	void setZ(QString z);
	void setRotation(QString rotation);

	/// update after motorMovement
	void onMotorMoved();

	/// sets whether to use the current motor coordinate for calibration
	void setUseMotorCoordinate(bool);

	/// sets whether to use the camera matrix for visualization
	void setUseCameraMatrix(bool);

	/// shows the camera/beam configuration window
	void showCameraBeamWindow();

	/// sets the shape to draw on
	void setDrawOnShape();
	/// enable or disable drawing on shape
	void setDrawOnShapeEnabled(bool enable);

	/// update the camera configuration
	void reviewCameraConfiguration();

	/// switches between move to beam or move to crosshair
	void onMoveToBeamToggled(bool checked);

	/// switches between movement on sample plate and movement along the image plane
	void onMoveOnSamplePlateToggled(bool checked);

	/// sets the sample plate selected
	void samplePlateSelected();


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

	void mouseMove(QPointF);



	// for beam configuring
	void oneSelect();
	void twoSelect();

	void updateShapes(int);

	void applyDistortion();

	void motorMovementEnabled(bool enabled);
        void motorRotation(double rotation);

	void moveSucceeded();

	void changeSampleName(int, QString);

	void shapePropertyUpdated(AMShapeData *shapeData);

	// signal for each wizard finished
	void beamWizardFinished();
	void cameraWizardFinished();
	void samplePlateWizardFinished();
	void rotationWizardFinished();

	void samplePlateSelected(AMSamplePlate*);




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

	void startCameraWizard();

	void startBeamWizard();

	void startSampleWizard();

	void startRotationWizard();

	void setSamplePlate();

	void setCameraConfigurationShape();

	void moveSamplePlate(int movement);

	void showBeamOutline(bool show);

	void moveTestSlot();

	void requestUpdate();

	void shapeDrawingFinished();


	// request database loads
	void requestLoadBeam();
	void requestLoadCamera();
	void requestLoadSamplePlate();
	void requestLoadRotationConfiguration();

	void setRotationOffsetX(QString offset);
	void setRotationOffsetY(QString offset);
	void setRotationOffsetZ(QString offset);

	void onRotationalOffsetChanged(QVector3D);

protected slots:
	void updateCurrentShape();


	/// Updates all shapes
	void refreshSceneView();

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

	void showConfigurationWindow();

	void setPoint(QPointF position, int point);
	void selectPoint(int);
	void selectPointOne();
	void selectPointTwo();
	void selectPointThree();
	void selectPointFour();
	void selectPointFive();
	void selectPointSix();

	void runCameraConfiguration();

	void deleteCalibrationPoints();

	void stopTimer();

	void changeDrawButtonText();

	void updateItemName(int index);
	void updateItemReturnPressed(int index);

	void updateCurrentTextItemName();

	void setViewName();
	void setViewOtherData();
	void setViewIdNumber();
	void setViewHidden();

	void autoCompleteEnterPressed();

	void beamShape(int);

	void beamCalibrate();

	void samplePlateCreate();

	void rotationConfiguration();


	void moveBeamSamplePlate(QVector3D);
	void moveBeamSamplePlate(QVector3D, double);

	void showBeamMarker(int);

	void transmitMotorMovementEnabled();

        void transmitMotorRotation();

	void updateShapeName(QString newName);

	void updateDataOne(QString data);

	void updateDataTwo(QString data);

	void onShowSamplePlateStateChanged(bool state);

	void onSamplePlateWizardFinished();

	void enableMotorMovement(bool isEnabled);
	void enableMotorTracking(bool isEnabled);

	void onEnableMotorMovementChanged(bool isEnabled);
	void onEnableMotorTrackingChanged(bool isEnabled);

protected:




	/// Add and remove shapes from the scene
public:
	void addNewShape();
protected:
	void deleteShape();



	void createGroupRectangle();

	void destroyGroupRectangle();








	/// We catch resize events and re-position the crosshair lines as required
	void resizeEvent(QResizeEvent *event);


	void createIntersectionShapes(QVector<QPolygonF>);

	void clearIntersections();

	void setGUI(ViewType viewType);

	void makeConnections(ViewType viewType);

	QColor colour(ShapeColour role);

	void drawSamplePlate();

        bool samplePointListEmpty(QList<QPointF>*list, int numberOfPoints) const;


protected:
	/// selectMode used to determine what actions should be performed by  mouse events
	enum selectMode{DRAW, MOVE, EDIT, SHIFT, OPERATION, GROUP, CONFIGURE, MULTIDRAW};

	/// ViewMode used to determine what each text item displays
	enum ViewMode{NAME,DATA,ID,HIDE};


	/// crosshair lines
	QGraphicsLineItem* crosshairXLine_, *crosshairYLine_;

	/// the graphics view that visualizes all the shapes
	AMSampleCameraGraphicsView *shapeScene_;

	/// window for beam and camera settings
	QFrame *configurationWindow_;

	/// camera settings window
	AMCameraConfigurationView *cameraConfiguration_;

	/// beam settings window
	AMBeamConfigurationView *beamConfiguration_;


	//    QColor borderColour_;
	//    QColor activeBorderColour_;



	/// Map of QGraphicsPolygonItem, corresponds to list in AMSampleCamera - should probably just be changed to a list
	QMap<int,QGraphicsPolygonItem*> shapes_;

	/// The model for all the shapes displayed
	AMSampleCamera* shapeModel_;

	QGraphicsPolygonItem* groupRectangle_;

	QGraphicsPolygonItem* samplePlate_;

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

	QCheckBox *lockCrosshairCheckBox_;
	QCheckBox *showCrosshairCheckBox_;
	AMColorPickerButton2* crosshairColorPicker_;
	QSlider* crosshairThicknessSlider_;

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
	QCheckBox* drawOnShapeCheckBox_;
	QPushButton* showShapeView_;

	QTimer* pressTimer_;

	QList<AMGraphicsTextItem*> textItems_;

	QToolBar* toolBar_;
	QAction* markAction_;
	QAction* moveAction_;
	QAction* editAction_;
	QAction* shiftAction_;
	QAction* operationAction_;
	QAction* groupAction_;

	QToolBar* labelToolBar_;
	QAction* viewName_;
	QAction* viewOtherData_;
	QAction* viewIdNumber_;
	QAction* viewHidden_;

	QPushButton* distortionButton_;

	ViewMode currentView_;

	QCompleter* autoCompleter_;
	QStringListModel* wordList_;

	QFrame* viewPortWindow_;
	QGraphicsView* viewPortView_;
	QMediaPlayer* viewMediaPlayer_;

	QGraphicsPathItem* currentShape_;

	QPushButton* cameraWizardButton_;
	AMCameraConfigurationWizard* cameraWizard_;

	QPushButton* beamWizardButton_;
	AMBeamConfigurationWizard* beamWizard_;

	QList<QGraphicsPolygonItem*> beamList_;
	int updateTracker_;

	QPushButton* samplePlateButton_;
	QPushButton* saveSamplePlate_;

	QPushButton* samplePlateWizardButton_;
	AMSamplePlateWizard* samplePlateWizard_;
	int samplePlateMovement_;

	AMRotationWizard* rotationWizard_;

	QPushButton* cameraConfigurationShapeButton_;

	QCheckBox* showBeamOutlineCheckBox_;

	bool showBeamOutline_;

	QFrame* advancedWindow_;
	QPushButton* advancedButton_;

	QCheckBox* moveToBeam_;
	QCheckBox* moveOnSamplePlate_;
	QCheckBox* showSamplePlate_;

	bool samplePlateSelected_;

	QPushButton* loadDefaultBeam_;
	QPushButton* loadDefaultCamera_;
	QPushButton* loadDefaultSamplePlate_;

	QGraphicsRectItem* videoBorderItem_;

	QLineEdit* rotationalOffset_ [3];

};

#endif // AMCROSSHAIROVERLAYVIDEOWIDGET2_H
