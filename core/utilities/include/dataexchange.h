/********************************************************************************
 *  SALSA                                                                       *
 *  Copyright (C) 2007-2012                                                     *
 *  Gianluca Massera <emmegian@yahoo.it>                                        *
 *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
 *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
 *                                                                              *
 *  This program is free software; you can redistribute it and/or modify        *
 *  it under the terms of the GNU General Public License as published by        *
 *  the Free Software Foundation; either version 2 of the License, or           *
 *  (at your option) any later version.                                         *
 *                                                                              *
 *  This program is distributed in the hope that it will be useful,             *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 *  GNU General Public License for more details.                                *
 *                                                                              *
 *  You should have received a copy of the GNU General Public License           *
 *  along with this program; if not, write to the Free Software                 *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA  *
 ********************************************************************************/

#ifndef DATAEXCHANGE_H
#define DATAEXCHANGE_H

#include <QEvent>
#include <QObject>
#include <QSet>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QSharedData>
#include <QExplicitlySharedDataPointer>
#include <memory>
#include "baseexception.h"
#include "utilitiesexceptions.h"

/**
 * \file dataexchange.h
 *
 * \brief A set of utility classes to exchange data across threads
 *
 * These classes can be used to exchange data (we will use the singular "datum"
 * to indicate a single element) between two objects which live in different
 * threads, one of which "produces" or "uploads" data (we will call it
 * "uploader") and the other which "consumes" or "downloads" it (we will call it
 * "downloader"). The uploaded elements are kept in a queue. It is possible to
 * configure what to do when the queue is full. The possible behaviors are:
 * 	- start overriding older elements. This is useful when the downloader is
 * 	  slower than the uploader (e.g. the downloader is a GUI which only has
 * 	  to display the current data);
 * 	- block the uploader until at least one datum has been downloaded;
 * 	- tell the uploader that there is no more space to hold a new datum,
 * 	  without blocking it;
 * 	- increase the size of the queue.
 * The downloader can query how many data are available and get the next datum
 * from the queue. What happends when a new datum is available in the queue can
 * be configured, too. The possible behaviors are:
 * 	- nothing happends and calls to DataDownloader::downloadDatum() when the
 * 	  queue is empty return nullptr;
 * 	- nothing happends but calls to DataDownloader::downloadDatum() when the
 * 	  queue is empty block the downloader until at least one datum is
 * 	  available;
 * 	- a qt event of type NewDatumEvent is sent to a QObject. For this to
 * 	  work, the object that receives the event must live inside a thread
 * 	  that has the event dispatcher running (e.g. the GUI thread). Calls to
 * 	  DataDownloader::downloadDatum() when the queue is empty return nullptr;
 * 	- a function is called. The notified object must inherit from
 * 	  NewDatumNotifiable and implement the function
 * 	  NewDatumNotifiable::newDatumAvailable(). Moreover the callback
 * 	  function must be thread-safe. Calls to DataDownloader::downloadDatum()
 * 	  when the queue is empty return nullptr.
 * The object that creates data must use an instance of the class DataUploader
 * and the object that uses data must use an instance of the class
 * DataDownloader, with the same DataType.
 * To associate an uploader and a downloader you have to use the
 * GlobalUploaderDownloader::attach function. The objects that are associated
 * must not be associated with other objects, otherwise an exception is thrown.
 * To remove an association call GlobalUploaderDownloader::detach, passing
 * either the uploader or the downloader. Both the attach and the detach
 * functions are thread-safe. Here is an example of the association process:
 *
 * \code
 * // Uploader creation
 * DataUploader<MyData> uploader(queueSize, fullQueueBehavior);
 *
 * ...
 *
 * // Downloader creation (possibly in another thread)
 * DataDownloader<MyData> downloader(newDatumAvailableBehavior);
 *
 * ...
 *
 * // Association
 * GlobalUploaderDownloader::attach(&uploader, &downloader);
 *
 * ...
 *
 * // Removal of an association. You can alternatively pass the downloader
 * GlobalUploaderDownloader::detach(&uploader);
 * \endcode
 *
 * The association can only be 1:1, i.e. one downloader can only be associated
 * with one uploader and vice-versa.
 * The DataType used by the downloader and the uploader has the only requirement
 * to have a constructor without arguments. To create a new datum, call
 * DataUploader::createDatum(). The returned object will be the next element in
 * the queue. It is possible (indeed very probable) that the returned datum is
 * an object that has already been used before, so you shouldn't rely on the
 * fact that is has been just created (if you need this, you can, for example,
 * add a clear()  function to DataType that resets all members). Once you have
 * modified the datum, call DataUploader::uploadDatum(). For example:
 *
 * \code
 * void A::f()
 * {
 * 	// This call can block or return nullptr depending on the FullQueueBehavior
 * 	MyData* d = m_uploader.createDatum();
 *
 * 	// Fill data
 * 	d->...
 * 	d->...
 * 	...
 *
 * 	m_uploader.uploadDatum();
 * }
 * \endcode
 *
 * A more convenient way of creating a new datum is to use the helper class
 * DatumToUpload, which automatically calls createDatum() when it is created
 * and uploadDatum() when it is destroyed. For example:
 *
 * \code
 * void A::f()
 * {
 * 	// This call can block or return nullptr depending on the FullQueueBehavior
 * 	DatumToUpload<MyData> d(m_uploader);
 *
 * 	// Fill data
 * 	d->...
 * 	d->...
 * 	...
 *
 * 	// m_uploader.uploadDatum() will be called automatically before
 * 	// returning from this function (i.e. when the destructor of
 * 	// DatumToUpload is called)
 * }
 * \endcode
 *
 * To get the new datum, call DataDownloader::downloadDatum(). The returned
 * element is guaranteed to remain valid until DataDownloader::downloadDatum()
 * is called again. For example:
 *
 * \code
 * void B::f()
 * {
 * 	// In this example we use polling
 * 	while (true) {
 * 		// This call can block or retun nullptr depending on the
 * 		// NewDatumAvailableBehavior
 * 		const MyData* d = m_downloader.downloadDatum();
 *
 * 		// Uses the datum
 * 		d->...
 * 		d->...
 * 		...
 * 	}
 * }
 * \endcode
 *
 * It is also possible to inherit from DataUploader and DataDownloader to
 * upload/download data.
 * If the communication between two classes needs to be bi-directonal, it is
 * possible to use the DataUploaderDownloader class. It has two template
 * parameters for the data to upload and the data to download. Tha advantage is
 * that the association can be done once for both channels. The
 * DataUploaderDownloader class inherits from both DataUploader and
 * DataDownloader, so it has the methods of both classes. The two ends of the
 * channel must have (of course) complementary data types: the upload data type
 * for one end must be the download data type for the other end and vice-versa.
 * The functions in GlobalUploaderDownloader to create or remove associations
 * are specialized for DataUploaderDownloader, so that you can associate both
 * ends of the communication channel in one call.
 * One final note about stopping all data exchanges. It is possible to call the
 * GlobalUploaderDownloader::stopAllDataExchanges() to end all possible
 * exchanges of data and wake all uploader/downloader. This can be necessary if
 * e.g. you want to stop a simulation and so you need to wake up all sleeping
 * threads so that they can terminate. That function only influences uploader
 * and downloader created before it is called. There is no way to resume data
 * exchanges at the moment, you have to destroy and re-create all uploaders and
 * downloaders.
 *
 * \warning Functions in both DataUploader and DataDownloader are not
 *          thread-safe, so you must not share DataUploader or DataDownloader
 *          objects among objects which live indifferent threads.
 * \note A note regarding downloader notification: it is possible that one
 *       notification is sent even if there are more that one datum available.
 *       This can happend only when the downloader is associated to the uploader
 *       after the uploader has already uploaded some data. If you are sure that
 *       the uploader hasn't uploaded any datum before the association with the
 *       downloader, then each notification means that exactly one datum has
 *       been added. It is however possible that there are fewer available data
 *       than notifications: this can happend if the notification is done via a
 *       QT event, but the event handler is called only after more than the
 *       uploader queue length data has been added. Always check that the
 *       returned datum is not nullptr before using it in this situation. The
 *       callback notification mechanism doesn't have this kind of problems
 * \note As explained above, when creating a new datum it is possible that an
 *       object used before is returned. This means that you should not rely on
 *       the fact that the returned object has been just created.
 * \note When the FullQueueBehavior of an uploader is set to BlockUploader or
 *       the NewDatumAvailableBehavior of a downloader is set to
 *       NoNotificationBlocking, the upload/download function can still return
 *       nullptr if all uploaders/downloaders are woken up by
 *       GlobalUploaderDownloader::stopAllDataExchanges(). This implies
 *       that you must always check the return value of downloader/uploader
 *       functions to get data.
 */

