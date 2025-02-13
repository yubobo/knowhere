// Copyright (C) 2019-2020 Zilliz. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied. See the License for the specific language governing permissions and limitations under the License

#include <memory>
#include <string>

#ifdef KNOWHERE_GPU_VERSION
#include <faiss/gpu/GpuAutoTune.h>
#include <faiss/gpu/GpuCloner.h>
#endif
#include <faiss/IndexFlat.h>
#include <faiss/IndexScalarQuantizer.h>
#include <faiss/clone_index.h>

#include "common/Exception.h"
#include "common/Utils.h"
#include "index/vector_index/IndexIVFSQ.h"
#include "index/vector_index/adapter/VectorAdapter.h"
#include "index/vector_index/helpers/IndexParameter.h"
#ifdef KNOWHERE_GPU_VERSION
#include "index/vector_index/gpu/IndexGPUIVFSQ.h"
#include "index/vector_index/helpers/FaissGpuResourceMgr.h"
#endif

namespace knowhere {

void
IVFSQ::Train(const DatasetPtr& dataset_ptr, const Config& config) {
    GET_TENSOR_DATA_DIM(dataset_ptr)

    knowhere::utils::SetBuildOmpThread(config);
    faiss::MetricType metric_type = GetFaissMetricType(config);
    faiss::Index* coarse_quantizer = new faiss::IndexFlat(dim, metric_type);
    auto index = std::make_shared<faiss::IndexIVFScalarQuantizer>(
        coarse_quantizer, dim, GetIndexParamNlist(config), faiss::QuantizerType::QT_8bit, metric_type);
    index->own_fields = true;
    index->train(rows, reinterpret_cast<const float*>(p_data));
    index_ = index;
}

VecIndexPtr
IVFSQ::CopyCpuToGpu(const int64_t device_id, const Config& config) {
#ifdef KNOWHERE_GPU_VERSION
    if (auto res = FaissGpuResourceMgr::GetInstance().GetRes(device_id)) {
        ResScope rs(res, device_id, false);

        auto gpu_index = faiss::gpu::index_cpu_to_gpu(res->faiss_res.get(), device_id, index_.get());

        std::shared_ptr<faiss::Index> device_index;
        device_index.reset(gpu_index);
        return std::make_shared<GPUIVFSQ>(device_index, device_id, res);
    } else {
        KNOWHERE_THROW_MSG("CopyCpuToGpu Error, can't get gpu_resource");
    }
#else
    KNOWHERE_THROW_MSG("Calling IVFSQ::CopyCpuToGpu when we are using CPU version");
#endif
}

int64_t
IVFSQ::Size() {
    if (!index_) {
        KNOWHERE_THROW_MSG("index not initialize");
    }
    auto ivfsq_index = dynamic_cast<faiss::IndexIVFScalarQuantizer*>(index_.get());
    auto nb = ivfsq_index->invlists->compute_ntotal();
    auto code_size = ivfsq_index->code_size;
    auto nlist = ivfsq_index->nlist;
    auto d = ivfsq_index->d;
    // ivf codes, ivf ids, sq trained vectors and quantizer
    return (nb * code_size + nb * sizeof(int64_t) + 2 * code_size + nlist * code_size);
}

}  // namespace knowhere
