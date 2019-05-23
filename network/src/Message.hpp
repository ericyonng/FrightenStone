#ifndef _Message_hpp_
#define _Message_hpp_
//定义网络报文

enum MessageTypes
{
	T_Login=0,
	T_Login_Result,
	T_Logout,
	T_Logout_Result,
	T_ERROR
};

struct MesageHeader
{
	short lenth;
	short type;
};

struct Login :public MesageHeader
{
	Login()
	{
		lenth = sizeof(Login);
		type = T_Login;
	}
	char name[32];
	char password[32];
};
struct Login_Result :public MesageHeader
{
	Login_Result()
	{
		lenth = sizeof(Login_Result);
		type = T_Login_Result;
		result = 0;				//0表示成功
	}
	char name[32] = { 0 };
	int result;
};

struct Logout :public MesageHeader
{
	Logout()
	{
		lenth = sizeof(Logout);
		type = T_Logout;
	}
	char name[32];
	char password[32];	//要验证退出消息的合法性，所以这里加上密码来验证
};


struct Logout_Result :public MesageHeader
{
	Logout_Result()
	{
		lenth = sizeof(Logout);
		type = T_Logout_Result;
		result = 0;		//0表示成功
	}
	int result;
};

#endif