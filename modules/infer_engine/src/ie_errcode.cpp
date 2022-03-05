/**
 * *****************************************************************************
 * 				版权所有 (C), 帷幄匠心科技（杭州）有限公司
 * 				Copyright 2022 Whale, All Rights Reserved. 
 * ******************************************************************************
 * 文 件 名: ie_errcode.cpp
 * 创建日期: 2022-02-28
 * 作    者: rongzhaoyang (rongzhaoyang@whale.im)
 * 功能描述: 错误码处理
 * ******************************************************************************
 */
#include "ie_errcode.hpp"

/**
 * 功能描述: 错误码转字符串
 * 
 * 输入参数： code 
 * 返回参数：const char* 
 */
const char* IE_err2str(int code)
{
    auto it = IE_ERR_TABLE.find(code);
    if (it == IE_ERR_TABLE.end())
        return "unknown error";
    return it->second;
}