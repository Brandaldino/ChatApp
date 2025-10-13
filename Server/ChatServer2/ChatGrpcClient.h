#pragma once

#include "const.h"
#include "Singleton.h"
#include "ConfigMgr.h"
#include "message.grpc.pb.h"
#include "message.pb.h"
#include "data.h"

#include <grpcpp/grpcpp.h> 
#include <queue>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::AddFriendReq;
using message::AddFriendRsp;

using message::AuthFriendReq;
using message::AuthFriendRsp;

using message::GetChatServerRsp;
using message::LoginRsp;
using message::LoginReq;
using message::ChatService;

using message::TextChatMsgReq;
using message::TextChatMsgRsp;
using message::TextChatData;

class ChatConPool {
public:
	ChatConPool(size_t poolSize, std::string host, std::string port) :
		_poolSize(poolSize), _host(host), _port(port), _b_stop(false) {
		for (size_t i = 0; i < _poolSize; ++i) {
			std::shared_ptr<Channel> channel = grpc::CreateChannel(
				_host + ":" + _port, // 服务端地址
				grpc::InsecureChannelCredentials() // 主动使用未加密的传输层，允许一个不安全的连接
			);

			// 哨兵(邮递员)
			_connections.push(ChatService::NewStub(channel));
		}
	}

	~ChatConPool() {
		std::lock_guard<std::mutex> lock(_mtx);
		Close();
		while (!_connections.empty()) {
			_connections.pop();
		}
	}

	std::unique_ptr<ChatService::Stub> getConnection() {
		std::unique_lock<std::mutex> lock(_mtx);
		_cv.wait(lock, [this]() {
			if (_b_stop) return true;
			return !_connections.empty();
			});
		// 如果停止则直接返回空指针
		if (_b_stop) return nullptr;
		// 防御性编程，添加空队列保护
		if (_connections.empty()) return nullptr;

		auto context = std::move(_connections.front());
		_connections.pop();
		return context;
	}

	void returnConnection(std::unique_ptr<ChatService::Stub> context) {
		std::lock_guard<std::mutex> lock(_mtx);
		if (_b_stop) return;
		_connections.push(std::move(context));
		_cv.notify_one();
	}
	void Close() {
		_b_stop = true;
		_cv.notify_all();
	}
private:
	size_t _poolSize;
	std::string _host;
	std::string _port;
	std::queue<std::unique_ptr<ChatService::Stub>> _connections;
	std::mutex _mtx;
	std::condition_variable _cv;
	std::atomic<bool> _b_stop;
};

class ChatGrpcClient:public Singleton<ChatGrpcClient>
{
	friend class Singleton<ChatGrpcClient>;
public:
	~ChatGrpcClient() {

	}

	AddFriendRsp NotifyAddFriend(std::string server_ip, const AddFriendReq& req);
	AuthFriendRsp NotifyAuthFriend(std::string server_ip, const AuthFriendReq& req);
	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
	TextChatMsgRsp NotifyTextChatMsg(std::string server_ip, const TextChatMsgReq& req, const Json::Value& rtvalue);
	// KickUserRsp NotifyKickUser(std::string server_ip, const KickUserReq& req);
private:
	ChatGrpcClient();
	std::unordered_map<std::string, std::unique_ptr<ChatConPool>> _pools;
};