#warning PENSARE A COME FARE LA COSA DEL COMMENTO SOTTO (DICHIARARE STRUTTURA CHE SI SCAMBIA)
// Invece di avere come ora nei Component la possibilità di tornare l'UI manager che crea le gui,
// è meglio avere un meccanismo per il quale ogni Component dichiara di quali
// Uploader/Downloader/UploaderDownloader dispone. Poi si possono registrare delle GUI (forse
// semplicemente dei Component particolari) che allo stesso modo dichiarano
// Uploader/Downloader/UploaderDownloader in modo da poterli poi associare a runtime. Probabilmente
// serve fare delle classi base astratte (virtuali) Uploader e Downloader dalle quali le classi
// template ereditano e poi serve un meccanismo di registrazione analogo a quello dei component
// (o meglio, integrato con quello dei component, forse si possono usare dei typedef di una
// lista template da mettere in component per l'elenco degli Uploader/Downloader/UploaderDownloader
// disponibili e i metodi per ottenerli). Bisogna poi controllare che l'associazione tra Uploader
// e Downloader sia dinamica e thread-safe (o forse questo non serve subito, l'associazione si
// può fare in fase di creazione dei componenti e lasciarla poi fissa).
#warning VEDERE ANCHE QUESTO ALTRO COMMENTO
// MOdificare ConfigurationManager in modo che i Component creati vengano distrutti quando l'ultimo
// ConfigurationMananger viene distrutto e non far distruggere i Component da altri Component (al
// limite mettere la funzione speculare di getObjectFromGroup, una cosa tipo releaseObjectFromGroup
// con un reference counter). Sarebbe bene usare la libreria C++ di Stroustrup e i tool automatici
// (vedi talk alla CppCon 2016). Ricordarsi anche di aggiungere le operation ai component. Se
// possibile splittare i vari pezzi che sono in un component (configurazione, operations,
// uploader/downloader) in interfacce/classi più piccole. Ovviamente ricontrollare il codice e usare
// C++14

namespace salsa {

namespace __DataExchange_internal {
	class QueueHolderBase;
	template <class DataType_t>
	class QueueHolder;
}
template <class DataType_t>
class DatumToUpload;
template <class DataType_t>
class DataDownloader;
template <class DataType_t>
class Downloader;

/**
 * \brief The class used to upload data
 *
 * See dataexchange.h for more information
 * \warning Functions in this class are NOT thread safe
 */
template <class DataType_t>
class SALSA_UTIL_TEMPLATE DataUploader
{
public:
	/**
	 * \brief The type of data being exchanged
	 */
	typedef DataType_t DataType;

	/**
	 * \brief A typedef to use the correct DatumToUpload
	 */
	typedef salsa::DatumToUpload<DataType> DatumToUpload;

	/**
	 * \brief The possible behaviors when the queue is full
	 */
	enum FullQueueBehavior {
		OverrideOlder, /**< Overrides older data */
		BlockUploader, /**< Blocks the uploader until at least one datum
		                    has been downloaded */
		IncreaseQueueSize, /**< Increases the queue size to upload the
		                        new datum */
		SignalUploader /**< Tells the uploader the queue is full */
	};

public:
	/**
	 * \brief Constructor
	 *
	 * \param queueSize the size of the queue. If the b parameter is
	 *                  IncreaseQueueSize, this is the initial size and the
	 *                  queue will never be shorter than this. The queue
	 *                  will always have at least one element (even if this
	 *                  is set to 0)
	 * \param b the behavior when the queue is full
	 */
	DataUploader(unsigned int queueSize, FullQueueBehavior b);

	/**
	 * \brief Destructor
	 */
	~DataUploader();

	/**
	 * \brief Returns the queue size
	 *
	 * \return the queue size
	 */
	unsigned int getQueueSize() const
	{
		return m_queueSize;
	}

	/**
	 * \brief Returns the FullQueueBehavior
	 *
	 * \return the FullQueueBehavior
	 */
	FullQueueBehavior getFullQueueBehavior() const
	{
		return m_fullQueueBehavior;
	}

	/**
	 * \brief Returns true if we are associated with a downloader
	 *
	 * \return true if we are associated with a downloader
	 */
	bool downloaderPresent() const;

	/**
	 * \brief Returns the number of data the queue can hold before becoming
	 *        full
	 *
	 * If the FullQueueBehavior is IncreaseQueueSize, this function always
	 * returns at least 1
	 * \return the number of data the queue can hold before becoming full
	 */
	unsigned int getAvailableSpace() const;

	/**
	 * \brief Returns the number of data currently in the queue
	 *
	 * If the FullQueueBehavior is IncreaseQueueSize, this function can
	 * return a value greater than getQueueSize()
	 * \return the number of data currently in the queue
	 */
	unsigned int getNumDataInQueue() const;

	/**
	 * \brief Returns a pointer to an object that will be the next datum to
	 *        upload
	 *
	 * Modify the object returned by this function, it will be the next
	 * datum to upload. If the queue is full, this function blocks if the
	 * FullQueueBehavior is set to BlockUploader, while it returns nullptr if
	 * the FullQueueBehavior is set to SignalUploader
	 * \return the object that will be the next datum
	 * \warning If you call this function remember to call uploadDatum()
	 *          when done. A more convenient way to create data is to use
	 *          DatumToUpload
	 */
	DataType* createDatum();

	/**
	 * \brief Adds the current datum to the queue
	 *
	 * If createDatum() hasn't been called, this function does nothing
	 */
	void uploadDatum();

	/**
	 * \brief Returns true if a new datum has been created but not uploaded
	 *        (i.e. createDatum() has been called but uploadDatum() hasn't)
	 *
	 * \return true if a new datum has been created but not uploaded
	 */
	bool datumCreatedNotUploaded() const;

	/**
	 * \brief Sets whether an exception has to be thrown if the user tries
	 *        to upload a datum but no association is present
	 *
	 * When this object is constructed, this is set to true
	 * \param v if true an exception is thrown when the user tries to upload
	 *          a datum and no association is present
	 */
	void checkAssociationBeforeUpload(bool v)
	{
		m_checkAssociationBeforeUpload = v;
	}

	/**
	 * \brief Returns true if an exception is thrown if the user tries to
	 *        upload a datum but no association is present
	 *
	 * When this object is constructed, this is set to true
	 * \return true if an exception is thrown if the user tries to upload a
	 *         datum but no association is present
	 */
	bool associationBeforeUploadChecked() const
	{
		return m_checkAssociationBeforeUpload;
	}

private:
	/**
	 * \brief The size of the queue
	 *
	 * If m_fullQueueBehavior is IncreaseQueueSize, this is the initial size
	 * and the queue will never be shorter than this.
	 */
	const unsigned int m_queueSize;

	/**
	 * \brief The behavior when the queue is full
	 */
	const FullQueueBehavior m_fullQueueBehavior;

	/**
	 * \brief The object containig the queue
	 *
	 * The queue is created by uploaders and is then shared with downloaders
	 * during the attach process. The QExplicitlySharedDataPointer object
	 * takes care of destroying the queue once no reference to it is still
	 * present
	 */
	QExplicitlySharedDataPointer<__DataExchange_internal::QueueHolder<DataType> > m_queue;

	/**
	 * \brief If true an exception is thrown if the user tries to upload a
	 *        datum but no association is present
	 *
	 * This is true by default
	 */
	bool m_checkAssociationBeforeUpload;

	/**
	 * \brief Copy constructor
	 *
	 * This is here to prevent usage
	 */
	DataUploader(const DataUploader<DataType>& other);

	/**
	 * \brief Copy operator
	 *
	 * This is here to prevent usage
	 */
	DataUploader& operator=(const DataUploader<DataType>& other);

