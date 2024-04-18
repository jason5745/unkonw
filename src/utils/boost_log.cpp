

#include <boost/log/sources/logger.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>

#include "boost_log.h"

logger& logger::instance() {
    static logger log;
    return log;
}
bool logger::configure(std::string name, boost::log::trivial::severity_level level, int maxFileSize) {
    boost::log::formatter formatter =
        boost::log::expressions::stream
        << boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp","%Y-%m-%d %H:%M:%S.%f") /*.%f*/
        << " ["<< boost::log::expressions::attr<boost::log::attributes::current_thread_id::value_type>("ThreadID") << "]"
        << " ["<< boost::log::expressions::attr<boost::log::trivial::severity_level>("Severity") << "] - "
        << boost::log::expressions::smessage;


    boost::shared_ptr<logger::file_sink> fileSink(new logger::file_sink(
        boost::log::keywords::target_file_name = name + ".%Y-%m-%d-%H.%N.log",   // file name pattern
        boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(16, 0, 0),
        boost::log::keywords::rotation_size = maxFileSize * 1024 * 1024,                                        
        boost::log::keywords::open_mode = std::ios::out | std::ios::app
    ));

    fileSink->locked_backend()->set_file_collector(boost::log::sinks::file::make_collector(
        boost::log::keywords::target = "logs",                                          //folder name.
        boost::log::keywords::max_size = maxFileSize * 512 * 1024 * 1024,               //The maximum amount of space of the folder.
        boost::log::keywords::min_free_space = 10 * 1024 * 1024,                        //Reserved disk space minimum.
        boost::log::keywords::max_files = 512
    ));

    fileSink->set_formatter(formatter);
    fileSink->locked_backend()->scan_for_files();
    fileSink->locked_backend()->auto_flush(true);

    auto consoleSink = boost::log::add_console_log();
    consoleSink->set_formatter(formatter);

    boost::log::core::get()->add_sink(fileSink);
    boost::log::core::get()->add_sink(consoleSink);

    boost::log::add_common_attributes();
    boost::log::core::get()->set_filter(
        boost::log::trivial::severity >= level
    );
    return true;
}
bool logger::configure(boost::log::trivial::severity_level level, int maxFileSize) {
    boost::log::formatter formatter = 
        boost::log::expressions::stream
        << boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp","%Y-%m-%d %H:%M:%S.%f") /*.%f*/
        << " ["<< boost::log::expressions::attr<boost::log::attributes::current_process_id::value_type>("ThreadID") << "]"
        << " ["<< boost::log::expressions::attr<boost::log::trivial::severity_level>("Severity") << "] - "
        << boost::log::expressions::smessage;

    auto consoleSink = boost::log::add_console_log();
    consoleSink->set_formatter(formatter);
    boost::log::core::get()->add_sink(consoleSink);
    boost::log::add_common_attributes();
    boost::log::core::get()->set_filter(
        boost::log::trivial::severity >= level
    );
    return true;
}