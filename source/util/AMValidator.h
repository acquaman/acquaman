#ifndef AMVALIDATOR_H
#define AMVALIDATOR_H

#include <QObject>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QList>
/*!
  * A simple class for recording the valid state of an object.
  */
class AMValidator : public QObject
{
	Q_OBJECT
public:
	/*!
	  * Creates a new instance of an AM Validator which is initialized in the valid
	  * state.
	  */
	explicit AMValidator(QObject *parent = 0);

	/*!
	  * Virtual destructor for an AM Validator
	  */
	virtual ~AMValidator() { }

	/*!
	  * Updates the record of validity failures based on the passesTest value.
	  *   If passesTest is true the given failMessage is removed from the validator.
	  *   If passesTest is false then given failMessage is ensured to exist in the
	  *   validator.
	  * \param failMessage ~ The failure message uniquely identified with a failure
	  * to validate.
	  * \param failureCriteria ~ A boolean which indicates that the validity test
	  * has failed.
	  */
	void updateValidity(const QString& failMessage, bool failureCriteria);

	/*!
	  * Whether the validator currently has the object being validated in a valid
	  * state.
	  */
	bool isValid() const;

	/*!
	  * The number of failures the validator has recorded for the object being
	  * validatated.
	  */
	int failureCount() const;

	/*!
	  * A list containing all the messages which are preventing the object being
	  * validated from being in a valid state.
	  */
	QStringList failureMessages() const;

	/*!
	 * A string containing all the error messages associated with the object,
	 * sepearated by the optional delimiter.
	 * \param delimiter ~ An optional delimiter used to separate each message. If
	 * none is provided then a newline is used
	 * \return
	 */
	QString fullFailureMessage(const QString& delimiter = "\n") const;

	/*!
	  * Adds a validator which this validator will track. Any failures which are
	  * added to the child validator will be counted as failures of this validator.
	  * \param validator ~ The validator which is to be tracked.
	  */
	void addChildValidator(AMValidator* validator);
signals:

	/*!
	  * Signal indicating that the overall valid state of the object being validated
	  * has altered.
	  * \param validState ~ The new valid state of the object.
	  */
	void validStateChanged(bool validState);

	/*!
	  * Signal indicating that the number of validation failures has altered.
	  * \param failCount ~ The new number of failures.
	  */
	void failureCountChanged(int failureCount);

	/*!
	 * Signal indicating that a failure has been removed.
	 * \param failureMessage ~ A string associated with the failure which was
	 * removed.
	 */
	void failureRemoved(const QString& failureMessage);

	/*!
	 * Signal indicating that a failure has been added to the validator.
	 * \param failureAdded ~ A string associated with the failure which was
	 * added.
	 */
	void failureAdded(const QString& failureAdded);
public slots:
protected slots:
	/*!
	  * Slot which handles one of the child validators being destroyed. Removes
	  * the child validator from the list of validators to be tracked.
	  * \param qObject ~ The QObject which has been destroyed. Should always be
	  * of AMValidator type.
	  */
	void onChildValidatorDestroyed(QObject* qObject);

	/*!
	  * Slot which handles one of the child validator's failure count changing.
	  * Updates the failure count of this parent validator.
	  */
	void onChildFailureCountChanged(int);
protected:
	QSet<QString> failureMessages_;
	QList<AMValidator*> childValidators_;
};

#endif // AMVALIDATOR_H
