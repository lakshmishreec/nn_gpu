/*
 * Copyright @2017 Intel Corporation
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef ANDROID_HARDWARE_NEURALNETWORKS_V1_0_VK_CS_EXECUTOR_H
#define ANDROID_HARDWARE_NEURALNETWORKS_V1_0_VK_CS_EXECUTOR_H

#include "gpu_executor.h"
#include "vk_operand.h"
#include "vk_memory_manager.h"
#include "vk_op_base.h"
#include "operation_cpu_timer.h"

NAME_SPACE_BEGIN

struct ShaderConfig
{
    int local_size_x;
    int local_size_y;
    int local_size_z;
    int block_width;
    int block_height;
    int block_depth;
    ShaderConfig(const int lsz_x,
                     const int lsz_y,
                     const int lsz_z,
                     const int block_w,
                     const int block_h,
                     const int block_d)
    {
        local_size_x = lsz_x;
        local_size_y = lsz_y;
        local_size_z = lsz_z;
        block_width  = block_w;
        block_height = block_h;
        block_depth  = block_d;
    }
};

class VkCsExecutor : public GpuExecutor
{
public:
    static bool initPerProcess();
    static void deinitPerProcess();
    static void getCapabilities(Capabilities& cap);
    static std::vector<bool> getSupportedOperations(const Model& model);
    //static bool checkGroupParam(uint32_t* localSize, uint32_t* groupCount);

    VkCsExecutor(const Model& model);
    ~VkCsExecutor() override;

    bool initPerModel() override;
    bool initPerExecThread() override;
    bool run(const Request& request) override;
    void deinitPerExecThread() override;
    void deinitPerModel() override;
    std::string getOpName(const Operation& operation);

private:
    //cannot be a global memMgr per process since the gl objects belong to one context (_ctx)
    VkMemoryManager memMgr;
    std::vector<VkOperand> operands;
    std::vector<OperationCpuTimer> operationTimers;
    std::shared_ptr<VkOpBase> opBase;

    void initOperands();
    void restoreOperands();
    void setArgOperands(const Request& request);

    void initOperationTimers();
    void showOperationTimers();
    void deinitOperationResources();

    bool run(const Operation& operation, OperationCpuTimer* timer);

    bool doEleWise(const Operation& operation, const int type);
    bool convolve(const Operation& operation, ShaderConfig& config);
    bool doPool(const Operation& operation, ShaderConfig& config, const int type);

#define SETUP_OP(op) bool do##op(const Operation& operation);
#include "vk_setup_op.hxx"
#undef SETUP_OP
};

NAME_SPACE_STOP

#endif
