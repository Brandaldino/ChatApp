#include "HttpConnection.h"
#include "LogicSystem.h"

HttpConnection::HttpConnection(boost::asio::io_context& ioc): _socket(ioc){
}
void HttpConnection::Start() {
	auto self = shared_from_this();
	http::async_read(
		_socket,
		_buffer,
		_request,
		[self](beast::error_code ec, std::size_t bytes_transfered) {
			try {
				if (ec) {
					std::cerr << "http read err is " << ec.what() << std::endl;
					return;  // ��������ֱ���˳�
				}

				boost::ignore_unused(bytes_transfered);
				self->HandleReq();
				self->CheckDeadline();
			}
			catch (std::exception& e) {
				std::cerr << "exception is " << e.what() << std::endl;
			}
		}
	);
}
tcp::socket& HttpConnection::GetSocket()
{
	return _socket;
}
// ʮ����תʮ������
std::string ToHex(unsigned char decimal) {
	std::ostringstream oss;
	oss << std::hex << decimal; 
	return oss.str();
}

// ʮ������תʮ����
unsigned char FromHex(unsigned char hexStr) {
	std::istringstream iss(hexStr);
	unsigned char decimal;
	iss >> std::hex >> decimal;
	return decimal;
}
// url����
std::string UrlEncode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		//�ж��Ƿ�������ֺ���ĸ����
		if (isalnum((unsigned char)str[i]) ||
			(str[i] == '-') ||
			(str[i] == '_') ||
			(str[i] == '.') ||
			(str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ') //Ϊ���ַ�
			strTemp += "+";
		else
		{
			//�����ַ���Ҫ��ǰ��%���Ҹ���λ�͵���λ�ֱ�תΪ16����
			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] & 0x0F);
		}
	}
	return strTemp;
}
// url����
std::string UrlDecode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		//��ԭ+Ϊ��
		if (str[i] == '+') strTemp += ' ';
		//����%������������ַ���16����תΪchar��ƴ��
		else if (str[i] == '%')
		{
			assert(i + 2 < length);
			unsigned char high = FromHex((unsigned char)str[++i]);
			unsigned char low = FromHex((unsigned char)str[++i]);
			strTemp += high * 16 + low;
		}
		else strTemp += str[i];
	}
	return strTemp;
}
// url����
void HttpConnection::PreParseGetParam() {
	// ��ȡ URI  
	auto uri = _request.target();
	// ���Ҳ�ѯ�ַ����Ŀ�ʼλ�ã��� '?' ��λ�ã�  
	auto query_pos = uri.find('?');
	if (query_pos == std::string::npos) {
		_get_url = uri;
		return;
	}

	_get_url = uri.substr(0, query_pos);
	std::string query_string = uri.substr(query_pos + 1);
	std::string key;
	std::string value;
	size_t pos = 0;
	while ((pos = query_string.find('&')) != std::string::npos) {
		auto pair = query_string.substr(0, pos);
		size_t eq_pos = pair.find('=');
		if (eq_pos != std::string::npos) {
			key = UrlDecode(pair.substr(0, eq_pos)); // ������ url_decode ����������URL����  
			value = UrlDecode(pair.substr(eq_pos + 1));
			_get_params[key] = value;
		}
		query_string.erase(0, pos + 1);
	}
	// �������һ�������ԣ����û�� & �ָ�����  
	if (!query_string.empty()) {
		size_t eq_pos = query_string.find('=');
		if (eq_pos != std::string::npos) {
			key = UrlDecode(query_string.substr(0, eq_pos));
			value = UrlDecode(query_string.substr(eq_pos + 1));
			_get_params[key] = value;
		}
	}
}
void HttpConnection::HandleReq() {
	// ���ð汾
	_response.version(_request.version());
	_response.keep_alive(false);
	if (_request.method() == http::verb::get) {
		PreParseGetParam();
		bool success = LogicSystem::GetInstance()->HandleGet(_get_url, shared_from_this());
		if (!success) {
			_response.result(http::status::not_found);
			_response.set(http::field::content_type, "test/plain");
			beast::ostream(_response.body()) << "url not found\r\n";
			WriteResponse();
			return;
		}
		_response.result(http::status::ok);
		_response.set(http::field::server, "GateServer");
		WriteResponse();
		return;
	}

	if (_request.method() == http::verb::post) {
		bool success = LogicSystem::GetInstance()->HandlePost(_request.target(), shared_from_this());
		if (!success) {
			_response.result(http::status::not_found);
			_response.set(http::field::content_type, "test/plain");
			beast::ostream(_response.body()) << "url not found\r\n";
			WriteResponse();
			return;
		}
		_response.result(http::status::ok);
		_response.set(http::field::server, "GateServer");
		WriteResponse();
		return;
	}
}
void HttpConnection::WriteResponse() {
	auto self = shared_from_this();
	_response.content_length(_response.body().size());
	http::async_write(_socket, _response, [self](beast::error_code ec,std::size_t bytes_transferred) {
		self->_socket.shutdown(tcp::socket::shutdown_send, ec); // tcp�رգ��ȹ��Լ�
		self->deadline_.cancel();
		});
}
void HttpConnection::CheckDeadline(){
	auto self = shared_from_this();
	deadline_.async_wait([self](beast::error_code ec) {
		if (!ec) {
			self->_socket.close();
		}
		});
}
