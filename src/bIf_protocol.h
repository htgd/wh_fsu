/**@file  	    bIf_protocol.h
 * @brief   	动换监控 B接口协议实现
 * @details
 * @author  	wanghuan  any question please send mail to 371463817@qq.com
 * @date    	2019-08-07
 * @version 	V1.0
 * @copyright	Copyright (c) 2019-2022  江苏亨通光网科技有限公司
 **********************************************************************************
 * @attention
 * 硬件平台:iMX6ULL \n
 * 内核版本：L4.1.15
 * @par 修改日志:
 * <table>
 * <tr><th>Date        <th>Version  <th>Author    <th>Description
 * <tr><td>2019/08/07  <td>1.0      <td>wanghuan  <td>创建初始版本
 * </table>
 **********************************************************************************
 */

#ifndef __BIF_PROTOCOL_H
#define	__BIF_PROTOCOL_H

// 常量定义
#define USER_LENGTH 	20	///< 用户名长度
#define PASSWORD_LEN 	20	///< 口令长度
#define DES_LENGTH 		120	///< 描述信息长度
#define VER_LENGTH 		20	///< 版本描述的长度
#define FSUID_LEN 		20	///< FSU ID字符串长度
#define NMALARMID_LEN 	40	///< 网管告警编号
#define IP_LENGTH 		15	///< IP串长度
#define DEVICEID_LEN 	26	///< 设备ID长度
#define ID_LENGTH 		12	///< 监控点/站点/机房ID长度
#define SERIALNO_LEN 	10	///< 告警序号长度
#define TIME_LEN 		19	///< 时间串长度
#define DEV_CONF_LEN   	1000	///< 设备配置信息长度
#define ALARMREMARK_LEN	60	///< 告警预留字段
#define NAME_LENGTH 	80	///< 名字命名长度
#define FAILURE_CAUSE_LEN 	40	///< 失败原因描述信息长度

#if 0
// 枚举定义
/**@brief 报文返回结果 */
enum EnumResult
{
	FSU_FAILURE=0,	///< 失败
	FSU_SUCCESS,	///< 成功
	FSU_LOGIN		///< 重新登录
};

/**@brief 监控系统数据的种类 */
typedef enum
{
	DI=0,	///< 数字输入量（包含多态数字输入量），遥信
	AI=1,	///< 模拟输入量，遥测
	DO=2,	///< 数字输出量，遥控
	AO=3	///< 模拟输出量，遥调
}EnumType;

/**@brief 数据值的状态 */
typedef enum
{
	NOALARM=0,	///< 正常数据
	CRITICAL=1,	///< 一级告警
	MAJOR=2,	///< 二级告警
	MINOR=3,	///< 三级告警
	HINT=4,		///< 四级告警
	OPEVENT=5,	///< 操作事件
	INVALID=6,	///< 无效数据
}EnumState;

/**@brief 告警标志 */
typedef enum
{
	BEGIN,	///< 开始
	END,	///< 结束
}EnumFlag;

/**@brief 门禁命令执行结果 */
typedef enum
{
//	FAILURE=0,	///< 失败
//	SUCCESS=1,	///< 成功
	BROKEN=2,	///< 控制器断线
}EnumDoorResult;

// 数据结构定义
/**@brief 时间的结构 */
typedef struct
{
	short Years;	///< 年
	char  Month;
	char  Day;
	char  Hour;
	char  Minute;
	char  Second;
} TTime;

/**@brief 信号量的值的结构 */
typedef struct
{
	EnumType Type;			///< 数据类型
	char  ID[ID_LENGTH];	///< 监控点ID
	float  MeasuredVal;		///< 实测值
	float  SetupVal;		///< 设置值
	EnumState Status;		///< 状态
	char  Time[TIME_LEN];	///< 时间，格式YYYY-MM-DD<SPACE键>hh:mm:ss（采用24小时的时间制式）
} TSemaphore;

/**@brief 信号量的门限值的结构 */
typedef struct
{
	EnumType Type;			///< 数据类型
	char  ID[ID_LENGTH];	///< 监控点ID
	float  Threshold;		///< 门限值
	float  AbsoluteVal;		///< 绝对阀值
	float  RelativeVal;		///< 百分比阀值
	EnumState Status;		///< 状态
	short  Period;			///< 存储周期（秒）
} TThreshold;

/**@brief 告警消息的结构 */
typedef struct
{
	char SerialNo[SERIALNO_LEN];	///< 告警序号，同一条告警的开始和结束，告警序号要一致
	char ID[ID_LENGTH];				///< 监控点ID
	char DeviceID[DEVICEID_LEN];	///< 设备ID
	char AlarmTime[TIME_LEN];		///< 告警时间，YYYY-MM-DD<SPACE键>hh:mm:ss（采用24小时的时间制式）
	EnumState  AlarmLevel;			///< 告警级别
	EnumFlag   AlarmFlag;			///< 告警标志
	char AlarmDesc[DES_LENGTH];		///< 告警的事件描述，内容为：信号标准名 + 空格 + 告警信息(告警信息根据B接口规范填写)
	float EventValue;				///< 告警触发值
	char AlarmRemark1[ALARMREMARK_LEN];		///< 预留字段1
	char AlarmRemark2[ALARMREMARK_LEN];		///< 预留字段2
} TAlarm;

/**@brief 监控点信号配置信息 */
typedef struct
{
	EnumType Type;					///< 数据类型
	char ID[ID_LENGTH];				///< 监控点ID
	char SignalName[NAME_LENGTH];	///< 信号名称
	EnumState AlarmLevel;			///< 告警等级
	float Threshold;				///< 门限值
	float AbsoluteVal;				///< 绝对阀值
	float RelativeVal;				///< 百分比阀值
	char Describe[DES_LENGTH];		///< 描述信息。状态信号为状态描述,格式举例：0&正常;1&告警 。模拟信号为单位
	short Period;					///< 存储周期（秒）

} TSignal;

/**@brief FSU状态参数 */
typedef struct
{
	float CPUUsage;		///< CPU使用率
	float MEMUsage;		///< 内存使用率
} TFSUStatus;

#endif

/**@brief 监控对象配置信息 */
typedef struct
{
	char DeviceID[DEVICEID_LEN];	///< 设备ID（设备ID最大长度5位，从00001起算）
	char DeviceName[NAME_LENGTH];	///< 设备名称[定义参考中国电信动环监控集约化统一编码及命名规范]
	char RoomName[NAME_LENGTH];		///< 站点/机房名称[定义参考中国电信动环监控集约化统一编码及命名规范]
//	EnumDeviceType DeviceType;		///< 设备类型（按动环标准化定义）
//	EnumDeviceSubType DeviceSubType;	///< 设备子类型（按动环标准化定义）
	char Model[NAME_LENGTH];		///< 设备型号
	char Brand[NAME_LENGTH];		///< 设备品牌
	float RatedCapacity;			///< 额定容量
	char Version[VER_LENGTH];		///< 版本
	char BeginRunTime[TIME_LEN];	///< 启用时间
	char DevDescribe[DES_LENGTH];	///< 设备描述信息（包含设备的安装位置）
//	TSignal *Signals;				///< 监控点信号配置信息 ,具体参见下表注。

} TDevConf;



#endif	/* __BIF_PROTOCOL_H */

















