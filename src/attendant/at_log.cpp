#include "at_log.h"

#include <QtCore/QAtomicInt>
#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QThreadPool>

#include "qgzip.h"

#define MAX_LOG_FILE_SIZE 2*1024*1024

// enum QtMsgType { QtDebugMsg, QtWarningMsg, QtCriticalMsg, QtFatalMsg, QtSystemMsg = QtCriticalMsg };

void at_message_output(QtMsgType type, const char *msg)
{
	at_log_level_t level = at_log_level_debug;
	switch(type)
	{
		case QtDebugMsg:
			level = at_log_level_debug;
			break;
		case QtWarningMsg:
			level = at_log_level_warning;
			break;
		case QtCriticalMsg:
			level = at_log_level_critical;
			break;
		case QtFatalMsg:
			level = at_log_level_critical;
			break;
		default:
			level = at_log_level_debug;
	}
	at_log_stream_t(level, "qDebug") << msg;
}

std::unique_ptr<at_log_t> at_log_t::instance_;
at_log_handler_t* at_log_t::log_handler_ = NULL;

at_log_t::at_log_t()
{
	qInstallMsgHandler(at_message_output);
}

at_log_t::~at_log_t()
{

}

at_log_handler_t* at_log_t::log_handler()
{
	if (instance_ == NULL)
	{
		instance_.reset(new at_log_t());
	}

	if (log_handler_ == NULL)
	{
		log_handler_ = &default_log_handler;
	}

	return log_handler_;
}

void at_log_t::set_log_handler(at_log_handler_t* handler)
{
	log_handler_ = handler;
}

class log_zipper_t : public QRunnable
{
public:
	log_zipper_t(const QString& file_name)
		: file_name_(file_name)
	{

	}

	virtual void run()
    {
		const QString zip_file_name = file_name_ + ".gz";
		QFile infile(file_name_);
		QFile outfile(zip_file_name);
		infile.open(QIODevice::ReadOnly);
		outfile.open(QIODevice::WriteOnly);
		const QByteArray uncompressed_data = infile.readAll();
		QByteArray compressed_data;
		qgzip_t::compress(uncompressed_data, compressed_data, 9);
		outfile.write(compressed_data);
		infile.close();
		outfile.close();
		QDir().remove(file_name_);
	}

	QString file_name_;
};

class recursion_guard_t
{
public:
	recursion_guard_t(QAtomicInt& recursion_level)
		: _recursion_level(recursion_level)
	{
		_recursion_level.fetchAndAddAcquire(1);
	}
	~recursion_guard_t()
	{
		_recursion_level.fetchAndAddAcquire(-1);
	}

	int level() const
	{
		return _recursion_level;
	}
private:
	QAtomicInt& _recursion_level;
};

void at_log_t::default_log_handler(at_log_level_t level, const QByteArray& category, const QString& text)
{
	static std::unique_ptr<QString> file_name;
	static QAtomicInt recursion_level = 0;
	recursion_guard_t guard = recursion_guard_t(recursion_level);

	if (guard.level() > 2)
	{
		return;
	}

	if (file_name.get() == NULL)
	{
		static QString log_path = QCoreApplication::instance()->property("log_path").toString();
		if (log_path.isEmpty())
		{
			log_path = QFileInfo(QCoreApplication::instance()->arguments().first()).absoluteDir().absolutePath() + "/../log";
		}
		static QString log_name = QCoreApplication::instance()->property("log_name").toString();
		if (log_name.isEmpty())
		{
			log_name = QCoreApplication::instance()->applicationName();
		}

		if (!QDir().exists(log_path))
		{
			QDir().mkpath(log_path);
		}
		file_name.reset(new QString(log_path + "/" + log_name + ".log"));
	}
	std::unique_ptr<QFile> file(new QFile(*file_name));
	if (file->size() > MAX_LOG_FILE_SIZE)
	{
		const QString archived_file_name = *file_name + "." + QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss");
		file->rename(archived_file_name);
		file.reset(new QFile(*file_name));

		log_zipper_t* log_zipper = new log_zipper_t(archived_file_name);
		QThreadPool::globalInstance()->start(log_zipper);
	}
	if (!file->open(QIODevice::Append))
	{
		return;
	}

	QFile* file_ptr = file.get();
	auto file_writer = [file_ptr, &text, &category] (const QString& mask)
	{
		file_ptr->write(QString(mask).arg(text).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(QString::fromAscii(category)).toAscii());
	};

	switch (level) {
	case at_log_level_debug:
		file_writer("%2 [%3] Debug: %1\n");
		break;
	case at_log_level_info:
		file_writer("%2 [%3] Info: %1\n");
		break;
	case at_log_level_event:
		file_writer("%2 [%3] Event: %1\n");
		break;
	case at_log_level_warning:
		file_writer("%2 [%3] Warning: %1\n");
		break;
	case at_log_level_message:
		file_writer("%2 [%3] Message: %1\n");
		break;
	case at_log_level_critical:
		file_writer("%2 [%3] Critical: %1\n");
		break;
	case at_log_level_fatal:
		file_writer("%2 [%3] Fatal: %1\n");
		file->close(); // закрываем файл
		exit(42);
	}
	file->close(); // закрываем файл
}

at_log_stream_t::at_log_stream_t(at_log_level_t level, const QByteArray& category)
	: level_(level)
	, category_(category)
{
	stream_.reset(new QDebug(&buffer_));
	stream_->nospace();
}

at_log_stream_t::~at_log_stream_t()
{
	stream_.reset();
	at_log_t::log_handler()(level_, category_, buffer_);
}

at_log_handler_t* at_log_buffer_t::output_handler_ = NULL;
QList<at_log_buffer_t::storage_item_t> at_log_buffer_t::temp_storage_;

void at_log_buffer_t::input_handler(at_log_level_t level, const QByteArray& category, const QString& text)
{
	if (output_handler_ != NULL)
	{
		output_handler_(level, category, text);
		return;
	}
	temp_storage_.append(storage_item_t( {level, category, text} ));
}

void at_log_buffer_t::set_output_handler(at_log_handler_t* handler)
{
	output_handler_ = handler;
	Q_FOREACH(storage_item_t item, temp_storage_)
	{
		output_handler_(item.level, item.category, item.text);
	}
	temp_storage_.clear();
}


at_log_stream_t& at_log_stream_t::operator<<(const QString& str)
{
	if (QCoreApplication::instance() != NULL)
	{
		*stream_ << str.toLocal8Bit().data();
	}
	else
	{
		*stream_ << str.toLatin1().data();
	}
	return *this;
}

at_log_stream_t& at_log_stream_t::operator<<(const QStringRef& str)
{
	if (QCoreApplication::instance() != NULL)
	{
		*stream_ << str.toLocal8Bit().data();
	}
	else
	{
		*stream_ << str.toLatin1().data();
	}
	return *this;
}

at_log_stream_t& at_log_stream_t::operator<<(const QByteArray& str)
{
	*stream_ << str.data();
	return *this;
}
