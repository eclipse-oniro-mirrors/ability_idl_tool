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
class MetadataReaderUnitTest : public testing::Test {
public:
    MetadataReaderUnitTest() {}

    virtual ~MetadataReaderUnitTest() {}

    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};

void MetadataReaderUnitTest::SetUpTestCase() {}

void MetadataReaderUnitTest::TearDownTestCase() {}

void MetadataReaderUnitTest::SetUp() {}

void MetadataReaderUnitTest::TearDown() {}

/*
 * @tc.name: MetadataReaderUnitTest_0100
 * @tc.desc: test ReadMetadataFromFile in MetadataReaderUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataReaderUnitTest, MetadataReaderUnitTest_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataReader> metadataReader = std::make_shared<MetadataReader>();
    ASSERT_NE(nullptr, metadataReader);
    String filePath = "this is filePath";
    std::shared_ptr<MetaComponent> result = metadataReader->ReadMetadataFromFile(filePath);
    EXPECT_EQ(result, nullptr);
}

/*
 * @tc.name: MetadataReaderUnitTest_0200
 * @tc.desc: test ReadMetadataFromFile in MetadataReaderUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataReaderUnitTest, MetadataReaderUnitTest_0200, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataReader> metadataReader = std::make_shared<MetadataReader>();
    ASSERT_NE(nullptr, metadataReader);
    String filePath = String::Format("%s/%s", "./", "special_name_test.ts");

    std::shared_ptr<MetaComponent> result = metadataReader->ReadMetadataFromFile(filePath);
    EXPECT_EQ(result, nullptr);
}
} // namespace idl
} // namespace OHOS
