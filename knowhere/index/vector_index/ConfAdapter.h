// Copyright (C) 2019-2020 Zilliz. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied. See the License for the specific language governing permissions and limitations under the License.

#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "knowhere/common/Config.h"
#include "knowhere/index/IndexType.h"

namespace knowhere {

class ConfAdapter {
 public:
    virtual bool
    CheckTrain(Config& cfg, const IndexMode mode);

    virtual bool
    CheckSearch(Config& cfg, const IndexType type, const IndexMode mode);

    virtual bool
    CheckRangeSearch(Config& cfg, const IndexType type, const IndexMode mode);
};
using ConfAdapterPtr = std::shared_ptr<ConfAdapter>;

class IVFConfAdapter : public ConfAdapter {
 public:
    bool
    CheckTrain(Config& cfg, const IndexMode mode) override;

    bool
    CheckSearch(Config& cfg, const IndexType type, const IndexMode mode) override;
};

class IVFSQConfAdapter : public IVFConfAdapter {
 public:
    bool
    CheckTrain(Config& cfg, const IndexMode mode) override;
};

class IVFPQConfAdapter : public IVFConfAdapter {
 public:
    bool
    CheckTrain(Config& cfg, const IndexMode mode) override;

    static bool
    CheckPQParams(int64_t dimension, int64_t m, int64_t nbits, IndexMode& mode);

    static bool
    CheckGPUPQParams(int64_t dimension, int64_t m, int64_t nbits);

    static bool
    CheckCPUPQParams(int64_t dimension, int64_t m);
};

class BinIDMAPConfAdapter : public ConfAdapter {
 public:
    bool
    CheckTrain(Config& cfg, const IndexMode mode) override;
};

class BinIVFConfAdapter : public IVFConfAdapter {
 public:
    bool
    CheckTrain(Config& cfg, const IndexMode mode) override;
};

class HNSWConfAdapter : public ConfAdapter {
 public:
    bool
    CheckTrain(Config& cfg, const IndexMode mode) override;

    bool
    CheckSearch(Config& cfg, const IndexType type, const IndexMode mode) override;
};

class ANNOYConfAdapter : public ConfAdapter {
 public:
    bool
    CheckTrain(Config& cfg, const IndexMode mode) override;

    bool
    CheckSearch(Config& cfg, const IndexType type, const IndexMode mode) override;
};

}  // namespace knowhere
