//
// Created by paun on 9/25/26.
//

#include "amanet_client.h"

#include <iostream>
#include <boost/beast/version.hpp>

static void fail(const beast::error_code& ec, char const* what) {
	std::cerr << what << ": " << ec.message() << "\n";
}

amanet_client::amanet_client(asio::io_context& io_ctx) :
	resolver_(asio::make_strand(io_ctx)),
	stream_(asio::make_strand(io_ctx)) {
}

void amanet_client::run(
		char const* host,
		char const* port,
		char const* target,
		const int version,
		const http::verb method,
		std::string body,
		const std::string& content_type) {
	request_.version(version);
	request_.method(method);
	request_.target(target);

	request_.set(http::field::host, host);
	request_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

	if (!content_type.empty()) {
		request_.set(http::field::content_type, content_type);
	}

	request_.body() = std::move(body);
	request_.prepare_payload();

	resolver_.async_resolve(
			host,
			port,
			beast::bind_front_handler(
					&amanet_client::on_resolve,
					shared_from_this()));
}


void amanet_client::on_resolve(
		const beast::error_code& ec,
		const tcp::resolver::results_type& results) {
	if (ec) {
		return fail(ec, "resolve failed");
	}

	stream_.expires_after(std::chrono::seconds(30));

	stream_.async_connect(
			results,
			beast::bind_front_handler(
					&amanet_client::on_connect,
					shared_from_this()));
}

void amanet_client::on_connect(
		const beast::error_code& ec,
		const tcp::resolver::results_type::endpoint_type&) {
	if (ec) {
		return fail(ec, "connect");
	}

	stream_.expires_after(std::chrono::seconds(30));

	http::async_write(
			stream_,
			request_,
			beast::bind_front_handler(
					&amanet_client::on_write,
					shared_from_this()));
}

void amanet_client::on_write(
		const beast::error_code& ec,
		std::size_t bytes_transferred) {
	boost::ignore_unused(bytes_transferred);

	if (ec) {
		return fail(ec, "write");
	}

	http::async_read(
			stream_,
			buffer_,
			response_,
			beast::bind_front_handler(
					&amanet_client::on_read,
					shared_from_this()));
}

void amanet_client::on_read(
		const beast::error_code& ec,
		std::size_t bytes_transferred) {
	boost::ignore_unused(bytes_transferred);

	if (ec) {
		return fail(ec, "read");
	}

	std::cout << "response: " << response_ << std::endl;

	beast::error_code shutdown_ec;
	stream_.socket().shutdown(tcp::socket::shutdown_both, shutdown_ec);

	if (shutdown_ec && shutdown_ec != beast::errc::not_connected) {
		return fail(shutdown_ec, "shutdown");
	}
}