	/**
	 * \brief GlobalUploaderDownloader is friend to access the queue
	 */
	friend class GlobalUploaderDownloader;
};

/**
 * \brief The class used to implement RAII for data uploading
 *
 * A helper class to ease the creation and upload of a datum: the datum is
 * created when this object is created and it is uploaded when this object is
 * destroyed. See dataexchange.h for more information
 * \warning Functions in this class are NOT thread safe
 */
template <class DataType_t>
class SALSA_UTIL_TEMPLATE DatumToUpload
{
public:

	/**
	 * \brief The type of data being exchanged
	 */
	typedef DataType_t DataType;

public:
	/**
	 * \brief Constructor
	 *
	 * \param uploader the uploader that has to upload a new datum
	 */
	DatumToUpload(DataUploader<DataType>& uploader) :
		m_uploader(uploader),
		m_datum(m_uploader.createDatum()),
		m_datumUploaded(false)
	{
	}

	/**
	 * \brief Destructor
	 *
	 * Uploads the datum if it hasn't been uploaded yet
	 */
	~DatumToUpload()
	{
		uploadDatum();
	}

	/**
	 * \brief Uploads the datum
	 *
	 * From this point on the datum pointer will be nullptr
	 */
	void uploadDatum()
	{
		if (!m_datumUploaded) {
			m_uploader.uploadDatum();
			m_datum = nullptr;
			m_datumUploaded = true;
		}
	}

	/**
	 * \brief Overloading of the -> operator to access the datum (const
	 *        version)
	 *
	 * \return a const pointer to the datum
	 */
	const DataType* operator->() const
	{
		return m_datum;
	}

	/**
	 * \brief Overloading of the -> operator to access the datum
	 *
	 * \return a pointer to the datum
	 */
	DataType* operator->()
	{
		return m_datum;
	}

	/**
	 * \brief Comparison operator with a pointer to DataType
	 *
	 * \param other a pointer to DataType
	 * \return true if the two pointer are the same
	 */
	bool operator==(const DataType* other) const
	{
		return (m_datum == other);
	}

	/**
	 * \brief Cast to bool
	 *
	 * \return true if the datum pointer is non-null
	 */
	operator bool() const
	{
		return (m_datum != nullptr);
	}

private:
	/**
	 * \brief The uploader
	 */
	DataUploader<DataType>& m_uploader;

	/**
	 * \brief The datum
	 */
	DataType* m_datum;

	/**
	 * \brief Whether uploadDatum() has been called or not
	 */
	bool m_datumUploaded;

	/**
	 * \brief Copy constructor
	 *
	 * Here to disallow usage
	 */
	DatumToUpload(const DatumToUpload<DataType>& other);

	/**
	 * \brief Copy operator
	 *
	 * Here to disallow usage
	 */
	DatumToUpload& operator=(const DatumToUpload<DataType>& other);
};

/**
 * \brief The event sent to downloader when a new datum is ready
 *
 * If the downloader chooses to be notified with a qt event, this is the event
 * that it receives
 */
template<class DataType_t>
class SALSA_UTIL_TEMPLATE NewDatumEvent : public QEvent
{
public:
	/**
	 * \brief The ID of this event
	 */
	static const QEvent::Type newDatumEventType = static_cast<QEvent::Type>(QEvent::User + 1);

	/**
	 * \brief The type of data being exchanged
	 */
	typedef DataType_t DataType;

public:
	/**
	 * \brief Constructor
	 *
	 * \param downloader the downloader object which triggered the event
	 */
	NewDatumEvent(DataDownloader<DataType>* downloader) :
		QEvent(newDatumEventType),
		m_downloader(downloader)
	{
	}

	/**
	 * \brief Destructor
	 */
	virtual ~NewDatumEvent()
	{
	}

	/**
	 * \brief Returns the downloader object which triggered this event
	 *        (const version)
	 *
	 * \return a const pointer to the downloader that triggered this event
	 */
	const DataDownloader<DataType>* getDownloader() const
	{
		return m_downloader;
	}

	/**
	 * \brief Returns the downloader object which triggered this event
	 *
	 * \return a pointer to the downloader that triggered this event
	 */
	DataDownloader<DataType>* getDownloader()
	{
		return m_downloader;
	}

private:
	/**
	 * \brief The downloader that triggered this event
	 */
	DataDownloader<DataType>* const m_downloader;
};

/**
 * \brief The interface for classes that want to be notified when a new datum is
 *        available
 */
template <class DataType_t>
class SALSA_UTIL_TEMPLATE NewDatumNotifiable
{
public:
	/**
	 * \brief The type of data being exchanged
	 */
	typedef DataType_t DataType;

public:
	/**
	 * \brief Destructor
	 */
	virtual ~NewDatumNotifiable()
	{
	}

	/**
	 * \brief The function called when a new datum is available
	 *
	 * This function is called when a new datum is available.
	 * \param downloader the downloader with the available datum
	 * \note This should be thread-safe, as the notification arrives from
	 *       the thread where the uploader lives
	 */
	virtual void newDatumAvailable(DataDownloader<DataType>* downloader) = 0;
};

/**
 * \brief The class used to download data
 *
 * See dataexchange.h for more information
 * \warning Functions in this class are NOT thread safe
 */
template <class DataType_t>
class SALSA_UTIL_TEMPLATE DataDownloader
{
public:
	/**
	 * \brief The type of data being exchanged
	 */
	typedef DataType_t DataType;

	/**
	 * \brief The possible behaviors when a new datum arrives
	 */
	enum NewDatumAvailableBehavior {
		NoNotification, /**< No notification */
		NoNotificationBlocking, /**< Like NoNotification but blocks the
		                             downloader if it tries to get a
		                             datum when the queue is empty */
		QtEvent, /**< Send NewDatumEvent when a new datum is
		              available */
		Callback /**< Calls a function when a new datum is available */
	};

public:
	/**
	 * \brief Constructor
	 *
	 * \param b the behavior when a new datum arrives. In this constructor
	 *          this must be either NoNotification or
	 *          NoNotificationBlocking, otherwise an exception is thrown
	 */
	DataDownloader(NewDatumAvailableBehavior b);

	/**
	 * \brief Constructor
	 *
	 * This constructor sets the NewDatumAvailableBehavior to QtEvent
	 * \param o the object to send notifications to when a new datum is
	 *          available. This must not be nullptr.
	 */
	DataDownloader(QObject* o);

	/**
	 * \brief Constructor
	 *
	 * This constructor sets the NewDatumAvailableBehavior to Callback
	 * \param o the object whose callback has to be called when a new datum
	 *          is available. This must not be nullptr.
	 */
	DataDownloader(NewDatumNotifiable<DataType>* o);

	/**
	 * \brief Destructor
	 */
	~DataDownloader();

	/**
	 * \brief Returns the NewDatumAvailableBehavior
	 *
	 * \return the NewDatumAvailableBehavior
	 */
	NewDatumAvailableBehavior getNewDatumAvailableBehavior() const
	{
		return m_newDatumAvailableBehavior;
	}

	/**
	 * \brief Returns true if we are associated with an uploader
	 *
	 * \return true if we are associated with an uploader
	 */
	bool uploaderPresent() const;

	/**
	 * \brief Returns the number of available data
	 *
	 * \return the number of available data
	 */
	unsigned int getNumAvailableData() const;

	/**
	 * \brief Returns a pointer to the next datum
	 *
	 * This returns the next datum. The returned datum is guaranteed to
	 * remain valid until the next call to this function. If the queue is
	 * empty this function blocks if the NewDatumAvailableBehavior is set to
	 * NoNotificationBlocking, otherwise it returns nullptr. If we are not
	 * associated with any uploader, this function returns nullptr.
	 * \return the next datum
	 */
	const DataType* downloadDatum();

private:
	/**
	 * \brief This function is called by the uploader when a new datum is
	 *        available, to send notification.
	 *
	 * \note this function is thread-safe
	 */
	void sendNotification();

	/**
	 * \brief The behavior when a new datum is available
	 */
	const NewDatumAvailableBehavior m_newDatumAvailableBehavior;

	/**
	 * \brief The QObject to notify in case NewDatumAvailableBehavior is set
	 *        to QtEvent
	 */
	QObject* const m_qoject;

	/**
	 * \brief The object with the callback to call in case
	 *        NewDatumAvailableBehavior is set to Callback
	 */
	NewDatumNotifiable<DataType>* const m_newDatumNotifiable;

	/**
	 * \brief The object containig the queue
	 *
	 * The queue is created by the uploader
	 */
	QExplicitlySharedDataPointer<__DataExchange_internal::QueueHolder<DataType> > m_queue;

