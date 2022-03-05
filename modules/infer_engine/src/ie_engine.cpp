/**
 * *****************************************************************************
 * 				版权所有 (C), 帷幄匠心科技（杭州）有限公司
 * 				Copyright 2022 Whale, All Rights Reserved. 
 * ******************************************************************************
 * 文 件 名: ie_engine.cpp
 * 创建日期: 2022-02-28
 * 作    者: rongzhaoyang (rongzhaoyang@whale.im)
 * 功能描述: 推理引擎
 * ******************************************************************************
 */
#include "ie_engine.hpp"
#include "ie_preprocess.hpp"

/**
 * 功能描述: 前处理
 * 
 * 输入参数： input_img 
 * 返回参数：IE_ERR_CODE 
 */
IE_ERR_CODE IE_Engine::preprocess(const cv::Mat& input_img)
{
    return ie_preprocess(input_img, m_input_blob, 
                m_input_shape.w, m_input_shape.h,
                m_infer_param.pre_param.rgb_order,
                m_infer_param.pre_param.format,
                m_infer_param.pre_param.mean_values,
                m_infer_param.pre_param.std_values);
}