#ifndef AMCARRAYHANDLER_H
#define AMCARRAYHANDLER_H

/*!
  * Class for doing standard RAII with a c style arrays.
  */
template <typename T>
class AMCArrayHandler
{
public:
	/*!
	  * Creates a resource handler for a c style string of the provided size
	  * \param size ~ The size of the c array being handled.
	  */
	AMCArrayHandler(int size)
	{
		cArray_ = new T[size];
	}

	/*!
	  * Destroys the AMCArrayHandler, freeing the resources associated with the
	  * contained c array being handled.
	  */
	~AMCArrayHandler()
	{
		delete[] cArray_;
	}

	/*!
	  * A reference to the c style array being handled.
	  */
	T* cArray()
	{
		return cArray_;
	}

private:
	/// The c style array being handled.
	T* cArray_;
};

#endif // AMCARRAYHANDLER_H