	/**
	 * \brief The mutex protecting the m_queue variable
	 *
	 * This is needed because the m_queue variable could be changed in other
	 * threads by GlobalUploaderDownloader functions. This is acquired in
	 * basically every function call, this means that this class is
	 * practically thread-safe (even if it is not meant to be used in
	 * different threads concurrently). The mutex is mutable because it is
	 * used in const functions
	 */
	mutable QMutex m_mutex;

	/**
	 * \brief The uploader is friend to call sendNotification()
	 */
	friend class DataUploader<DataType>;

	/**
	 * \brief GlobalUploaderDownloader is friend to access private members
	 */
	friend class GlobalUploaderDownloader;

	/**
	 * \brief Copy constructor
	 *
	 * This is here to prevent usage
	 */
	DataDownloader(const DataDownloader<DataType>& other);

	/**
	 * \brief Copy operator
	 *
	 * This is here to prevent usage
	 */
	DataDownloader& operator=(const DataDownloader<DataType>& other);
};

/**
 * \brief A class for bi-directional communication
 *
 * This class inherits from both DataUploader and DataDownloader. See
 * dataexchange.h for more information
 */
template <class UploadedData_t, class DownloadedData_t>
class SALSA_UTIL_TEMPLATE DataUploaderDownloader : public DataUploader<UploadedData_t>, public DataDownloader<DownloadedData_t>
{
public:
	/**
	 * \brief The type of data being uploaded by this class
	 */
	typedef UploadedData_t UploadedData;

	/**
	 * \brief The type of data being downloaded by this class
	 */
	typedef DownloadedData_t DownloadedData;

	/**
	 * \brief A typedef to easily access the FullQueueBehavior type
	 */
	typedef typename DataUploader<UploadedData>::FullQueueBehavior FullQueueBehavior;

	/**
	 * \brief A typedef to easily access the NewDatumAvailableBehavior type
	 */
	typedef typename DataDownloader<DownloadedData>::NewDatumAvailableBehavior NewDatumAvailableBehavior;

public:
	/**
	 * \brief Constructor
	 *
	 * \param uploadQueueSize the size of the upload queue. If the b
	 *                        parameter is IncreaseQueueSize, this is the
	 *                        initial size and the queue will never be
	 *                        shorter than this. The queue will always have
	 *                        at least one element (even if this is set to
	 *                        0)
	 * \param fullQueueBehavior the behavior of the uploader when the queue
	 *                          is full
	 * \param newDatumAvailableBehavior the behavior of the downloader when
	 *                                  a new datum arrives. In this
	 *                                  constructor this must be either
	 *                                  NoNotification or
	 *                                  NoNotificationBlocking, otherwise an
	 *                                  exception is thrown
	 */
	DataUploaderDownloader(unsigned int uploadQueueSize, FullQueueBehavior fullQueueBehavior, NewDatumAvailableBehavior newDatumAvailableBehavior) :
		DataUploader<UploadedData>(uploadQueueSize, fullQueueBehavior),
		DataDownloader<DownloadedData>(newDatumAvailableBehavior)
	{
	}

	/**
	 * \brief Constructor
	 *
	 * This constructor sets the NewDatumAvailableBehavior of the downloader
	 * to QtEvent
	 * \param uploadQueueSize the size of the upload queue. If the b
	 *                        parameter is IncreaseQueueSize, this is the
	 *                        initial size and the queue will never be
	 *                        shorter than this. The queue will always have
	 *                        at least one element (even if this is set to
	 *                        0)
	 * \param fullQueueBehavior the behavior of the uploader when the queue
	 *                          is full
	 * \param o the object to send notifications to when a new datum is
	 *          available. This must not be nullptr.
	 */
	DataUploaderDownloader(unsigned int uploadQueueSize, FullQueueBehavior fullQueueBehavior, QObject* o) :
		DataUploader<UploadedData>(uploadQueueSize, fullQueueBehavior),
		DataDownloader<DownloadedData>(o)
	{
	}

	/**
	 * \brief Constructor
	 *
	 * This constructor sets the NewDatumAvailableBehavior of the downloader
	 * to Callback
	 * \param uploadQueueSize the size of the upload queue. If the b
	 *                        parameter is IncreaseQueueSize, this is the
	 *                        initial size and the queue will never be
	 *                        shorter than this. The queue will always have
	 *                        at least one element (even if this is set to
	 *                        0)
	 * \param fullQueueBehavior the behavior of the uploader when the queue
	 *                          is full
	 * \param o the object whose callback has to be called when a new datum
	 *          is available. This must not be nullptr.
	 */
	DataUploaderDownloader(unsigned int uploadQueueSize, FullQueueBehavior fullQueueBehavior, NewDatumNotifiable<DownloadedData>* o) :
		DataUploader<UploadedData>(uploadQueueSize, fullQueueBehavior),
		DataDownloader<DownloadedData>(o)
	{
	}

	/**
	 * \brief Destructor
	 */
	~DataUploaderDownloader()
	{
		// Nothing to do here
	}
};

/**
 * \brief The class to create or remove associations and to wake all sleeping
 *        uploaders and downloaders
 *
 * This class is a singleton and only has static public functions. This class is
 * used for two purposes: creating or removing associations between uploaders
 * and downloaders and stopping all data exchange. See the description of
 * functions for more information.
 * \note All functions in this class (both public and private) are thread-safe
 */
class SALSA_UTIL_API GlobalUploaderDownloader
{
public:
	/**
	 * \brief The function to associate an uploader and a downloader
	 *
	 * Call this function to create an association between an uploader and a
	 * downloader. Both must not be associated with anything else (use
	 * detach in case they are), otherwise an exception is thrown
	 * \param uploader the uploader to associate with the downloader. Must
	 *                 not be nullptr
	 * \param downloader the downloader to associate with the uploader. Must
	 *                   not be nullptr
	 */
	template <class DataType>
	static void attach(DataUploader<DataType>* uploader, DataDownloader<DataType>* downloader);

	/**
	 * \brief The function to associate two DataUploaderDownloader
	 *
	 * This is the same as the attach function working on a single channel
	 * but performs the association of the two ends of two
	 * DataUploaderDownloader objects at the same time. Both objects must
	 * not be associated with anything else (use detach in case they are),
	 * otherwise an exception is thrown
	 * \param first the first object of the association. Must not be nullptr
	 * \param second the second object of the association. Must not be nullptr
	 */
	template <class DataType1, class DataType2>
	static void attach(DataUploaderDownloader<DataType1, DataType2>* first, DataUploaderDownloader<DataType2, DataType1>* second);

	/**
	 * \brief Removes an association
	 *
	 * This function breaks the association of the given uploader with the
	 * downloader. If the uploader is not associated, nothing is done
	 * \param uploader the uploader whose association to break
	 */
	template <class DataType>
	static void detach(DataUploader<DataType>* uploader);

	/**
	 * \brief Removes an association
	 *
	 * This function breaks the association of the given downloader with the
	 * uploader. If the downloader is not associated, nothing is done
	 * \param downloader the downloader whose association to break
	 */
	template <class DataType>
	static void detach(DataDownloader<DataType>* downloader);

	/**
	 * \brief Removes an association
	 *
	 * This function breaks the association of the given
	 * DataUploaderDownloader with its peer. If the object is not
	 * associated, nothing is done
	 * \param uploaderDownloader the object whose association to break
	 */
	template <class DataType1, class DataType2>
	static void detach(DataUploaderDownloader<DataType1, DataType2>* uploaderDownloader);

	/**
	 * \brief Stops all data exchanges
	 *
	 * Call this function to wake up all sleeping uploaders and downloaders
	 * and stop all data exchanges. The function only influences uploaders
	 * and downloaders that have been created before it is called. All
	 * attempts to get data to upload/download after this function is called
	 * will return nullptr (see note in dataexchange.h description). It is not
	 * possible to resume exchanges, you must destroy uploaders and
	 * downloaders and then re-create them.
	 */
	static void stopAllDataExchanges();

private:
	/**
	 * \brief Constructor
	 *
	 * Private because this is a singleton
	 */
	GlobalUploaderDownloader();

	/**
	 * \brief This is a singleton, this function returns the only instance
	 *
	 * \return the only instance of this class
	 */
	static GlobalUploaderDownloader& getInstance();

