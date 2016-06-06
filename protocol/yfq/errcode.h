//
// Created by root on 2/26/16.
//

#ifndef YFQSERVER_ERRCODE_H
#define YFQSERVER_ERRCODE_H

namespace ft {
namespace proto {
namespace yfqError {
/**
 * error code
 */
enum ErrorCode {
  Invalid = 0xffffffff,                   // 无效错误码
  None = 0x00000000,                      // 无错误
  Protocol_Fatal = 0x00000001,            // 协议错误
  Insert_To_Cache_Failed = 0x00000002,    // 插入数据到缓存失败
  Failed_To_Serial_Protobuf = 0x00000003, // 序列化protobuf失败
  Quality_Life_No_Data = 0x00000004,      // 该类型品质生活建议无数据
  Db_Init_Failed = 0x00000005,            //数据库初始化失败
  Db_Get_Connect_Failed = 0x00000006,     //数据库获取连接失败
  Db_Execute_Update_Failed = 0x00000007,  //执行数据库插入更改失败
  Db_Execute_Query_Failed = 0x00000008,   //执行数据库查询失败
  Failed_To_Insert_UserInfo = 0x00000009, //缓存用户信息失败
  Failed_To_Remove_UserInfo = 0x0000000a, //删除用户信息失败
  Failed_To_Judge_IsOnLine = 0x0000000b,   //判断用户是否在线失败
  Failed_To_Init_Mysql = 0x0000000c,    //初始化Mysql失败
  DB_No_Data = 0x0000000d,        //数据库没有查询到数据
  Error_Not_Know = 0x0000000f,      //未知错误
  Param_Error = 0x00000010        //函数传参有问题
};
}
}
}

#endif //YFQSERVER_ERRCODE_H
