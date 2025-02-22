#ifndef slic3r_GCodeSender_hpp_
#define slic3r_GCodeSender_hpp_
#ifdef BOOST_LIBS

#include "libslic3r.h"
#include <queue>
#include <string>
#include <vector>
#include <boost/asio.hpp>

#include <boost/version.hpp>
#if BOOST_VERSION >= 107300
#include <boost/bind/bind.hpp>
#else
#include <boost/bind.hpp>
#endif
#include <boost/thread.hpp>
#include <boost/core/noncopyable.hpp>

namespace Slic3r {

namespace asio = boost::asio;

#if BOOST_VERSION >= 107300
using boost::placeholders::_1;
using boost::placeholders::_2;
#endif

class GCodeSender : private boost::noncopyable {
    public:
    GCodeSender();
    ~GCodeSender();
    bool connect(std::string devname, unsigned int baud_rate);
    void send(const std::vector<std::string> &lines, bool priority = false);
    void send(const std::string &s, bool priority = false);
    void disconnect();
    bool error_status() const;
    bool is_connected() const;
    bool wait_connected(unsigned int timeout = 3) const;
    size_t queue_size() const;
    void pause_queue();
    void resume_queue();
    void purge_queue(bool priority = false);
    std::vector<std::string> purge_log();
    std::string getT() const;
    std::string getB() const;
    void set_DTR(bool on);
    void reset();

    private:
    asio::io_service io;
    asio::serial_port serial;
    boost::thread background_thread;
    boost::asio::streambuf read_buffer, write_buffer;
    bool open;      // whether the serial socket is connected
    bool connected; // whether the printer is online
    bool error;
    mutable boost::mutex error_mutex;

    // this mutex guards queue, priqueue, can_send, queue_paused, sent, last_sent
    mutable boost::mutex queue_mutex;
    std::queue<std::string> queue;
    std::list<std::string> priqueue;
    bool can_send;
    bool queue_paused;
    size_t sent;
    std::vector<std::string> last_sent;

    // this mutex guards log, T, B
    mutable boost::mutex log_mutex;
    std::queue<std::string> log;
    std::string T, B;

    void set_baud_rate(unsigned int baud_rate);
    void set_error_status(bool e);
    void do_send();
    void on_write(const boost::system::error_code& error, size_t bytes_transferred);
    void do_close();
    void do_read();
    void on_read(const boost::system::error_code& error, size_t bytes_transferred);
    void send();
};

}

#endif
#endif
