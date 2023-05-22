/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
 */

#include <gtest/gtest.h>
#define private public
#define protected public
#include "metadata/metadata_builder.h"
#include "metadata/metadata_dumper.h"
#include "metadata/metadata_reader.h"
#include "metadata/metadata_serializer.h"
#undef private
#undef protected

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS {
namespace idl {
class MetadataDumperUnitTest : public testing::Test {
public:
    MetadataDumperUnitTest() {}

    virtual ~MetadataDumperUnitTest() {}

    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};

void MetadataDumperUnitTest::SetUpTestCase() {}

void MetadataDumperUnitTest::TearDownTestCase() {}

void MetadataDumperUnitTest::SetUp() {}

void MetadataDumperUnitTest::TearDown() {}

/*
 * @tc.name: MetadataDumperUnitTest_0100
 * @tc.desc: test Dump in MetadataDumperUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataDumperUnitTest, MetadataDumperUnitTest_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataDumper> metadataDumper = std::make_shared<MetadataDumper>(nullptr);
    ASSERT_NE(nullptr, metadataDumper);

    String prefix = "this is prefix";
    metadataDumper->Dump(prefix);
}

/*
 * @tc.name: MetadataDumperUnitTest_0800
 * @tc.desc: test DumpMetaParameter in MetadataDumperUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataDumperUnitTest, MetadataDumperUnitTest_0800, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataDumper> metadataDumper = std::make_shared<MetadataDumper>(nullptr);
    ASSERT_NE(nullptr, metadataDumper);

    MetaType mt = {TypeKind::Char, 0, 1, new (std::nothrow) int(0)};
    metadataDumper->DumpMetaType(&mt);
}

/*
 * @tc.name: MetadataDumperUnitTest_0900
 * @tc.desc: test DumpMetaParameter in MetadataDumperUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataDumperUnitTest, MetadataDumperUnitTest_0900, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataDumper> metadataDumper = std::make_shared<MetadataDumper>(nullptr);
    ASSERT_NE(nullptr, metadataDumper);

    MetaType mt = {TypeKind::Boolean, 0, 1, new (std::nothrow) int(0)};
    metadataDumper->DumpMetaType(&mt);
}

/*
 * @tc.name: MetadataDumperUnitTest_1000
 * @tc.desc: test DumpMetaParameter in MetadataDumperUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataDumperUnitTest, MetadataDumperUnitTest_1000, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataDumper> metadataDumper = std::make_shared<MetadataDumper>(nullptr);
    ASSERT_NE(nullptr, metadataDumper);

    MetaType mt = {TypeKind::Byte, 0, 1, new (std::nothrow) int(0)};
    metadataDumper->DumpMetaType(&mt);
}

/*
 * @tc.name: MetadataDumperUnitTest_1100
 * @tc.desc: test DumpMetaParameter in MetadataDumperUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataDumperUnitTest, MetadataDumperUnitTest_1100, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataDumper> metadataDumper = std::make_shared<MetadataDumper>(nullptr);
    ASSERT_NE(nullptr, metadataDumper);

    MetaType mt = {TypeKind::Short, 0, 1, new (std::nothrow) int(0)};
    metadataDumper->DumpMetaType(&mt);
}

/*
 * @tc.name: MetadataDumperUnitTest_1200
 * @tc.desc: test DumpMetaParameter in MetadataDumperUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataDumperUnitTest, MetadataDumperUnitTest_1200, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataDumper> metadataDumper = std::make_shared<MetadataDumper>(nullptr);
    ASSERT_NE(nullptr, metadataDumper);

    MetaType mt = {TypeKind::Integer, 0, 1, new (std::nothrow) int(0)};
    metadataDumper->DumpMetaType(&mt);
}

/*
 * @tc.name: MetadataDumperUnitTest_1300
 * @tc.desc: test DumpMetaParameter in MetadataDumperUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataDumperUnitTest, MetadataDumperUnitTest_1300, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataDumper> metadataDumper = std::make_shared<MetadataDumper>(nullptr);
    ASSERT_NE(nullptr, metadataDumper);

    MetaType mt = {TypeKind::Long, 0, 1, new (std::nothrow) int(0)};
    metadataDumper->DumpMetaType(&mt);
}

/*
 * @tc.name: MetadataDumperUnitTest_1400
 * @tc.desc: test DumpMetaParameter in MetadataDumperUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataDumperUnitTest, MetadataDumperUnitTest_1400, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataDumper> metadataDumper = std::make_shared<MetadataDumper>(nullptr);
    ASSERT_NE(nullptr, metadataDumper);

    MetaType mt = {TypeKind::Float, 0, 1, new (std::nothrow) int(0)};
    metadataDumper->DumpMetaType(&mt);
}

/*
 * @tc.name: MetadataDumperUnitTest_1500
 * @tc.desc: test DumpMetaParameter in MetadataDumperUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataDumperUnitTest, MetadataDumperUnitTest_1500, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataDumper> metadataDumper = std::make_shared<MetadataDumper>(nullptr);
    ASSERT_NE(nullptr, metadataDumper);

    MetaType mt = {TypeKind::Double, 0, 1, new (std::nothrow) int(0)};
    metadataDumper->DumpMetaType(&mt);
}

/*
 * @tc.name: MetadataDumperUnitTest_1600
 * @tc.desc: test DumpMetaParameter in MetadataDumperUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataDumperUnitTest, MetadataDumperUnitTest_1600, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataDumper> metadataDumper = std::make_shared<MetadataDumper>(nullptr);
    ASSERT_NE(nullptr, metadataDumper);

    MetaType mt = {TypeKind::String, 0, 1, new (std::nothrow) int(0)};
    metadataDumper->DumpMetaType(&mt);
}
} // namespace idl
} // namespace OHOS
