#pragma once
#include <boost/asio.hpp>
#include "Singleton.h"
#include <vector>

class AsioIOServicePool :public Singleton<AsioIOServicePool>
{
	friend class Singleton<AsioIOServicePool>;

	using IOService = boost::asio::io_context;
	// using Work = boost::asio::io_context::work; // 这是旧版boost的写法
	using WorkGuard = boost::asio::executor_work_guard<IOService::executor_type>;
	using WorkPtr = std::unique_ptr<WorkGuard>;
public:
	~AsioIOServicePool();

	AsioIOServicePool(const AsioIOServicePool&) = delete;
	AsioIOServicePool& operator=(const AsioIOServicePool&) = delete;
	boost::asio::io_context& GetIOService();
	void Stop();
private:
	AsioIOServicePool(std::size_t size = std::thread::hardware_concurrency());
	std::vector<IOService> _ioServices;
	std::vector<WorkPtr> _works;
	std::vector<std::thread> _threads;
	std::size_t _nextIOService;
};