	/**
	 * \brief The function to associate an uploader and a downloader
	 *
	 * The public static function calls this
	 * \param uploader the uploader to associate with the downloader
	 * \param downloader the downloader to associate with the uploader
	 */
	template <class DataType>
	void internalAttach(DataUploader<DataType>* uploader, DataDownloader<DataType>* downloader);

	/**
	 * \brief The function to associate two DataUploaderDownloader
	 *
	 * The public static function calls this
	 * \param first the first object of the association
	 * \param second the second object of the association
	 */
	template <class DataType1, class DataType2>
	void internalAttach(DataUploaderDownloader<DataType1, DataType2>* first, DataUploaderDownloader<DataType2, DataType1>* second);

	/**
	 * \brief Removes an association
	 *
	 * The public static function calls this
	 * \param uploader the uploader whose association to break
	 */
	template <class DataType>
	void internalDetach(DataUploader<DataType>* uploader);

	/**
	 * \brief Removes an association
	 *
	 * The public static function calls this
	 * \param downloader the downloader whose association to break
	 */
	template <class DataType>
	void internalDetach(DataDownloader<DataType>* downloader);

	/**
	 * \brief Removes an association
	 *
	 * The public static function calls this
	 * \param uploaderDownloader the object whose association to break
	 */
	template <class DataType1, class DataType2>
	void internalDetach(DataUploaderDownloader<DataType1, DataType2>* uploaderDownloader);

	/**
	 * \brief The function actually stopping all data exchanges
	 *
	 * The public static function calls this
	 */
	void internalStopAllDataExchanges();

	/**
	 * \brief Adds a new QueueHolder to the set
	 *
	 * This is private so that only QueueHolder can call it
	 * \param queueHolder the object to add to the list
	 */
	void addQueueHolder(__DataExchange_internal::QueueHolderBase* queueHolder);

	/**
	 * \brief Removes the given QueueHolder from the set
	 *
	 * This is private so that only QueueHolder can call it
	 * \param queueHolder the object to remove from the list
	 */
	void removeQueueHolder(__DataExchange_internal::QueueHolderBase* queueHolder);

	/**
	 * \brief This is called by the destructor of DataUploader
	 *
	 * We need this function to prevent race conditions when the objec being
	 * destroyed is also part of an association that is being changed. In
	 * this function we remove the association with the downloader, if
	 * present. This is different from detach() applied to the uploader,
	 * because here we must not remove the queue from the downloader
	 * (deleting the queue becomes the downloader reponsability, which can
	 * also consume data still in it)
	 * \param uploader The uploader that is being destroyed
	 */
	template <class DataType>
	void uploaderDestroyed(DataUploader<DataType>* uploader);

	/**
	 * \brief This is called by the destructor of DataDownloader
	 *
	 * We need this function to prevent race conditions when the objec being
	 * destroyed is also part of an association that is being changed. In
	 * this function we remove the association with the uploader, if
	 * present. This is the same as detaching the downloader (this function
	 * simply call detach)
	 * \param downloader The downloader that is being destroyed
	 */
	template <class DataType>
	void downloaderDestroyed(DataDownloader<DataType>* downloader);

	/**
	 * \brief The set of all queue holders created so far
	 */
	QSet<__DataExchange_internal::QueueHolderBase*> m_queueHolders;

	/**
	 * \brief The mutex serializing access to the set of queue holders and
	 *        serializing associations creation and removal
	 */
	QMutex m_mutex;

	/**
	 * \brief DataUploader is friend to access private members
	 */
	template <class DataType_t>
	friend class DataUploader;

	/**
	 * \brief DataDownloader is friend to access private members
	 */
	template <class DataType_t>
	friend class DataDownloader;

	/**
	 * \brief QueueHolder is friend to access private members
	 */
	template <class DataType_t>
	friend class __DataExchange_internal::QueueHolder;

private:
	/**
	 * \brief Copy constructor
	 *
	 * Here to prevent copy
	 */
	GlobalUploaderDownloader(const GlobalUploaderDownloader&);

	/**
	 * \brief Copy operator
	 *
	 * Here to prevent copy
	 */
	GlobalUploaderDownloader& operator=(const GlobalUploaderDownloader&);
};

}  // end namespace salsa

// Implementation of all template members
#include <QCoreApplication>
#include <QMutex>
#include <QLinkedList>
#include <QWaitCondition>
#include <QMutexLocker>
#include <QtGlobal>

namespace salsa {

/**
 * \brief The namespace with classes used in the implementation
 *
 * \internal
 */
namespace __DataExchange_internal {
	/**
	 * \brief The parent of the class containing the queue and all related
	 *        elements
	 *
	 * This just contains the lock to use when accessing the queue, the
	 * QWaitCondition and the flag to signal that data exchange should stop.
	 * We need this base class because of GlobalUploaderDownloader
	 * \internal
	 */
	class QueueHolderBase : public QSharedData
	{
	public:
		/**
		 * \brief Constructor
		 */
		QueueHolderBase()
			: mutex()
			, waitCondition()
			, dataExchangeStopped(false)
		{
		}

	public:
		/**
		 * \brief The mutex protecting from concurrent accesses to data
		 *        in this object
		 */
		QMutex mutex;

		/**
		 * \brief The wait condition used to wake the blocked downloader
		 *        when a new datum arrives or the uploader when the
		 *        queue is full
		 */
		QWaitCondition waitCondition;

		/**
		 * \brief If true no data exchange is possible
		 */
		bool dataExchangeStopped;
	};

	/**
	 * \brief The class containing the queue and all related elements
	 *
	 * This also contains the lock to use when accessing the queue
	 * \internal
	 */
	template <class DataType_t>
	class QueueHolder : public QueueHolderBase
	{
	public:
		/**
		 * \brief The type of data being exchanged
		 */
		typedef DataType_t DataType;

	public:
		/**
		 * \brief Constructor
		 *
		 * This creates the queue ad allocates all objects used here
		 * \param queueSize the initial size of the queue
		 * \param u the uploader associated with this queue
		 */
		QueueHolder(unsigned int queueSize, DataUploader<DataType>* u)
			: QueueHolderBase()
			, queue()
			, availableSpace(queueSize)
			, numDataInQueue(0)
			, queueFullLastDatumCreation(false)
			, datumCreatedNotUploaded(false)
			, nextUploaderDatum(nullptr)
			, currentDownloaderDatum(nullptr)
			, nextUploadIt()
			, nextDownloadIt()
			, uploader(u)
			, downloader(nullptr)
		{
			// Allocating all memory. We use unique_ptr to ensure exception safety
			std::unique_ptr<DataType> uploaderDatum(new DataType());
			std::unique_ptr<DataType> downloaderDatum(new DataType());

			// Explicitly using a try-catch block to be exception-safe
			try {
				for (unsigned int i = 0; i < queueSize; i++) {
					queue.push_back(new DataType());
				}
			} catch (...) {
				// If an exception is thrown, deleting all objects allocated so far
				foreach(DataType* d, queue) {
					delete d;
				}

				// Propagating exception
				throw;
			}

			// Now initializing the iterators for the uploader and the downloader
			nextUploadIt = queue.begin();
			nextDownloadIt = queue.begin();

			// Now releasing the unique_ptrs
			nextUploaderDatum = uploaderDatum.release();
			currentDownloaderDatum = downloaderDatum.release();

			// Adding ourself to the list of queue holders
			GlobalUploaderDownloader::getInstance().addQueueHolder(this);
		}

		/**
		 * \brief Destructor
		 *
		 * This deallocates all memory
		 */
		~QueueHolder()
		{
			// Removing ourself from the list of queue holders
			GlobalUploaderDownloader::getInstance().removeQueueHolder(this);

			foreach(DataType* d, queue) {
				delete d;
			}
			queue.clear();

			delete nextUploaderDatum;
			delete currentDownloaderDatum;
		}

	public:
		/**
		 * \brief The queue of data
		 */
		QLinkedList<DataType*> queue;

		/**
		 * \brief The number of data the queue can hold before becoming
		 *        full
		 */
		unsigned int availableSpace;

		/**
		 * \brief The number of data currently in the queue
		 */
		unsigned int numDataInQueue;

		/**
		 * \brief Whether the queue was full the last time a datum was
		 *        created
		 */
		bool queueFullLastDatumCreation;

