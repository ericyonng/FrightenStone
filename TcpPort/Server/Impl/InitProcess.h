/**
* @file InitProcess.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/29
* @brief
*/

#pragma once

#include "base/exportbase.h"

class InitProcess
{
public:
    InitProcess();
    virtual ~InitProcess();

private:
    fs::Locker _locker;
};
