#ifndef __BOOST_LOGGER_H_
#define __BOOST_LOGGER_H_


#include <string>

#include <boost/log/common.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/trivial.hpp>

#define log_trace(message) BOOST_LOG_SEV(logger::instance()._logger,boost::log::trivial::trace) << __FILE__ << ":" << __LINE__ << " " << message
#define log_debug(message) BOOST_LOG_SEV(logger::instance()._logger,boost::log::trivial::debug) << __FILE__ << ":" << __LINE__ << " " << message
#define log_info(message)  BOOST_LOG_SEV(logger::instance()._logger,boost::log::trivial::info) << __FILE__ << ":" << __LINE__ << " " << message
#define log_warn(message)  BOOST_LOG_SEV(logger::instance()._logger,boost::log::trivial::warning) << __FILE__ << ":" << __LINE__ << " " << message
#define log_error(message) BOOST_LOG_SEV(logger::instance()._logger,boost::log::trivial::error) << __FILE__ << ":" << __LINE__ << " " << message
#define log_fatal(message) BOOST_LOG_SEV(logger::instance()._logger,boost::log::trivial::fatal) << __FILE__ << ":" << __LINE__ << " " << message

class logger {
public:
    typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> file_sink;
    enum loggerType {
        console = 0,
        file,
    };
    logger() {}
    ~logger() {}

    boost::log::sources::severity_logger<boost::log::trivial::severity_level> _logger;
    static logger& instance();
    static bool configure(boost::log::trivial::severity_level level, int maxFileSize);
    static bool configure(std::string name, boost::log::trivial::severity_level level, int maxFileSize);
};

#endif