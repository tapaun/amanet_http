//
// Created by paun on 9/25/26.
//

#ifndef AMANET_AMANET_CLIENT_H
#define AMANET_AMANET_CLIENT_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/strand.hpp>
#include <cstdlib>
#include <memory>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class amanet_client : public std::enable_shared_from_this<amanet_client> {
	tcp::resolver resolver_;
	beast::tcp_stream stream_;
	beast::flat_buffer buffer_;
	http::request<http::string_body> request_;
	http::response<http::string_body> response_;

public:
	// Objects are constructed with a strand to
	// ensure that handlers do not execute concurrently.
	explicit amanet_client(asio::io_context& io_ctx);

	// Starts one asynchronous HTTP request.
	//
	// Prepares the request and begins hostname resolution.
	// The network operations follow the next order:
	// on_resolve -> on_connect -> on_write -> on_read
	//
	// body and content_type are optional because methods such as
	// GET may not need a request body.
	void run(char const* host,
	         char const* port,
	         char const* target,
	         int version,
	         http::verb method,
	         std::string body                = {},
	         const std::string& content_type = {});

	// Invoked when the asynchronous hostname resolution completes.
	//
	// If the resolution succeeds, starts an asynchronous TCP
	// connection using the resolved endpoints.
	void on_resolve(const beast::error_code& ec,
	                const tcp::resolver::results_type& results);

	// Invoked when the asynchronous TCP connection completes.
	//
	// If the connection succeeds, starts writing the HTTP request.
	void on_connect(const beast::error_code& ec,
	                const tcp::resolver::results_type::endpoint_type&);

	// Invoked when the complete HTTP request has been
	// written to the TCP stream.
	//
	// If the writing succeeds, starts reading the HTTP response.
	void on_write(const beast::error_code& ec,
	              std::size_t bytes_transferred);

	// Called when the complete HTTP has been read and parsed.
	//
	// If the reading succeeds, processes or displays the response
	// and closes the TCP connection.
	void on_read(const beast::error_code& ec,
	             std::size_t bytes_transferred);
};


#endif //AMANET_AMANET_CLIENT_H