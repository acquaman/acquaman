/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMCROSSHAIROVERLAYVIDEOWIDGET2_H
#define AMCROSSHAIROVERLAYVIDEOWIDGET2_H

#include "ui/AMOverlayVideoWidget2.h"

#include <QWidget>
#include <QVector3D>
#include <QGraphicsPolygonItem>

#ifdef AM_MOBILITY_VIDEO_ENABLED
class QMediaPlayer;
#endif

class QTimer;
class QGraphicsLineItem;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QSlider;
class QPushButton;
class QLineEdit;
class QColor;
class QToolBar;
class QAction;
class QCompleter;
class QStringListModel;

class AMSampleCamera;
class AMCameraConfiguration;
class AMColorPickerButton2;
class AMSampleCameraGraphicsView;
class AMShapeDataView;
class AMCameraConfigurationView;
class AMBeamConfigurationView;
class AMGraphicsTextItem;
class AMGraphicsViewWizard;
class AMCameraConfigurationWizard;
class AMBeamConfigurationWizard;
class AMSamplePlateWizard;
class AMSimpleSamplePlateWizard;
class AMRotationWizard;
class AMShapeDataListView;
class AMShapeData;
class AMSamplePlate;
class AMWizardManager;
class AMSampleCameraColorEditor;

#define AMSAMPLECAMERAVIEW_REFRESHSCENEVIEW_MISSING_SHAPE 670001
#define AMSAMPLECAMERAVIEW_SAMPLEPLATECREATE_INVALID_NUMBER_OF_POINTS 670002
#define AMSAMPLECAMERAVIEW_SAMPLEPLATECREATE_INVALID_SAMPLE_INDEX 670003
#define AMSAMPLECAMERAVIEW_SAMPLEPLATECREATE_LIST_UNEXPECTEDLY_EMPTY 670004
#define AMSAMPLECAMERAVIEW_ROTATIONCONFIGURATION_INCORRECT_NUMBER_OF_POINTS 670005
#define AMSAMPLECAMERAVIEW_LOADED_DEFAULT_BEAM 670006
#define AMSAMPLECAMERAVIEW_LOADED_DEFAULT_CAMERA 670007
#define AMSAMPLECAMERAVIEW_LOADED_DEFAULT_ROTATION 670008
#define AMSAMPLECAMERAVIEW_DRAWSAMPLEPLATE_NULL_SAMPLEPLATE 670009


/// This class is a view for drawing 2D rectangles in 3D space with a configurable camera, with a video in the background
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

	/// Returns the color that will be returned when calling colour(ACTIVEBORDER)
	QColor activeBorderColor();
	/// Returns the color that will be returned when calling colour(BORDER)
	QColor borderColor();
	/// Returns the color that will be returned when calling colour(INTERSECTION)
	QColor intersectionColor();
	/// Returns the color that will be returned when calling colour(SAMPLEPLATEINTERSECTION)
	QColor samplePlateIntersectionColor();
	/// Returns the color that will be returned when calling colour(SAMPLEBORDER)
	QColor sampleBorderColor();

	#ifdef AM_MOBILITY_VIDEO_ENABLED
	/// set the media to play
	void setMedia(QMediaContent url);
	/// start playing the current media
	void play();
	/// the media player
	QMediaPlayer* mediaPlayer() const;
	#endif

	/// maps from other videos to this one
	QPointF mapPointToVideo(QPointF);

	/// checks for valid index
	bool isValid(int index) const;

	/// Load beam based on id. Default is -1 which loads the last value.
	bool loadBeam(int databaseId = -1);
	/// Load camera based on id. Default is -1 which loads the last value.
	bool loadCamera(int databaseId = -1);
	/// Load sample plate based on id. Default is -1 which loads the last value.
	bool loadSamplePlate(int databaseId = -1);
	/// Load rotational offset based on id. Default is -1 which loads the last value.
	bool loadRotationalOffset(int databaseId = -1);

	/// Returns the AMSampleCamera that we're using
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

	/// Sets the active border color
	void setActiveBorderColor(QColor activeBorderColor);
	/// Sets the border color
	void setBorderColor(QColor borderColor);
	/// Sets the intersection color
	void setIntersectionColor(QColor intersectionColor);
	/// Sets the sample plate intersection color
	void setSamplePlateIntersectionColor(QColor samplePlateIntersectionColor);
	/// Sets the sample border color
	void setSampleBorderColor(QColor sampleBorderColor);

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
	void samplePlateWizardFinished(bool requiresSave);
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

	/** these slots are for the
	 *	simpleSamplePlateWizard
	 */
	void initializeSampleShape();
	void shiftSampleShape(QPointF shift);
	void sampleShapeMousePressed(QPointF position);


	// request database loads
	void requestLoadBeam();
	void requestLoadCamera();
	void requestLoadSamplePlate();
	void requestLoadRotationConfiguration();

	void requestLoadBeamFromDatabase();
	void requestLoadCameraFromDatabase();
	void requestLoadSamplePlateFromDatabase();
	void requestLoadRotationConfigurationFromDatabase();

	void setRotationOffsetX(QString offset);
	void setRotationOffsetY(QString offset);
	void setRotationOffsetZ(QString offset);

	void onRotationalOffsetChanged(QVector3D);

	void showAdvancedWindow();

	void refreshSceneColours();


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

	void createBeamShape(int);
	void moveBeamShape(QPointF point, int index);

	void beamCalibrate();

	/// create the sample plate from the wizard
	void samplePlateCreate();
	void simpleSamplePlateCreate();

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

	void onSamplePlateWizardFinished(bool requiresSave);

	void enableMotorMovement(bool isEnabled);
	void enableMotorTracking(bool isEnabled);

	void onEnableMotorMovementChanged(bool isEnabled);
	void onEnableMotorTrackingChanged(bool isEnabled);

	void onOffsetTypeComboBoxCurrentIndexChanged(const QString &text);

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

