#pragma once

#include <memory>

#include <QtCore/QtDebug>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QTextStream>

#include "at_common.h"

// !!!!!!!!!!!! IMPORTANT
// for correct initialization you must call AT_LOG  straightly after create QApplication object

#if !defined(AT_LOG)

	#define AT_LOG_STR__(s) (#s)
	#define AT_LOG_NOOP__ if (false) qDebug()

	#if defined AT_LOG_DISABLED
		#define AT_LOG          AT_LOG_NOOP__
		#define AT_LOG_DEBUG    AT_LOG_NOOP__
		#define AT_LOG_INFO     AT_LOG_NOOP__
		#define AT_LOG_EVENT    AT_LOG_NOOP__
		#define AT_LOG_WARNING  AT_LOG_NOOP__
		#define AT_LOG_MESSAGE  AT_LOG_NOOP__
		#define AT_LOG_CRITICAL AT_LOG_NOOP__
		#define AT_LOG_FATAL    AT_LOG_NOOP__

		#define AT_LOG_FUNC          AT_LOG_NOOP__
		#define AT_LOG_FUNC_DEBUG    AT_LOG_NOOP__
		#define AT_LOG_FUNC_INFO     AT_LOG_NOOP__
		#define AT_LOG_FUNC_EVENT    AT_LOG_NOOP__
		#define AT_LOG_FUNC_WARNING  AT_LOG_NOOP__
		#define AT_LOG_FUNC_MESSAGE  AT_LOG_NOOP__
		#define AT_LOG_FUNC_CRITICAL AT_LOG_NOOP__
		#define AT_LOG_FUNC_FATAL    AT_LOG_NOOP__
	#else
		#define AT_LOG_CATEGORY__(category, level) (at_log_stream_t(level, AT_LOG_STR__(category)))

		#ifndef AT_LOG_LEVEL_DEFAULT
			#define AT_LOG_LEVEL_DEFAULT at_log_level_info
		#endif

		#ifndef AT_LOG_CATEGORY
			#define AT_LOG_CATEGORY Log.General
		#endif

		#define AT_LOG          AT_LOG_CATEGORY__(AT_LOG_CATEGORY, AT_LOG_LEVEL_DEFAULT)
		#define AT_LOG_DEBUG    AT_LOG_CATEGORY__(AT_LOG_CATEGORY, at_log_level_debug)
		#define AT_LOG_INFO     AT_LOG_CATEGORY__(AT_LOG_CATEGORY, at_log_level_info)
		#define AT_LOG_EVENT    AT_LOG_CATEGORY__(AT_LOG_CATEGORY, at_log_level_event)
		#define AT_LOG_WARNING  AT_LOG_CATEGORY__(AT_LOG_CATEGORY, at_log_level_warning)
		#define AT_LOG_MESSAGE  AT_LOG_CATEGORY__(AT_LOG_CATEGORY, at_log_level_message)
		#define AT_LOG_CRITICAL AT_LOG_CATEGORY__(AT_LOG_CATEGORY, at_log_level_critical)
		#define AT_LOG_FATAL    AT_LOG_CATEGORY__(AT_LOG_CATEGORY, at_log_level_fatal)

		#define AT_LOG_FUNC          (AT_LOG          << __PRETTY_FUNCTION__)
		#define AT_LOG_FUNC_DEBUG    (AT_LOG_DEBUG    << __PRETTY_FUNCTION__)
		#define AT_LOG_FUNC_INFO     (AT_LOG_INFO     << __PRETTY_FUNCTION__)
		#define AT_LOG_FUNC_EVENT    (AT_LOG_EVENT    << __PRETTY_FUNCTION__)
		#define AT_LOG_FUNC_WARNING  (AT_LOG_WARNING  << __PRETTY_FUNCTION__)
		#define AT_LOG_FUNC_MESSAGE  (AT_LOG_MESSAGE  << __PRETTY_FUNCTION__)
		#define AT_LOG_FUNC_CRITICAL (AT_LOG_CRITICAL << __PRETTY_FUNCTION__)
		#define AT_LOG_FUNC_FATAL    (AT_LOG_FATAL    << __PRETTY_FUNCTION__)
	#endif


	enum at_log_level_t
	{
		at_log_level_debug    = 1,
		at_log_level_info     = 2,
		at_log_level_event    = 3,
		at_log_level_warning  = 4,
		at_log_level_message  = 5,
		at_log_level_critical = 6,
		at_log_level_fatal    = 7,
	};

	class at_log_stream_t;

	typedef void (at_log_handler_t)(at_log_level_t level, const QByteArray& category, const QString& text);

	class at_log_stream_t
	{
	public:
		at_log_stream_t(at_log_level_t level, const QByteArray& category);
		~at_log_stream_t();

		at_log_stream_t& operator<<(const QString& str);
		at_log_stream_t& operator<<(const QStringRef& str);
		at_log_stream_t& operator<<(const QByteArray& str);

		template <typename T> at_log_stream_t& operator<<(const T& t)
		{
			*stream_ << t;
			return *this;
		}
	private:
		at_log_level_t level_;
		QByteArray category_;
		std::unique_ptr<QDebug> stream_;
		QString buffer_;
	};

	class at_log_t
	{
	public:
		~at_log_t();

		static at_log_handler_t* log_handler();

		static void set_log_handler(at_log_handler_t*);

		static void default_log_handler(at_log_level_t level, const QByteArray& category, const QString& text);

	private:
		at_log_t();

	private:
		static std::unique_ptr<at_log_t> instance_;
		static at_log_handler_t* log_handler_;
	};

	class at_log_buffer_t
	{
	public:
		static void input_handler(at_log_level_t level, const QByteArray& category, const QString& text);
		static void set_output_handler(at_log_handler_t*);
	private:
		static at_log_handler_t* output_handler_;

		struct storage_item_t
		{
			at_log_level_t level;
			QByteArray category;
			QString text;
		};

		static QList<storage_item_t> temp_storage_;
	};

#else
	#error AT_LOG macro redifinition
#endif
