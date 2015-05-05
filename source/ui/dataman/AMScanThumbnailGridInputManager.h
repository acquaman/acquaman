#ifndef AMSCANTHUMBNAILGRIDINPUTMANAGER_H
#define AMSCANTHUMBNAILGRIDINPUTMANAGER_H

#include <QObject>
#include <QTimer>
#include <QRect>
#include <QItemSelectionModel>

#include <cmath>

// The distance which the mouse can travel before it is considered
// a drag operation rather than just a slight drift in a point operation.
#define TRAVEL_TOLERANCE 50
// The time between two clicks before they are considered separate, and not
// part of the same double click.
#define DOUBLE_CLICK_INTERVAL_MS 1000
// The time that a mouse is over a given item index before it is considered to
// be hovering.
#define HOVER_INTERVAL_MS 10000



class AMScanThumbnailGridView;
/**
 * A class for managing all the user input for a AMScanThumbnailGridView.
 * A flow chart of the interactions can be found at:
 *	https://creately.com/diagram/i7w2km9s1/pfyr3Eob1R0riPbWAUdEOcLEo%3D
 */
class AMScanThumbnailGridInputManager : public QObject {
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
	  */
	void mouseDownAt(int itemIndex, int positionX, int positionY, Qt::MouseButtons mouseButtons);

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

	/// Indicates that a drag operation has begun on the provided item index
	void dragBegun(int itemIndex);

	/// Indicates that the current selection drag rectangle has been extended to
	/// encompass the provided selectionRectangle
	void selectionRectangleChanged(const QRect& selectionRectangle);

	/// Indicates that the current selection drag rectangle has ended with the
	/// provided selection flag modifiers
	void selectionRectangleEnded(QItemSelectionModel::SelectionFlags command);

	/// Indicates that the mouse has moved over a the provided index by the provided
	/// deltas while a hover interaction is taking place
	void hoverMove(int itemIndex, int deltaX, int deltaY);

public slots:

protected slots:
	/// Handles the double click timer expiring. This timer is used to give time for
	/// another click to occur to distinguish between a single click and a double
	/// click ie. when this timer expires it indicates a single click has just
	/// occured
	void onDoubleClickTimerExpired();

private:

	/// Resets all interaction variables to their current state. This indicates
	/// that one entire interaction cycle has taken place, and the input manager
	/// is now in a state ready for another to being.
	void resetInteraction();

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
