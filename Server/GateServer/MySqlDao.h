#pragma once
#include "const.h"


class SqlConnection {
public:
	SqlConnection(sql::Connection* con, int64_t lasttime) :_con(con), _last_oper_time(lasttime) {};
	std::unique_ptr<sql::Connection> _con;
	int64_t _last_oper_time;
};

class MySqlPool
{
public:
	MySqlPool(
		std::string url, 
		const std::string& user,
		const std::string& pass, 
		const std::string& schema, 
		int poolSize
	)
		:_url(url), _user(user), _pass(pass), _schema(schema), _poolSize(poolSize), _b_stop(false) {
		try {
			for (int i = 0; i < poolSize; ++i) {
				sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
				std::unique_ptr<sql::Connection> con(driver->connect(_url, _user, _pass));
				con->setSchema(_schema);
				_pool.push(std::move(con));
			}
		}
		catch(sql::SQLException& e){
			std::cout << "mysql pool init failed." << std::endl;
		}
	}

	std::unique_ptr<sql::Connection> getConnection() {
		std::unique_lock<std::mutex> lock(_mtx);
		_cv.wait(lock, [this]() {
			if (_b_stop) return true;
			return !_pool.empty();
			});
		if (_b_stop) return nullptr;
		std::unique_ptr<sql::Connection> con(std::move(_pool.front()));
		_pool.pop();
		return con;
	}

	void returnConnection(std::unique_ptr<sql::Connection> con) {
		std::unique_lock<std::mutex> lock(_mtx);
		if (_b_stop) return;
		_pool.push(std::move(con));
		_cv.notify_one();
	}

	void Close() {
		_b_stop = true;
		_cv.notify_all();
	}

	~MySqlPool() {
		std::unique_lock<std::mutex> lock(_mtx);
		while (!_pool.empty()) {
			_pool.pop();
		}
	}

private:
	std::string _url;
	std::string _user;
	std::string _pass;
	std::string _schema;
	int _poolSize;
	std::queue<std::unique_ptr<sql::Connection>> _pool;
	std::mutex _mtx;
	std::condition_variable _cv;
	std::atomic<bool> _b_stop;
};

struct UserInfo {
	std::string name;
	std::string pwd;
	int uid;
	std::string email;
};

class MysqlDao {
public:
	MysqlDao();
	~MysqlDao();
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
	bool CheckEmail(const std::string& name, const std::string& email);
	bool UpdatePwd(const std::string& name, const std::string& newpwd);
	bool CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userinfo);
private:
	std::unique_ptr<MySqlPool> _pool;
};