		/**
		 * \brief True if a new datum has been created but not uploaded
		 *        (i.e. createDatum() in the uploader has been called
		 *        but uploadDatum() hasn't been called yet)
		 */
		bool datumCreatedNotUploaded;

		/**
		 * \brief The next datum the uploader will use
		 *
		 * This will go in the queue once uploading has ended and will
		 * be replaced by an element of the queue. If however the
		 * uploader FullQueueBehavior is IncreaseQueueSize, a new object
		 * could be created
		 */
		DataType* nextUploaderDatum;

		/**
		 * \brief The datum the downloader is currently using
		 *
		 * This will go back in the queue once the downloader asks for a
		 * new datum
		 */
		DataType* currentDownloaderDatum;

		/**
		 * \brief The iterator to the element of the queue where the
		 * \      next datum uploaded by the uploader will be put
		 */
		typename QLinkedList<DataType*>::iterator nextUploadIt;

		/**
		 * \brief The position in the queue that will be read next by
		 *        the downloader
		 */
		typename QLinkedList<DataType*>::iterator nextDownloadIt;

		/**
		 * \brief The uploader associated with this queue
		 *
		 * This is only modified by GlobalUploaderDownloader when its
		 * mutex is locked
		 */
		DataUploader<DataType>* uploader;

		/**
		 * \brief The downloader associated with this queue
		 *
		 * This is only modified by GlobalUploaderDownloader when its
		 * mutex is locked
		 */
		DataDownloader<DataType>* downloader;
	};
}

template <class DataType_t>
DataUploader<DataType_t>::DataUploader(unsigned int queueSize, FullQueueBehavior b) :
	m_queueSize((queueSize == 0) ? 1 : queueSize),
	m_fullQueueBehavior(b),
	m_queue(new __DataExchange_internal::QueueHolder<DataType>(m_queueSize, this)),
	m_checkAssociationBeforeUpload(true)
{
}

template <class DataType_t>
DataUploader<DataType_t>::~DataUploader()
{
	// Calling the function of GlobalUploaderDownloader, it will take care of removing the
	// association, if present
	GlobalUploaderDownloader::getInstance().uploaderDestroyed(this);

	// The queue is deleted if we are the only one referencing it (we have used
	// QExplicitlySharedDataPointer)
}

template <class DataType_t>
bool DataUploader<DataType_t>::downloaderPresent() const
{
	QMutexLocker locker(&(m_queue->mutex));

	return (m_queue->downloader != nullptr);
}

template <class DataType_t>
unsigned int DataUploader<DataType_t>::getAvailableSpace() const
{
	QMutexLocker locker(&(m_queue->mutex));

	if (m_fullQueueBehavior == IncreaseQueueSize) {
		return (m_queue->availableSpace == 0) ? 1 : m_queue->availableSpace;
	} else {
		return m_queue->availableSpace;
	}
}

template <class DataType_t>
unsigned int DataUploader<DataType_t>::getNumDataInQueue() const
{
	QMutexLocker locker(&(m_queue->mutex));

	return m_queue->numDataInQueue;
}

template <class DataType_t>
DataType_t* DataUploader<DataType_t>::createDatum()
{
	QMutexLocker locker(&(m_queue->mutex));

	// If the datum has already been created, returning the same datum again
	if (m_queue->datumCreatedNotUploaded) {
		return m_queue->nextUploaderDatum;
	}

	// Checking whether data exchange has been stopped
	if (m_queue->dataExchangeStopped) {
		return nullptr;
	}

	// Checking if we are associated with a downloader if we have to
	if (m_checkAssociationBeforeUpload && (m_queue->downloader == nullptr)) {
		throw UploaderDownloaderAssociationNotPresentException(UploaderDownloaderAssociationNotPresentException::DownloaderNotPresent);
	}

	// Resetting the flag signalling whether the queue was full
	m_queue->queueFullLastDatumCreation = false;

	// Checking if the queue is full
	if (m_queue->availableSpace == 0) {
		// Checking that uploader and downloader iterators point to the same location (this must
		// always happend if we get here)
		Q_ASSERT(m_queue->nextUploadIt == m_queue->nextDownloadIt);

		// The queue is full, setting the flag
		m_queue->queueFullLastDatumCreation = true;

		// What to do depends on the FullQueueBehavior
		switch (m_fullQueueBehavior) {
			case OverrideOlder:
				// The next datum would go where the downloader is going to take the element, so
				// we have to move the downloader forward
				++m_queue->nextDownloadIt;
				if (m_queue->nextDownloadIt == m_queue->queue.end()) {
					m_queue->nextDownloadIt = m_queue->queue.begin();
				}
				break;
			case BlockUploader:
				// Waiting on the wait condition, we will be woke up when the downloader downloads
				// an element. In that case the nextDownloadIt has been moved forward, so we can safely
				// use nextUploadIt
				m_queue->waitCondition.wait(&m_queue->mutex);

				// If we were woken up because data exchange has been stopped, simply returning nullptr
				if (m_queue->dataExchangeStopped) {
					return nullptr;
				}
				break;
			case IncreaseQueueSize: {
					// We have to add a datum to the queue at the current location. After the current
					// datum has been uploaded, this element will be the nextUploaderDatum
				std::unique_ptr<DataType> d(new DataType());
					m_queue->nextUploadIt = m_queue->queue.insert(m_queue->nextDownloadIt, d.get());
					d.release();
				} break;
			case SignalUploader:
				// Returning nullptr to tell the uploader that there is no space
				return nullptr;
				break;
		}
	}

	m_queue->datumCreatedNotUploaded = true;

	// Returning the datum to modify
	return m_queue->nextUploaderDatum;
}

template <class DataType_t>
void DataUploader<DataType_t>::uploadDatum()
{
	QMutexLocker locker(&(m_queue->mutex));

	// If the datum hasn't been created, doing nothing
	if (!m_queue->datumCreatedNotUploaded) {
		return;
	}

	// Checking whether data exchange has been stopped
	if (m_queue->dataExchangeStopped) {
		return;
	}

	// Putting the new datum in the queue, extracting the next element and moving the uploader iterator forward
	DataType* tmp = *m_queue->nextUploadIt;
	*m_queue->nextUploadIt = m_queue->nextUploaderDatum;
	m_queue->nextUploaderDatum = tmp;
	++m_queue->nextUploadIt;
	if (m_queue->nextUploadIt == m_queue->queue.end()) {
		m_queue->nextUploadIt = m_queue->queue.begin();
	}

	// To update the variables with available space and data in queue, we have to check if the queue was full
	// and then check which is the FullQueueBehavior, to undestand what has been done in createDatum()
	if (m_queue->queueFullLastDatumCreation) {
		switch (m_fullQueueBehavior) {
			case OverrideOlder:
				// Nothing has changed, the queue is still full
				break;
			case BlockUploader:
				// The downloader has downloaded one datum before we could add the new one, so we have to update the counters
				--m_queue->availableSpace;
				++m_queue->numDataInQueue;
				break;
			case IncreaseQueueSize:
				// The queue has grown, but still there is no space left. We only have to increase numDataInQueue
				++m_queue->numDataInQueue;
				break;
			case SignalUploader:
				// Nothing has been done (we should never get here)
				Q_ASSERT(false);
				break;
		}
	} else {
		--m_queue->availableSpace;
		++m_queue->numDataInQueue;
	}

	m_queue->datumCreatedNotUploaded = false;

	// Waking up the downloader, in case it was sleeping. This doesn't conflict with sendNotification() because if the
	// downloader is sleeping it is not expecting any notification
	m_queue->waitCondition.wakeAll();

	// Now we have to notify the downloader
	if (m_queue->downloader != nullptr) {
		// If the downloader expects a callback to be called, we have to release the lock, otherwise a deadlock
		// is possible if the downloader tries to get the datum from inside the callback
		if (m_queue->downloader->m_newDatumAvailableBehavior == DataDownloader<DataType>::Callback) {
			locker.unlock();
		}

		// Notifying the downloader
		m_queue->downloader->sendNotification();
	}
}

template <class DataType_t>
bool DataUploader<DataType_t>::datumCreatedNotUploaded() const
{
	QMutexLocker locker(&(m_queue->mutex));

	return m_queue->datumCreatedNotUploaded;
}

template <class DataType_t>
DataDownloader<DataType_t>::DataDownloader(NewDatumAvailableBehavior b) :
	m_newDatumAvailableBehavior(b),
	m_qoject(nullptr),
	m_newDatumNotifiable(nullptr),
	m_queue(),
	m_mutex()
{
	if ((m_newDatumAvailableBehavior != NoNotification) && (m_newDatumAvailableBehavior != NoNotificationBlocking)) {
		if (m_newDatumAvailableBehavior == QtEvent) {
			throw InvalidNewDatumAvailableBehaviorException("when the NewDatumAvailableBehavior is \"QtEvent\" you must specify the QObject that receives the event");
		} else if (m_newDatumAvailableBehavior == Callback) {
			throw InvalidNewDatumAvailableBehaviorException("when the NewDatumAvailableBehavior is \"Callback\" you must specify the NewDatumNotifiable object whose callback is called");
		}
	}
}

template <class DataType_t>
DataDownloader<DataType_t>::DataDownloader(QObject* o) :
	m_newDatumAvailableBehavior(QtEvent),
	m_qoject(o),
	m_newDatumNotifiable(nullptr),
	m_queue(),
	m_mutex()
{
	if (m_qoject == nullptr) {
		throw InvalidNewDatumAvailableBehaviorException("when the NewDatumAvailableBehavior is \"QtEvent\" you must specify a valid (i.e. not nullptr) QObject");
	}
}

template <class DataType_t>
DataDownloader<DataType_t>::DataDownloader(NewDatumNotifiable<DataType>* o) :
	m_newDatumAvailableBehavior(Callback),
	m_qoject(nullptr),
	m_newDatumNotifiable(o),
	m_queue(),
	m_mutex()
{
	if (m_newDatumNotifiable == nullptr) {
		throw InvalidNewDatumAvailableBehaviorException("when the NewDatumAvailableBehavior is \"Callback\" you must specify a valid (i.e. not nullptr) NewDatumNotifiable");
	}
}

template <class DataType_t>
DataDownloader<DataType_t>::~DataDownloader()
{
	// Calling the function of GlobalUploaderDownloader, it will take care of removing the
	// association, if present
	GlobalUploaderDownloader::getInstance().downloaderDestroyed(this);

	// The queue is deleted if we are the only one referencing it (we have used
	// QExplicitlySharedDataPointer)
}

template <class DataType_t>
bool DataDownloader<DataType_t>::uploaderPresent() const
{
	QMutexLocker internalLocker(&m_mutex);

	if (!m_queue) {
		return false;
	}

	QMutexLocker locker(&m_queue->mutex);

	return (m_queue->uploader != nullptr);
}

template <class DataType_t>
unsigned int DataDownloader<DataType_t>::getNumAvailableData() const
{
	QMutexLocker internalLocker(&m_mutex);

	if (!m_queue) {
		throw UploaderDownloaderAssociationNotPresentException(UploaderDownloaderAssociationNotPresentException::UploaderNotPresent);
	}

	QMutexLocker locker(&m_queue->mutex);

	return m_queue->numDataInQueue;
}

template <class DataType_t>
const DataType_t* DataDownloader<DataType_t>::downloadDatum()
{
	QMutexLocker internalLocker(&m_mutex);

	// Checking if the queue is there, We allow to download even if no uploader is present to
	// be able to remove the last data from the queue
	if (!m_queue) {
		throw UploaderDownloaderAssociationNotPresentException(UploaderDownloaderAssociationNotPresentException::UploaderNotPresent);
	}

	QMutexLocker locker(&(m_queue->mutex));

	// Checking whether data exchange has been stopped
	if (m_queue->dataExchangeStopped) {
		return nullptr;
	}

	// Checking if the queue is empty
	if (m_queue->numDataInQueue == 0) {
		// Checking that uploader and downloader iterators point to the same location (this must
		// always happend if we get here)
		Q_ASSERT(m_queue->nextUploadIt == m_queue->nextDownloadIt);

		// What to do depends on the NewDatumAvailableBehavior
		switch (m_newDatumAvailableBehavior) {
			case NoNotificationBlocking:
				// Waiting on the wait condition, we will be woke up when the uploader has uploaded something
				m_queue->waitCondition.wait(&m_queue->mutex);

				// If we were woken up because data exchange has been stopped, simply returning nullptr
				if (m_queue->dataExchangeStopped) {
					return nullptr;
				}
				break;
			default:
				// In all the other cases we return nullptr to tell that no datum is available
				return nullptr;
				break;
		}
	}

	// We can return the current datum. We have to put back in the queue the datum that was downloaded before and move
	// the iterator forward
	DataType* oldDatum = m_queue->currentDownloaderDatum;
	m_queue->currentDownloaderDatum = *m_queue->nextDownloadIt;
	*m_queue->nextDownloadIt = oldDatum;
	++m_queue->nextDownloadIt;
	if (m_queue->nextDownloadIt == m_queue->queue.end()) {
		m_queue->nextDownloadIt = m_queue->queue.begin();
	}

	// Incrementing the available space and decrementing the number of data in the queue
	++m_queue->availableSpace;
	--m_queue->numDataInQueue;

	// Waking up the uploader, in case it was sleeping
	m_queue->waitCondition.wakeAll();

	return m_queue->currentDownloaderDatum;
}

template <class DataType_t>
void DataDownloader<DataType_t>::sendNotification()
{
	// What we do here depends on the NewDatumAvailableBehavior
	switch (m_newDatumAvailableBehavior) {
		case NoNotification:
		case NoNotificationBlocking:
			// Nothing to do
			break;
		case QtEvent:
			// Posting a QT event
			Q_ASSERT(m_qoject != nullptr);
			QCoreApplication::postEvent(m_qoject, new NewDatumEvent<DataType>(this));
			break;
		case Callback:
			// Calling the callback
			Q_ASSERT(m_newDatumNotifiable != nullptr);
			m_newDatumNotifiable->newDatumAvailable(this);
			break;
	}
}

template <class DataType>
void GlobalUploaderDownloader::attach(DataUploader<DataType>* uploader, DataDownloader<DataType>* downloader)
{
	getInstance().internalAttach(uploader, downloader);
}

template <class DataType1, class DataType2>
void GlobalUploaderDownloader::attach(DataUploaderDownloader<DataType1, DataType2>* first, DataUploaderDownloader<DataType2, DataType1>* second)
{
	getInstance().internalAttach(first, second);
}

template <class DataType>
void GlobalUploaderDownloader::detach(DataUploader<DataType>* uploader)
{
	getInstance().internalDetach(uploader);
}

template <class DataType>
void GlobalUploaderDownloader::detach(DataDownloader<DataType>* downloader)
{
	getInstance().internalDetach(downloader);
}

template <class DataType1, class DataType2>
void GlobalUploaderDownloader::detach(DataUploaderDownloader<DataType1, DataType2>* uploaderDownloader)
{
	getInstance().internalDetach(uploaderDownloader);
}

template <class DataType>
void GlobalUploaderDownloader::internalAttach(DataUploader<DataType>* uploader, DataDownloader<DataType>* downloader)
{
	// Locking order is the same in all functions: our mutex, downloader mutex, queue mutex
	QMutexLocker locker(&m_mutex);

	// Locking the downloader mutex
	QMutexLocker downloaderMutexLocker(&(downloader->m_mutex));

	// Now also locking the queue mutex (we take it from the uploader)
	QMutexLocker queueMutexLocker(&(uploader->m_queue->mutex));

	// First of all checking that neither the uploader nor the downloader is already associated
	if (uploader->m_queue->downloader != nullptr) {
		throw UploaderDownloaderAssociationNotUniqueException(UploaderDownloaderAssociationNotUniqueException::UploaderAlreadyAssociated);
	} else if (downloader->m_queue && (downloader->m_queue->uploader != nullptr)) {
		throw UploaderDownloaderAssociationNotUniqueException(UploaderDownloaderAssociationNotUniqueException::DownloaderAlreadyAssociated);
	}

	// No need to check whether a datum has been created but not uploaded as both ends are not associated!

	// Creating the association. The old queue in the downloader is deleted by QExplicitlySharedDataPointer
	// if present
	uploader->m_queue->downloader = downloader;
	uploader->m_queue->uploader = uploader;
	downloader->m_queue = uploader->m_queue;

	// If there are data available in the queue, we must notify the downloader
	if (uploader->m_queue->numDataInQueue != 0) {
		downloader->sendNotification();
	}
}

template <class DataType1, class DataType2>
void GlobalUploaderDownloader::internalAttach(DataUploaderDownloader<DataType1, DataType2>* first, DataUploaderDownloader<DataType2, DataType1>* second)
{
	// Locking order is the same in all functions: our mutex, downloader mutex, queue mutex
	QMutexLocker locker(&m_mutex);

	// Splitting downloaders and uploaders
	DataUploader<DataType1>* const firstUploader = first;
	DataDownloader<DataType2>* const firstDownloader = first;
	DataUploader<DataType2>* const secondUploader = second;
	DataDownloader<DataType1>* const secondDownloader = second;

	// Taking all locks
	QMutexLocker firstDownloaderMutexLocker(&(firstDownloader->m_mutex));
	QMutexLocker firstQueueMutexLocker(&(firstUploader->m_queue->mutex));
	QMutexLocker secondDownloaderMutexLocker(&(secondDownloader->m_mutex));
	QMutexLocker secondQueueMutexLocker(&(secondUploader->m_queue->mutex));

	// First of all checking that neither uploaders nor downloaders are already associated
	if (firstUploader->m_queue->downloader != nullptr) {
		throw UploaderDownloaderAssociationNotUniqueException(UploaderDownloaderAssociationNotUniqueException::UploaderAlreadyAssociated);
	} else if (firstDownloader->m_queue && (firstDownloader->m_queue->uploader != nullptr)) {
		throw UploaderDownloaderAssociationNotUniqueException(UploaderDownloaderAssociationNotUniqueException::DownloaderAlreadyAssociated);
	} else if (secondUploader->m_queue->downloader != nullptr) {
		throw UploaderDownloaderAssociationNotUniqueException(UploaderDownloaderAssociationNotUniqueException::UploaderAlreadyAssociated);
	} else if (secondDownloader->m_queue && (secondDownloader->m_queue->uploader != nullptr)) {
		throw UploaderDownloaderAssociationNotUniqueException(UploaderDownloaderAssociationNotUniqueException::DownloaderAlreadyAssociated);
	}

	// No need to check whether a datum has been created but not uploaded as both ends are not associated!

	// Creating the association. The old queue in downloaders is deleted by QExplicitlySharedDataPointer
	// if present
	firstUploader->m_queue->downloader = secondDownloader;
	firstUploader->m_queue->uploader = firstUploader;
	secondDownloader->m_queue = firstUploader->m_queue;
	secondUploader->m_queue->downloader = firstDownloader;
	secondUploader->m_queue->uploader = secondUploader;
	firstDownloader->m_queue = secondUploader->m_queue;

	// If there are data available in the queue, we must notify the downloader
	if (firstUploader->m_queue->numDataInQueue != 0) {
		secondDownloader->sendNotification();
	}
	if (secondUploader->m_queue->numDataInQueue != 0) {
		firstDownloader->sendNotification();
	}
}

template <class DataType>
void GlobalUploaderDownloader::internalDetach(DataUploader<DataType>* uploader)
{
	// Locking order is the same in all functions: our mutex, downloader mutex, queue mutex
	// Here we must take the downloader from the queue and we do this without locking. This
	// is fine because the pointer to the downloader in the queue is protected by the mutex
	// in GlobalUploaderDownloader (which is locked first)
	QMutexLocker locker(&m_mutex);

	// If no association is present, returning directly
	DataDownloader<DataType>* const downloader = uploader->m_queue->downloader;
	if (downloader == nullptr) {
		return;
	}

	// Locking the downloader mutex
	QMutexLocker downloaderMutexLocker(&(downloader->m_mutex));

	// Now also locking the queue mutex (we take it from the uploader)
	QMutexLocker queueMutexLocker(&(uploader->m_queue->mutex));

	// Removing the association
	uploader->m_queue->downloader = nullptr;

	// Unlocking the lock on the queue because here it could be destroyed and if it
	// isn't, the lock is not necessary
	queueMutexLocker.unlock();
	downloader->m_queue.reset();
}

template <class DataType>
void GlobalUploaderDownloader::internalDetach(DataDownloader<DataType>* downloader)
{
	// Locking order is the same in all functions: our mutex, downloader mutex, queue mutex
	QMutexLocker locker(&m_mutex);

	// Locking the downloader mutex
	QMutexLocker downloaderMutexLocker(&(downloader->m_mutex));

	// If no association is present, returning directly
	if (!downloader->m_queue) {
		return;
	}

	// Now also locking the queue mutex (we take it from the downloader)
	QMutexLocker queueMutexLocker(&(downloader->m_queue->mutex));

	// Removing the association
	downloader->m_queue->downloader = nullptr;

	// Unlocking the lock on the queue because here it could be destroyed and if it
	// isn't, the lock is not necessary
	queueMutexLocker.unlock();
	downloader->m_queue.reset();
}

template <class DataType1, class DataType2>
void GlobalUploaderDownloader::internalDetach(DataUploaderDownloader<DataType1, DataType2>* uploaderDownloader)
{
	// Locking order is the same in all functions: our mutex, downloader mutex, queue mutex
	// Here we must take the downloader associated with the uploader in uploaderDownloader from
	// the queue and we do this without locking. This is fine because the pointer to the downloader
	// in the queue is protected by the mutex in GlobalUploaderDownloader (which is locked first)
	QMutexLocker locker(&m_mutex);

	// Splitting downloader and uploader
	DataUploader<DataType1>* const uploader = uploaderDownloader;
	DataDownloader<DataType2>* const downloader = uploaderDownloader;

	// Taking all locks. To be able to use RAII with the mutex of the downloader associated with
	// the uploader of uploaderDownloader and with the queue associated with the downloader of
	// uploaderDownloader (which may not exist), we use an std::unique_ptr
	std::unique_ptr<QMutexLocker> otherDownloaderMutexLocker;
	DataDownloader<DataType1>* const otherDownloader = uploader->m_queue->downloader;
	if (otherDownloader != nullptr) {
		otherDownloaderMutexLocker.reset(new QMutexLocker(&(otherDownloader->m_mutex)));
	}
	QMutexLocker queueMutexLocker(&(uploader->m_queue->mutex));
	QMutexLocker downloaderMutexLocker(&(downloader->m_mutex));
	std::unique_ptr<QMutexLocker> otherQueueMutexLocker;
	if (downloader->m_queue) {
		otherQueueMutexLocker.reset(new QMutexLocker(&(downloader->m_queue->mutex)));
	}

	// Removing all associations
	uploader->m_queue->downloader = nullptr;
	if (otherDownloader != nullptr) {
		// Unlocking the lock on the queue because here it could be destroyed and if it
		// isn't, the lock is not necessary
		otherDownloaderMutexLocker->unlock();
		otherDownloader->m_queue.reset();
	}
	if (downloader->m_queue) {
		downloader->m_queue->downloader = nullptr;

		// Unlocking the lock on the queue because here it could be destroyed and if it
		// isn't, the lock is not necessary
		otherQueueMutexLocker->unlock();
		downloader->m_queue.reset();
	}
}

template <class DataType>
void GlobalUploaderDownloader::uploaderDestroyed(DataUploader<DataType>* uploader)
{
	// Locking order is the same in all functions: our mutex, downloader mutex, queue mutex
	// Here we must take the downloader from the queue and we do this without locking. This
	// is fine because the pointer to the downloader in the queue is protected by the mutex
	// in GlobalUploaderDownloader (which is locked first)
	QMutexLocker locker(&m_mutex);

	// If no association is present, returning directly
	DataDownloader<DataType>* const downloader = uploader->m_queue->downloader;
	if (downloader == nullptr) {
		return;
	}

	// Locking the downloader mutex
	QMutexLocker downloaderMutexLocker(&(downloader->m_mutex));

	// Now also locking the queue mutex (we take it from the uploader)
	QMutexLocker queueMutexLocker(&(uploader->m_queue->mutex));

	// Removing the association
	uploader->m_queue->uploader = nullptr;

	// Unlocking the lock on the queue because here it could be destroyed and if it
	// isn't, the lock is not necessary
	queueMutexLocker.unlock();
	uploader->m_queue.reset();
}

template <class DataType>
void GlobalUploaderDownloader::downloaderDestroyed(DataDownloader<DataType>* downloader)
{
	// Here we can simply call detach on the downloader
	internalDetach(downloader);
}

} // end namespace salsa

#endif
