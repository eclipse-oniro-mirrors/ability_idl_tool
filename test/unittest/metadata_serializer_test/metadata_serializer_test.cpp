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
class MetadataSerializerUnitTest : public testing::Test {
public:
    MetadataSerializerUnitTest() {}

    virtual ~MetadataSerializerUnitTest() {}

    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};

void MetadataSerializerUnitTest::SetUpTestCase() {}

void MetadataSerializerUnitTest::TearDownTestCase() {}

void MetadataSerializerUnitTest::SetUp() {}

void MetadataSerializerUnitTest::TearDown() {}

/*
 * @tc.name: MetadataSerializerUnitTest_0100
 * @tc.desc: test SerializeMetaComponent in MetadataSerializerUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataSerializerUnitTest, MetadataSerializerUnitTest_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataSerializer> metadataSerializer = std::make_shared<MetadataSerializer>(nullptr);
    ASSERT_NE(nullptr, metadataSerializer);

    metadataSerializer->SerializeMetaComponent(nullptr);
}

/*
 * @tc.name: MetadataSerializerUnitTest_0200
 * @tc.desc: test SerializeMetaNamespace in MetadataSerializerUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataSerializerUnitTest, MetadataSerializerUnitTest_0200, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataSerializer> metadataSerializer = std::make_shared<MetadataSerializer>(nullptr);
    ASSERT_NE(nullptr, metadataSerializer);

    metadataSerializer->SerializeMetaNamespace(nullptr);
}

/*
 * @tc.name: MetadataSerializerUnitTest_0300
 * @tc.desc: test SerializeMetaSequenceable in MetadataSerializerUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataSerializerUnitTest, MetadataSerializerUnitTest_0300, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataSerializer> metadataSerializer = std::make_shared<MetadataSerializer>(nullptr);
    ASSERT_NE(nullptr, metadataSerializer);

    metadataSerializer->SerializeMetaSequenceable(nullptr);
}

/*
 * @tc.name: MetadataSerializerUnitTest_0400
 * @tc.desc: test SerializeMetaInterface in MetadataSerializerUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataSerializerUnitTest, MetadataSerializerUnitTest_0400, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataSerializer> metadataSerializer = std::make_shared<MetadataSerializer>(nullptr);
    ASSERT_NE(nullptr, metadataSerializer);

    metadataSerializer->SerializeMetaInterface(nullptr);
}

/*
 * @tc.name: MetadataSerializerUnitTest_0500
 * @tc.desc: test SerializeMetaMethod in MetadataSerializerUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataSerializerUnitTest, MetadataSerializerUnitTest_0500, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataSerializer> metadataSerializer = std::make_shared<MetadataSerializer>(nullptr);
    ASSERT_NE(nullptr, metadataSerializer);

    metadataSerializer->SerializeMetaMethod(nullptr);
}

/*
 * @tc.name: MetadataSerializerUnitTest_0600
 * @tc.desc: test SerializeMetaParameter in MetadataSerializerUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataSerializerUnitTest, MetadataSerializerUnitTest_0600, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataSerializer> metadataSerializer = std::make_shared<MetadataSerializer>(nullptr);
    ASSERT_NE(nullptr, metadataSerializer);

    metadataSerializer->SerializeMetaParameter(nullptr);
}

/*
 * @tc.name: MetadataSerializerUnitTest_0700
 * @tc.desc: test SerializeMetaType in MetadataSerializerUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataSerializerUnitTest, MetadataSerializerUnitTest_0700, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataSerializer> metadataSerializer = std::make_shared<MetadataSerializer>(nullptr);
    ASSERT_NE(nullptr, metadataSerializer);

    metadataSerializer->SerializeMetaType(nullptr);
}

/*
 * @tc.name: MetadataSerializerUnitTest_0800
 * @tc.desc: test Deserialize in MetadataSerializerUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataSerializerUnitTest, MetadataSerializerUnitTest_0800, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataSerializer> metadataSerializer = std::make_shared<MetadataSerializer>(nullptr);
    ASSERT_NE(nullptr, metadataSerializer);

    metadataSerializer->Deserialize();
}

/*
 * @tc.name: MetadataSerializerUnitTest_0900
 * @tc.desc: test DeserializeMetaComponent in MetadataSerializerUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataSerializerUnitTest, MetadataSerializerUnitTest_0900, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataSerializer> metadataSerializer = std::make_shared<MetadataSerializer>(nullptr);
    ASSERT_NE(nullptr, metadataSerializer);

    metadataSerializer->DeserializeMetaComponent(nullptr);
}

/*
 * @tc.name: MetadataSerializerUnitTest_1000
 * @tc.desc: test DeserializeMetaNamespace in MetadataSerializerUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataSerializerUnitTest, MetadataSerializerUnitTest_1000, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataSerializer> metadataSerializer = std::make_shared<MetadataSerializer>(nullptr);
    ASSERT_NE(nullptr, metadataSerializer);

    metadataSerializer->DeserializeMetaNamespace(nullptr);
}

/*
 * @tc.name: MetadataSerializerUnitTest_1100
 * @tc.desc: test DeserializeMetaSequenceable in MetadataSerializerUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataSerializerUnitTest, MetadataSerializerUnitTest_1100, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataSerializer> metadataSerializer = std::make_shared<MetadataSerializer>(nullptr);
    ASSERT_NE(nullptr, metadataSerializer);

    metadataSerializer->DeserializeMetaSequenceable(nullptr);
}

/*
 * @tc.name: MetadataSerializerUnitTest_1200
 * @tc.desc: test DeserializeMetaInterface in MetadataSerializerUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataSerializerUnitTest, MetadataSerializerUnitTest_1200, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataSerializer> metadataSerializer = std::make_shared<MetadataSerializer>(nullptr);
    ASSERT_NE(nullptr, metadataSerializer);

    metadataSerializer->DeserializeMetaInterface(nullptr);
}

/*
 * @tc.name: MetadataSerializerUnitTest_1300
 * @tc.desc: test DeserializeMetaMethod in MetadataSerializerUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataSerializerUnitTest, MetadataSerializerUnitTest_1300, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataSerializer> metadataSerializer = std::make_shared<MetadataSerializer>(nullptr);
    ASSERT_NE(nullptr, metadataSerializer);

    metadataSerializer->DeserializeMetaMethod(nullptr);
}


/*
 * @tc.name: MetadataSerializerUnitTest_1400
 * @tc.desc: test DeserializeMetaParameter in MetadataSerializerUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataSerializerUnitTest, MetadataSerializerUnitTest_1400, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataSerializer> metadataSerializer = std::make_shared<MetadataSerializer>(nullptr);
    ASSERT_NE(nullptr, metadataSerializer);

    metadataSerializer->DeserializeMetaParameter(nullptr);
}

/*
 * @tc.name: MetadataSerializerUnitTest_1500
 * @tc.desc: test DeserializeMetaType in MetadataSerializerUnitTest.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(MetadataSerializerUnitTest, MetadataSerializerUnitTest_1500, Function | MediumTest | Level1)
{
    std::shared_ptr<MetadataSerializer> metadataSerializer = std::make_shared<MetadataSerializer>(nullptr);
    ASSERT_NE(nullptr, metadataSerializer);

    metadataSerializer->DeserializeMetaType(nullptr);
}
} // namespace idl
} // namespace OHOS
