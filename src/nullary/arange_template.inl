/* Copyright 2021 NVIDIA Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "arg.h"
#include "core.h"
#include "dispatch.h"
#include "point_task.h"
#include "scalar.h"

namespace legate {
namespace numpy {

using namespace Legion;

template <VariantKind KIND, typename VAL>
struct ArangeImplBody;

template <VariantKind KIND>
struct ArangeImpl {
  template <LegateTypeCode CODE>
  void operator()(ArangeArgs &args) const
  {
    using VAL = legate_type_of<CODE>;

    const auto rect = args.shape.to_rect<1>();

    if (rect.empty()) return;

    auto out = args.out.write_accessor<VAL, 1>();

    const auto start = args.start.value<VAL>();
    const auto step  = args.step.value<VAL>();

    ArangeImplBody<KIND, VAL>{}(out, rect, start, step);
  }
};

template <VariantKind KIND>
static void arange_template(const Task *task,
                            const std::vector<PhysicalRegion> &regions,
                            Context context,
                            Runtime *runtime)
{
  Deserializer ctx(task, regions);
  ArangeArgs args;
  deserialize(ctx, args);
  type_dispatch(args.out.code(), ArangeImpl<KIND>{}, args);
}

}  // namespace numpy
}  // namespace legate