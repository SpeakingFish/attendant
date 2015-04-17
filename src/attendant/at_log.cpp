#include "at_log.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QStringList>


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

void at_log_t::default_log_handler(at_log_level_t level, const QByteArray& category, const QString& text)
{
	static std::unique_ptr<QString> file_name;
	if (file_name.get() == NULL)
	{
		file_name.reset(new QString(QCoreApplication::arguments()[0] + ".log"));
	}
	QFile file(*file_name);
	if (!file.open(QIODevice::Append))
	{
		return;
	}

	switch (level) {
	case at_log_level_debug:
		file.write(QString("%2 [%3] Debug: %1\n").arg(text).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(QString::fromAscii(category)).toUtf8());
		break;
	case at_log_level_info:
		file.write(QString("%2 [%3] Info: %1\n").arg(text).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(QString::fromAscii(category)).toUtf8());
		break;
	case at_log_level_warning:
		file.write(QString("%2 [%3] Warning: %1\n").arg(text).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(QString::fromAscii(category)).toUtf8());
		break;
	case at_log_level_message:
		file.write(QString("%2 [%3] Message: %1\n").arg(text).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(QString::fromAscii(category)).toUtf8());
		break;
	case at_log_level_critical:
		file.write(QString("%2 [%3] Critical: %1\n").arg(text).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(QString::fromAscii(category)).toUtf8());
		break;
	case at_log_level_fatal:
		file.write(QString("%2 [%3] Fatal: %1\n").arg(text).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(QString::fromAscii(category)).toUtf8());
		file.close(); // закрываем файл
		exit(42);
	}
	file.close(); // закрываем файл
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
