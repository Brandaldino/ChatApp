#include "MySqlDao.h"
#include "ConfigMgr.h"

MysqlDao::MysqlDao(){
    auto& cfg = ConfigMgr::Inst();
    const auto& host = cfg["Mysql"]["Host"];
    const auto& port = cfg["Mysql"]["Port"];
    const auto& pwd = cfg["Mysql"]["Passwd"];
    const auto& schema = cfg["Mysql"]["Schema"];
    const auto& user = cfg["Mysql"]["User"];
    _pool.reset(new MySqlPool(host + ":" + port, user, pwd, schema, 5));
}
MysqlDao::~MysqlDao(){
	_pool->Close();
}
int MysqlDao::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
    auto con = _pool->getConnection();
    try {
        if (con == nullptr) {
            return false;
        }
        // ׼�����ô洢����
        std::unique_ptr <sql::PreparedStatement > stmt(con->prepareStatement("CALL reg_user(?,?,?,@result)"));
        // �����������
        stmt->setString(1, name);
        stmt->setString(2, email);
        stmt->setString(3, pwd);
        // ����PreparedStatement��ֱ��֧��ע�����������������Ҫʹ�ûỰ������������������ȡ���������ֵ
        // ִ�д洢����
        stmt->execute();

        // ����洢���������˻Ự��������������ʽ��ȡ���������ֵ�������������ִ��SELECT��ѯ����ȡ����
        // ���磬����洢����������һ���Ự����@result���洢������������������ȡ��
        std::unique_ptr<sql::Statement> stmtResult(con->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmtResult->executeQuery("SELECT @result AS result"));
        if (res->next()) {
            int result = res->getInt("result");
            std::cout << "Result: " << result << std::endl;
            _pool->returnConnection(std::move(con));
            return result;
        }
        _pool->returnConnection(std::move(con));
        return -1;
    }
    catch (sql::SQLException& e) {
        _pool->returnConnection(std::move(con));
        std::cerr << "SQLException in RegUser: " << e.what() << std::endl;
        std::cerr << "Error code: " << e.getErrorCode() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;

        // ��ӡ���������ע�⣺ʵ��Ӧ���в�Ҫ��ӡ���룩
        std::cerr << "Parameters: name=" << name
            << ", email=" << email
            << ", pwd=[PROTECTED]" << std::endl;

        return -1;
    }
}
bool MysqlDao::CheckEmail(const std::string& name, const std::string& email) {
    auto con = _pool->getConnection();
    try {
        if (con == nullptr) {
            _pool->returnConnection(std::move(con));
            return false;
        }
        // ׼����ѯ���
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("SELECT email FROM user WHERE name = ?"));
        bool foundMatch = false;

        // �󶨲���
        pstmt->setString(1, name);
        // ִ�в�ѯ
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        // ���������
        while (res->next()) {
            std::cout << "Check Email: " << res->getString("email") << std::endl;
            if (email == res->getString("email")) {
                foundMatch = true;
                break;
            }
        }
        _pool->returnConnection(std::move(con));
        return foundMatch;
    }
    catch (sql::SQLException& e) {
        _pool->returnConnection(std::move(con));
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}
bool MysqlDao::UpdatePwd(const std::string& name, const std::string& newpwd) {
    auto con = _pool->getConnection();
    try {
        if (con == nullptr) {
            _pool->returnConnection(std::move(con));
            return false;
        }
        // ׼����ѯ���
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("UPDATE user SET pwd = ? WHERE name = ?"));
        // �󶨲���
        pstmt->setString(2, name);
        pstmt->setString(1, newpwd);
        // ִ�и���
        int updateCount = pstmt->executeUpdate();
        std::cout << "Updated rows: " << updateCount << std::endl;
        _pool->returnConnection(std::move(con));
        return true;
    }
    catch (sql::SQLException& e) {
        _pool->returnConnection(std::move(con));
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}
bool MysqlDao::CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userInfo){
    auto con = _pool->getConnection();
    if (con == nullptr) {
        return false;
    }

    Defer defer([this, &con]() {
        _pool->returnConnection(std::move(con));
        });

    try {
        // ׼��SQL���
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("SELECT * FROM user WHERE email = ?"));
        pstmt->setString(1, email); // ��username�滻Ϊ��Ҫ��ѯ���û���

        // ִ�в�ѯ
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        std::string origin_pwd = "";
        // ���������
        while (res->next()) {
            origin_pwd = res->getString("pwd");
            // �����ѯ��������
            std::cout << "Password: " << origin_pwd << std::endl;
            break;
        }

        if (pwd != origin_pwd) {
            return false;
        }
        userInfo.name = res->getString("name");
        userInfo.email = res->getString("email");
        userInfo.uid = res->getInt("uid");
        userInfo.pwd = origin_pwd;
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}
