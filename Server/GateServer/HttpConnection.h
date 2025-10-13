#pragma once

#include "const.h"

class HttpConnection:public std::enable_shared_from_this<HttpConnection>
{
public:
	friend class LogicSystem;
	HttpConnection(boost::asio::io_context& ioc);
	void Start();
	tcp::socket& GetSocket();
private:
	void CheckDeadline(); // 超时检测
	void WriteResponse(); // 应答
	void HandleReq();
	void PreParseGetParam();

	tcp::socket _socket;
	beast::flat_buffer _buffer{ 8192 };
	http::request<http::dynamic_body> _request; // 允许任意类型的请求，由http封装好的类
	http::response<http::dynamic_body> _response;
	net::steady_timer deadline_{ // 大括号形式的初始化列表，C++11引入，防止部分时候小括号初始化造成的歧义
		_socket.get_executor(),
		std::chrono::seconds(60)
	};

	std::string _get_url;
	std::unordered_map<std::string, std::string> _get_params;
};

