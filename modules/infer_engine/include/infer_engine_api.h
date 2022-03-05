/**
 * *****************************************************************************
 * 				版权所有 (C), 帷幄匠心科技（杭州）有限公司
 * 				Copyright 2022 Whale, All Rights Reserved. 
 * ******************************************************************************
 * 文 件 名: infer_engine_api.h
 * 创建日期: 2022-03-01
 * 作    者: rongzhaoyang (rongzhaoyang@whale.im)
 * 功能描述: 对外API
 * ******************************************************************************
 */
#ifndef __INFER_ENGINE_API_H__
#define __INFER_ENGINE_API_H__

#include "ie_engine.hpp"

/**
 * 功能描述: 工厂模式：生成 engine 
 * 
 * 输入参数： param 
 * 输出参数： errcode 
 * 返回参数：IE_Engine* 
 */
IE_Engine* create_engine(const char* model_path, const IE_Param& param, int* errcode);

/**
 * 功能描述: 运行引擎
 * 
 * 输入参数： engine 
 * 输出参数： outputs 
 * 返回参数：int 
 */
int run_engine(IE_Engine* engine, const cv::Mat& input_img, std::vector<IE_Blob>& outputs);

/**
 * 功能描述: 销毁引擎
 * 
 * 输入参数： engine 
 * 返回参数：
 */
void destroy_engine(IE_Engine* engine);

#endif // __INFER_ENGINE_API_H__