//	void setBeamWizardType(WizardType type);
//	WizardType beamWizardType();
//	void setSamplePlateWizardType(WizardType type);
//	WizardType samplePlateWizardType();


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

	/// simple sample plate settings window
	QFrame *simpleSamplePlateConfiguration_;
	QLineEdit *simpleSamplePlateLineEdit_ [9];
	QPushButton *showSimpleSamplePlateConfigurationButton_;

	/// Holds the color that will be returned when calling colour(ACTIVEBORDER)
	QColor activeBorderColor_;
	/// Holds the color that will be returned when calling colour(BORDER)
	QColor borderColor_;
	/// Holds the color that will be returned when calling colour(INTERSECTION)
	QColor intersectionColor_;
	/// Holds the color that will be returned when calling colour(SAMPLEPLATEINTERSECTION)
	QColor samplePlateIntersectionColor_;
	/// Holds the color that will be returned when calling colour(SAMPLEBORDER)
	QColor sampleBorderColor_;

	/// Map of QGraphicsPolygonItem, corresponds to list in AMSampleCamera - should probably just be changed to a list
	QMap<int, QGraphicsPolygonItem*> shapes_;

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

	/// Combo box for the type of offset we're using (plate, wafer, or user defined)
	QComboBox *offsetTypeComboBox_;
	/// Actual offset value (uneditable when plate or wafer)
	QDoubleSpinBox *offsetValueSpinBox_;

	QPushButton* distortionButton_;

	ViewMode currentView_;

	QCompleter* autoCompleter_;
	QStringListModel* wordList_;

	QFrame* viewPortWindow_;
	QGraphicsView* viewPortView_;

	#ifdef AM_MOBILITY_VIDEO_ENABLED
	QMediaPlayer* viewMediaPlayer_;
	#endif

	QGraphicsPathItem* currentShape_;


	AMWizardManager *wizardManager_;

	QPushButton* cameraWizardButton_;
//	AMCameraConfigurationWizard* cameraWizard_;

	QPushButton* beamWizardButton_;
//	AMGraphicsViewWizard *beamWizard_;
//	WizardType beamWizardType_;


	QList<QGraphicsPolygonItem*> beamList_;
	int updateTracker_;

	QPushButton* samplePlateButton_;
	QPushButton* saveSamplePlate_;

	QPushButton* samplePlateWizardButton_;
//	AMGraphicsViewWizard* samplePlateWizard_;
//	WizardType samplePlateWizardType_;
	int samplePlateMovement_;

//	AMRotationWizard* rotationWizard_;

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

	bool showGrid_;

	/// A widget to set the color options for the samepleCameraView
	AMSampleCameraColorEditor *sampleCameraColorEditor_;
	/// A push button to launch the sampleCameraColorEditor
	QPushButton *showSampleCameraColorEditor_;

};

class AMSampleCameraColorEditor : public QWidget
{
Q_OBJECT
public:
	/// Simple constructor takes a pointer to a cameraView that it's going to set the color for
	AMSampleCameraColorEditor(AMSampleCameraView *cameraView, QWidget *parent = 0);

protected slots:
	/// Handles button click to set the active border color
	void onActiveBorderColorButtonClicked();
	/// Handles button click to set the border color
	void onBorderColorButtonClicked();
	/// Handles button click to set the intersection color
	void onIntersectionColorButtonClicked();
	/// Handles button click to set the sample plate intersection color
	void onSamplePlateIntersectionColorButtonClicked();
	/// Handles button click to set the sample plate color
	void onSampleBorderColorButtonClicked();

protected:
	/// Pointer to the cameraView so we can get and set colors
	AMSampleCameraView *cameraView_;

	/// Button for launching QColorDialog for active border color
	QPushButton *activeBorderColorButton_;
	/// Button for launching QColorDialog for border color
	QPushButton *borderColorButton_;
	/// Button for launching QColorDialog for intersection color
	QPushButton *intersectionColorButton_;
	/// Button for launching QColorDialog for sample plate intersection color
	QPushButton *samplePlateIntersectionColorButton_;
	/// Button for launching QColorDialog for sample border color
	QPushButton *sampleBorderColorButton_;
};

#endif // AMCROSSHAIROVERLAYVIDEOWIDGET2_H
