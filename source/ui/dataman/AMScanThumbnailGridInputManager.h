#ifndef AMSCANTHUMBNAILGRIDINPUTMANAGER_H
#define AMSCANTHUMBNAILGRIDINPUTMANAGER_H

#include <QObject>
#include <QTimer>
#include <QRect>
#include <QItemSelectionModel>


// The distance which the mouse can travel before it is considered
// a drag operation rather than just a slight drift in a point operation.
#define TRAVEL_TOLERANCE 20
// The time between two clicks before they are considered separate, and not
// part of the same double click.
#define DOUBLE_CLICK_INTERVAL_MS 250
// The time that a mouse is over a given item index before it is considered to
// be hovering.
#define HOVER_INTERVAL_MS 500
// The distance which the mouse has to move between signals are emitted to inform
// listeners that the mouse has been moved while hovering over a given itemIndex
#define HOVER_TRAVEL_UPDATE_DISTANCE 20



class AMScanThumbnailGridView;
/**
 * A class for managing all the user input for a AMScanThumbnailGridView.
 * A flow chart of the interactions can be found at:
 *	https://creately.com/diagram/i7w2km9s1/pfyr3Eob1R0riPbWAUdEOcLEo%3D
 */
class AMScanThumbnailGridInputManager : public QObject
{
	Q_OBJECT
public:
	/**
	 * Creates an instance of an AMScanThumbnailGridInputManager.
	 * @param parent::QObject* ~ The parent QObject
	 */
	explicit AMScanThumbnailGridInputManager(QObject *parent = 0);

	/**
	  * Informs the input manager that the mouse has been pressed down
	  *
	  * @param itemIndex::int ~ The index of the model item which the mouse was pressed
	  * over. If the mouse was not over an item this value will be -1
	  * @param positionX::int ~ The X position of the mouse cursor at the time the button
	  * was pressed.
	  * @param positionY::int ~ The Y position of the mouse cursor at the time the button
	  * was pressed.
	  * @param mouseButtons::MouseButtons ~ A set of flags indicating which mouse buttons
	  * were pressed
	  * @param itemIsSelected::bool ~ Whether the item which the mouse was over is already selected
	  */
	void mouseDownAt(int itemIndex, int positionX, int positionY, Qt::MouseButtons mouseButtons, bool itemIsSelected);

	/**
	  * Informs the input manager that the mouse was moved as part of the current interaction.
	  * If the input manager is not currently in the process of performing an interaction then
	  * this is ignored.
	  *
	  * @param itemIndex::int ~ The index of the model item which the mouse was moved over.
	  * If the mouse was not over an item this value will be -1
	  * @param positionX::int ~ The X position which the mouse was moved to
	  * @param positionY::int ~ The Y position which the mouse was moved to
	  * @param mouseButtons::MouseButtons ~ The mouse buttons held while the move was
	  * happening
	  */
	void mouseMovedTo(int itemIndex, int positionX, int positionY, Qt::MouseButtons mouseButtons);

	/**
	  * Informs the input manager that the mouse button was released
	  *
	  * @param positionX::int ~ The X position of the mouse cursor at the time the button
	  * was released
	  * @param positionX::int ~ The X position of the mouse cursor at the time the button
	  * was released
	  * @param mouseButtons::MouseButtons ~ The mouse buttons released which triggered this
	  * event
	  * @param keys::KeyboardModifiers ~ The keys which were held down while the mouse
	  * button was released
	  */
	void mouseReleasedAt(int positionX, int positionY, Qt::MouseButtons mouseButtons, Qt::KeyboardModifiers keys);

signals:

	/// Indicates that a single item has been selected with the provided selection flags
	void itemSelected(int itemIndex, QItemSelectionModel::SelectionFlags command);

	/// Indicates that the selection has been extended from the closest currently
	/// selected index to the provided itemIndex, with all the model items inbetween
	void selectionExtended(int itemIndex);

	/// Indicates that the provided itemIndex has been double clicked
	void itemDoubleClicked(int itemIndex);

	/// Indicates that a context menu has been requested at the provided itemIndex
	void contextMenuRequested(int itemIndex);

	/// Indicates that a drag operation has begun
	void dragBegun();

	/// Indicates that the current selection drag rectangle has been extended to
	/// encompass the provided selectionRectangle
	void selectionRectangleChanged(const QRect& selectionRectangle);

	/// Indicates that the current selection drag rectangle has ended with the
	/// provided selection flag modifiers
	void selectionRectangleEnded(const QRect& selectionRectangle, QItemSelectionModel::SelectionFlags command);

	/// Indicates that the mouse has moved over the provided index by the provided
	/// delta horizontally while a hover interaction is taking place
	void hoverMove(int itemIndex, int positionX, int positionY);

public slots:

protected slots:

private:

	/// Resets all interaction variables to their current state. This indicates
	/// that one entire interaction cycle has taken place, and the input manager
	/// is now in a state ready for another to being.
	void resetInteraction();

	/// Stores the selection state of the item which the mouse was over when the
	/// interaction began. Is used to determine whether a rectangle selection or
	/// a drag operation should be begun
	bool startItemSelected_;

	/// The X position of the mouse cursor at the beginning of the current interaction
	/// (is -1 if no interaction is currently in progress)
	int startPositionX_;

	/// The Y position of the mouse cursor at the beginning of the current interaction
	/// (is -1 if no interaction is currently in progress)
	int startPositionY_;

	/// The X position of the mouse cursor during the last interaction update
	/// (is -1 if no interaction is currently in progress)
	int lastUpdatePositionX_;

	/// The Y position of the mouse cursor during the last interaction update
	/// (is -1 if no interaction is currently in progress)
	int lastUpdatePositionY_;

	/// The index of the item which was under the mouse the last time a mouse
	/// button was pressed
	int lastClickedItemIndex_;

	/// The index of the item which was under the mouse last time the mouse moved
	int lastMouseOverItemIndex_;
	
	/// The X position of the cursor the last time a hover position update was emitted
	int lastHoverUpdatePositionX_;

	/// Timer used to distinguish between a single and double click
	QTimer doubleClickTimer_;

	/// Timer used to determine whether the mouse is considered to be hovering
	/// over an item
	QTimer hoverTimer_;

	/// Timer used to determine how often update signals about the hover position
	/// of the mouse cursor should be emitted.
	QTimer hoverUpdateTimer_;

	/// Flag used to determine whether the current interaction has become a drag
	/// operation
	bool dragInProgress_;

	/// A set of flags which are used to store the state of the keys the last time
	/// the mouse button was pressed
	Qt::KeyboardModifiers keys_;

};

#endif // AMSCANTHUMBNAILGRIDINPUTMANAGER_H
