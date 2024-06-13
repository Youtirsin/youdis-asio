#include <boost/asio.hpp>
#include <iterator>
#include <memory>

#include "parser.hpp"
#include "socket_reader.hpp"
#include "value.hpp"
#include "handler.hpp"

using boost::asio::ip::tcp;

namespace youdis {

class Connection : public std::enable_shared_from_this<Connection> {
 public:
  Connection(tcp::socket socket, boost::asio::io_context& io_context)
      : socket_(std::move(socket)),
        io_context_(io_context),
        reader_(socket_),
        request_(nullptr) {}

  ~Connection() {
    if (request_) {
      delete request_;
    }
  }

  void start() { read_request(); }

 private:
  void read_request() {
    auto self(shared_from_this());
    io_context_.post([this, self]() {
      try {
        Parser parser(reader_);
        request_ = parser.read();
        handle_request();
      } catch (const std::exception& e) {
        std::unique_ptr<Value> response(new ErrorValue("invalid request."));
        write_response(response->marshal());
      }
    });
  }

  void handle_request() {
    auto self(shared_from_this());
    io_context_.post([this, self]() {
      auto request = getInType<ArrayValue>(request_);
      if (!request || request->arr.empty()) {
        std::cout << "invalid request, expected array." << std::endl;
        return;
      }

      auto command = getInType<BulkValue>(request->arr.front());
      if (!command) {
        std::cout << "invalid request, expected bulk value for command."
                  << std::endl;
        return;
      }

      std::vector<Value*> args;
      std::copy(request->arr.begin() + 1, request->arr.end(),
                std::back_inserter(args));

      try {
        std::unique_ptr<Value> response(handlers(command->bulk)(args));
        write_response(response->marshal());
      } catch (const std::exception& e) {
        std::unique_ptr<Value> response(new ErrorValue("invalid command."));
        write_response(response->marshal());
      }
    });
  }

  void write_response(std::string&& response) {
    auto self(shared_from_this());
    socket_.async_write_some(
        boost::asio::buffer(response),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec) {
            read_request();
          }
        });
  }

  tcp::socket socket_;
  boost::asio::io_context& io_context_;
  SocketReader reader_;
  Value* request_;
};

class Server {
 public:
  Server(boost::asio::io_context& io_context, short port)
      : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
        io_context_(io_context) {
    do_accept();
  }

 private:
  void do_accept() {
    acceptor_.async_accept([this](boost::system::error_code ec,
                                  tcp::socket socket) {
      if (!ec) {
        std::make_shared<Connection>(std::move(socket), io_context_)->start();
      }
      do_accept();
    });
  }

  tcp::acceptor acceptor_;
  boost::asio::io_context& io_context_;
};

}  // namespace youdis
