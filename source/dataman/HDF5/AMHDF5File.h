#ifndef AMHDF5FILE_H
#define AMHDF5FILE_H

#include <QObject>

#include <hdf5.h>

#include <QString>

#define AMHDF5FILE_INVALID_CREATE_OPTION 666000
#define AMHDF5FILE_FILE_ALREADY_OPEN 666001
#define AMHDF5FILE_FILE_ALREADY_CLOSED 666002
#define AMHDF5FILE_NOT_HDF5_FILE 666003
#define AMHDF5FILE_INVALID_OPEN_OPTION 666004
#define AMHDF5FILE_FLUSH_FILE_NOT_OPEN 666005

/// This is the base class that will relate all the other pieces of the HDF5 library.  Nothing can work
/// without having an open HDF5 file instance.
class AMHDF5File : public QObject
{
	Q_OBJECT
public:
	/// Enum that handles how file creation should work.  Choices are either open and erase if duplicate or fail if already exists.
	/*!
	 *	OverwriteExisting - if an HDF5 file already exists with the provided name then it will be overwritten and contents deleted.
	 *	DoNotOverwrite - if an HDF5 file already exists with the provided name then it will fail the creation.
	 */
	enum CreateOption {
		OverwriteExisting = 0,
		DoNotOverwrite = 1
	};

	/// Enum that handles how file read/write capabilities are used when opening an existing file.
	/*!
	 *	ReadWrite - opens the file in a mode where you can read and write to the file.
	 *	ReadOnly - opens the file such that you can only read from it.
	*/
	enum OpenOption {
		ReadWrite = 0,
		ReadOnly = 1
	};

	/// Constructor.  Takes the name (path) of the HDF5 file.  Note that this does not open the file.
	explicit AMHDF5File(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMHDF5File();

	/// Returns whether the provided name points to an actual HDF5 file.
	bool isHDF5File() const;
	/// Returns whether the file is currently open.
	bool isOpen() const;
	/// Returns the size of the HDF5 file in megabytes (MB).  Only valid if open.
	int fileSize() const;

signals:

public slots:
	/// Creates an HDF5 file.  If successful, this will double as opening a file for the extent that it is open.
	bool createHDF5File(AMHDF5File::CreateOption option = DoNotOverwrite);
	/// Opens an existing HDF5 file.  Returns false if it failed to open.
	bool openHDF5File(AMHDF5File::OpenOption option = ReadWrite);
	/// Closes the HDF5 file if it still has a valid, open file id.  Returns false if file is already closed.
	bool closeHDF5File();
	/// Flushes the current HDF5 file to disk.  Only valid when the file is open.  Not necessary to call prior to closing the file.
	bool flushHDF5File();

protected:
	/// The path and name of HDF5 file.
	QString name_;
	/// The file id.  This is heavily used by all the other parts of the API to create and associate objects in the HDF5 system.
	hid_t id_;
};

#endif // AMHDF5FILE_H
