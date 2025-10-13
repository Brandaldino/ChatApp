const config_module = require('./config')
const Redis = require("ioredis")


// 创建Redis客户端实例
const RedisCli = new Redis({
  host: config_module.redis_host,       // Redis服务器主机名
  port: config_module.redis_port,        // Redis服务器端口号
  password: config_module.redis_passwd, // Redis密码
});





// /**
//  * 监听错误信息
//  */
// RedisCli.on("error", function (err) {
//   console.log("RedisCli connect error");
//   RedisCli.quit();
// });


/**
 * 监听错误信息
 */
RedisCli.on("error", function (err) {
  // 添加详细的调试信息
  console.error("\n===== Redis连接错误 =====");
  console.error(`错误信息: ${err.message}`);
  console.error(`连接地址: redis://${config_module.redis_host}:${config_module.redis_port}`);
  console.error(`使用密码: ${config_module.redis_passwd ? "是" : "否"}`);
  
  // 添加连接状态检测
  console.error("\n诊断建议:");
  if (err.code === 'ECONNREFUSED') {
    console.error("→ Redis服务未启动或地址/端口错误");
    console.error("→ 请运行: redis-server.exe (Windows) 或 sudo service redis start (Linux)");
  } else if (err.message.includes('auth')) {
    console.error("→ 密码验证失败");
    console.error("→ 检查redis_passwd配置是否正确");
  } else if (err.message.includes('ENOTFOUND')) {
    console.error("→ 无法解析主机地址");
    console.error("→ 检查redis_host配置是否正确");
  }
  
  console.error("=========================\n");
  
  RedisCli.quit();
});




/**
 * 根据key获取value
 * @param {*} key 
 * @returns 
 */
async function GetRedis(key) {
    try{
        const result = await RedisCli.get(key)
        if(result === null){
          console.log('result:','<'+result+'>', 'This key cannot be find...')
          return null
        }
        console.log('Result:','<'+result+'>','Get key success!...');
        return result
    }catch(error){
        console.log('GetRedis error is', error);
        return null
    }
  }
/**
 * 根据key查询redis中是否存在key
 * @param {*} key 
 * @returns 
 */
async function QueryRedis(key) {
    try{
        const result = await RedisCli.exists(key)
        //  判断该值是否为空 如果为空返回null
        if (result === 0) {
          console.log('result:<','<'+result+'>','This key is null...');
          return null
        }
        console.log('Result:','<'+result+'>','With this value!...');
        return result
    }catch(error){
        console.log('QueryRedis error is', error);
        return null
    }
  }
/**
 * 设置key和value，并过期时间
 * @param {*} key 
 * @param {*} value 
 * @param {*} exptime 
 * @returns 
 */
async function SetRedisExpire(key,value, exptime){
    try{
        // 设置键和值
        await RedisCli.set(key,value)
        // 设置过期时间（以秒为单位）
        await RedisCli.expire(key, exptime);
        return true;
    }catch(error){
        console.log('SetRedisExpire error is', error);
        return false;
    }
}
/**
 * 退出函数
 */
function Quit(){
    RedisCli.quit();
}
module.exports = {GetRedis, QueryRedis, Quit, SetRedisExpire,}