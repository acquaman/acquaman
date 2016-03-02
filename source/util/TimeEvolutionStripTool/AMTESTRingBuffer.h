#ifndef AMTESTRINGBUFFER_H
#define AMTESTRINGBUFFER_H

#include <QVector>

/// This class encapsulates a ring buffer.  It adds data as appropriate based on the size
/// but provides a normal vector interface to access the data.
/// This technically could be a template but I'm going to make it use qreal only.
class AMTESTRingBuffer
{
public:
	/// Default consttuctor.  Defaults ring buffer size to 0.  Must call setRingBufferSize before ring buffer will work correctly.
	AMTESTRingBuffer();
	/// Constructor.  Requires the size of the ring buffer to allocate the data appropriately.
	AMTESTRingBuffer(int ringBufferSize);
	/// Destructor.
	virtual ~AMTESTRingBuffer();

	/// Returns the value at given position.
	qreal value(int index) const;
	/// Returns the values from the given start position to the end position into the provided array.
	void values(int startIndex, int endIndex, qreal *outputValues) const;

	/// Adds the given value to the ring buffer.
	void addValue(qreal value);
	/// Adds the provided vector to the ring buffer.
	void addValues(const QVector<qreal> &data);

	/// Returns the size of the ring buffer.  This is less than the original size while it is filling the buffer.
	int size() const;

	/// Sets the buffer size.  If you set a smaller size than the current buffer size, you will lose data.
	void setRingBufferSize(int bufferSize);
	/// Clears the ring buffer and reinitializes the zero position and size to zero.
	void clear();

protected:
	/// Helper method that returns the position of the data array based on the zero position.
	int ringBufferIndex(int index) const;

	/// Internal vector that holds the data.
	QVector<qreal> data_;
	/// Index of the ring buffer start.
	int zeroPosition_;
	/// Holds the size of the ring buffer.  While it is being filled this will be less than the buffer size.  Once filled, it will be the same as buffer size.
	int size_;
};

#endif // AMTESTRINGBUFFER_H
