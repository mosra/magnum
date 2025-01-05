/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <Corrade/Containers/ArrayTuple.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/StringToFile.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/Math/CubicHermite.h"
#include "Magnum/Math/Half.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"

#include "Magnum/SceneTools/Implementation/sceneConverterUtilities.h"

#include "configure.h"

namespace Magnum { namespace SceneTools { namespace Test { namespace {

struct SceneConverterImplementationTest: TestSuite::Tester {
    explicit SceneConverterImplementationTest();

    /* printPluginInfo(), printPluginConfigurationInfo() and
       printImporterInfo() tested thoroughly in
       Trade/Test/ImageConverterImplementationTest.cpp already */
    void converterInfo();

    void infoEmpty();
    void infoScenesObjects();
    void infoAnimations();
    void infoSkins();
    void infoLights();
    void infoCameras();
    void infoMaterials();
    void infoMeshes();
    void infoMeshesBounds();
    void infoTextures();
    void infoImages();
    /* Image info further tested in ImageConverterImplementationTest */
    void infoReferenceCount();
    void infoError();

    Utility::Arguments _infoArgs;

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<Trade::AbstractSceneConverter> _converterManager{"nonexistent"};
};

using namespace Containers::Literals;
using namespace Math::Literals;

const struct {
    const char* name;
    Containers::Array<const char*> args;
    const char* expected;
    Int defaultScene;
    bool printVisualCheck;
    bool omitParent;
} InfoScenesObjectsData[]{
    {"all", {InPlaceInit, {
           "", "--info"
        }}, "info-scenes-objects.txt", 1, true, false},
    {"both", {InPlaceInit, {
           "",  "--info-objects", "--info-scenes"
        }}, "info-scenes-objects.txt", 1, false, false},
    {"only scenes", {InPlaceInit, {
            "", "--info-scenes"
        }}, "info-scenes.txt", 0, false, false},
    {"only scenes, no default scene", {InPlaceInit, {
            "", "--info-scenes"
        }}, "info-scenes-no-default.txt", -1, false, false},
    {"only objects", {InPlaceInit, {
            "", "--info-objects"
        }}, "info-objects.txt", 1, false, false},
    {"object hierarchy, all", {InPlaceInit, {
            "", "--info", "--object-hierarchy"
        }}, "info-object-hierarchy.txt", -1, true, false},
    {"object hierarchy, both", {InPlaceInit, {
            "", "--info-objects", "--info-scenes", "--object-hierarchy"
        }}, "info-object-hierarchy.txt", -1, false, false},
    {"object hierarchy, no parents", {InPlaceInit, {
            "", "--info", "--object-hierarchy"
        }}, "info-object-hierarchy-no-parents.txt", -1, false, true},
    {"object hierarchy, only scenes", {InPlaceInit, {
            /* --object-hierarchy is only used if --info-objects is present
               so this is the same as just --info-scenes alone */
            "", "--info-scenes", "--object-hierarchy"
        }}, "info-scenes-no-default.txt", -1, false, false},
    {"object hierarchy, only objects", {InPlaceInit, {
            "", "--info-objects", "--object-hierarchy"
        }}, "info-object-hierarchy-only-objects.txt", -1, true, false},
    {"object hierarchy, only objects, no parents", {InPlaceInit, {
            "", "--info-objects", "--object-hierarchy"
        }}, "info-object-hierarchy-only-objects-no-parents.txt", -1, false, true},
};

const struct {
    const char* name;
    bool oneOrAll;
    bool printVisualCheck;
} InfoOneOrAllData[]{
    {"", true, true},
    {"--info", false, false},
};

SceneConverterImplementationTest::SceneConverterImplementationTest() {
    addTests({&SceneConverterImplementationTest::converterInfo,

              &SceneConverterImplementationTest::infoEmpty});

    addInstancedTests({&SceneConverterImplementationTest::infoScenesObjects},
        Containers::arraySize(InfoScenesObjectsData));

    addInstancedTests({&SceneConverterImplementationTest::infoAnimations,
                       &SceneConverterImplementationTest::infoSkins,
                       &SceneConverterImplementationTest::infoLights,
                       &SceneConverterImplementationTest::infoCameras,
                       &SceneConverterImplementationTest::infoMaterials,
                       &SceneConverterImplementationTest::infoMeshes},
        Containers::arraySize(InfoOneOrAllData));

    addTests({&SceneConverterImplementationTest::infoMeshesBounds});

    addInstancedTests({&SceneConverterImplementationTest::infoTextures,
                       &SceneConverterImplementationTest::infoImages},
        Containers::arraySize(InfoOneOrAllData));

    addTests({&SceneConverterImplementationTest::infoReferenceCount,
              &SceneConverterImplementationTest::infoError});

    /* A subset of arguments needed by the info printing code */
    _infoArgs.addBooleanOption("info")
             .addBooleanOption("info-scenes")
             .addBooleanOption("info-objects")
             .addBooleanOption("info-animations")
             .addBooleanOption("info-skins")
             .addBooleanOption("info-lights")
             .addBooleanOption("info-cameras")
             .addBooleanOption("info-materials")
             .addBooleanOption("info-meshes")
             .addBooleanOption("info-textures")
             .addBooleanOption("info-images")
             .addBooleanOption("bounds")
             .addBooleanOption("object-hierarchy");

    /* Load the plugin directly from the build tree. Otherwise it's static and
       already loaded. */
    #ifdef ANYSCENECONVERTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_converterManager.load(ANYSCENECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* To avoid warnings that printImageConverterInfo() / printImporterInfo()
       is unused. Again, those are tested in ImageConverterImplementationTest
       already. MSVC says "warning C4551: function call missing argument list"
       here. No shit, you stupid thing. */
    #ifdef CORRADE_TARGET_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4551)
    #endif
    static_cast<void>(Trade::Implementation::printImageConverterInfo);
    static_cast<void>(Trade::Implementation::printImporterInfo);
    #ifdef CORRADE_TARGET_MSVC
    #pragma warning(pop)
    #endif
}

void SceneConverterImplementationTest::converterInfo() {
    /* Check if the required plugin can be loaded. Catches also ABI and
       interface mismatch errors. */
    if(!(_converterManager.load("AnySceneConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnySceneConverter plugin can't be loaded.");

    Containers::Pointer<Trade::AbstractSceneConverter> converter = _converterManager.instantiate("AnySceneConverter");
    /** @todo pick a plugin that has some actual configuration */
    converter->configuration().setValue("something", "is there");

    /* Print to visually verify coloring */
    {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printSceneConverterInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, *converter);
        Debug{} << "======================== visual color verification end =========================";
    }

    Containers::String out;
    Debug redirectOutput{&out};
    Implementation::printSceneConverterInfo(Debug::Flag::DisableColors, *converter);
    CORRADE_COMPARE(out,
        "Plugin name: AnySceneConverter\n"
        "Features:\n"
        "  ConvertMeshToFile\n"
        "  ConvertMultipleToFile\n"
        "Configuration:\n"
        "  something=is there\n");
}

void SceneConverterImplementationTest::infoEmpty() {
    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    } importer;

    const char* argv[]{"", "--info"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    Containers::String out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, {}, _infoArgs, importer, time) == false);
    CORRADE_COMPARE(out, "");
}

void SceneConverterImplementationTest::infoScenesObjects() {
    auto&& data = InfoScenesObjectsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Importer: Trade::AbstractImporter {
        explicit Importer(Int defaultScene, bool omitParent): _defaultScene{defaultScene}, _omitParent{omitParent} {}

        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        /* First scene has 4, second 7, the last three are not in any scene and
           thus not listed. Object 5 has no fields and thus not listed either. */
        UnsignedLong doObjectCount() const override { return 10; }
        UnsignedInt doSceneCount() const override { return 2; }
        Int doDefaultScene() const override {
            return _defaultScene;
        }
        Containers::String doSceneName(UnsignedInt id) override {
            return id == 0 ? "A simple scene" : "";
        }
        Containers::String doObjectName(UnsignedLong id) override {
            if(id == 0) return "Parent-less mesh";
            if(id == 2) return "Two meshes, shared among two scenes";
            if(id == 4) return "Two custom arrays";
            if(id == 6) return "Only in the second scene, but no fields, thus same as unreferenced";
            if(id == 8) return "Not in any scene";
            return "";
        }
        Containers::String doSceneFieldName(Trade::SceneField name) override {
            if(name == Trade::sceneFieldCustom(1337)) return "directionVector";
            return "";
        }
        Containers::Optional<Trade::SceneData> doScene(UnsignedInt id) override {
            /* Builtin fields, some duplicated, one marked as ordered */
            if(id == 0) {
                Containers::ArrayView<UnsignedInt> parentMapping;
                Containers::ArrayView<Int> parents;
                Containers::ArrayView<UnsignedInt> meshMapping;
                Containers::ArrayView<UnsignedInt> meshes;
                Containers::ArrayTuple data{
                    {NoInit, 5, parentMapping},
                    {NoInit, 5, parents},
                    {NoInit, 4, meshMapping},
                    {ValueInit, 4, meshes},
                };
                Utility::copy({1, 2, 5, 4, 0}, parentMapping);
                Utility::copy({2, -1, 1, 2, 5}, parents);
                Utility::copy({2, 0, 2, 1}, meshMapping);
                /* No need to fill the other data, zero-init is fine */
                return Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 6, Utility::move(data), {
                    Trade::SceneFieldData{_omitParent ? Trade::sceneFieldCustom(0) : Trade::SceneField::Parent, parentMapping, parents},
                    Trade::SceneFieldData{Trade::SceneField::Mesh, meshMapping, meshes, Trade::SceneFieldFlag::OrderedMapping},
                }};
            }

            /* Two custom fields, one array, parent. Stored as an external
               memory. */
            if(id == 1) {
                return Trade::SceneData{Trade::SceneMappingType::UnsignedByte, 8, Trade::DataFlag::ExternallyOwned|Trade::DataFlag::Mutable, scene2Data, {
                    Trade::SceneFieldData{_omitParent ? Trade::sceneFieldCustom(0) : Trade::SceneField::Parent, Containers::arrayView(scene2Data->parentCustomMapping), Containers::arrayView(scene2Data->parent)},
                    Trade::SceneFieldData{Trade::sceneFieldCustom(42), Containers::arrayView(scene2Data->parentCustomMapping), Containers::arrayView(scene2Data->custom)},
                    Trade::SceneFieldData{Trade::sceneFieldCustom(1337), Trade::SceneMappingType::UnsignedByte, scene2Data->customArrayMapping, Trade::SceneFieldType::Short, scene2Data->customArray, 3},
                }};
            }

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        struct {
            UnsignedByte parentCustomMapping[2];
            Int parent[2];
            Double custom[2];
            UnsignedByte customArrayMapping[3];
            Vector3s customArray[3];
        } scene2Data[1]{{
            /* No need to fill data other than parents, zero-init is fine */
            {7, 3}, {3, -1}, {}, {2, 4, 4}, {}
        }};

        Int _defaultScene;
        bool _omitParent;
    } importer{data.defaultScene, data.omitParent};

    CORRADE_VERIFY(_infoArgs.tryParse(data.args.size(), data.args));

    std::chrono::high_resolution_clock::duration time;

    /* Print to visually verify coloring */
    if(data.printVisualCheck) {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, Debug::isTty(), _infoArgs, importer, time);
        Debug{} << "======================== visual color verification end =========================";
    }

    Containers::String out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, {}, _infoArgs, importer, time) == false);
    CORRADE_COMPARE_AS(out,
        Utility::Path::join({SCENETOOLS_TEST_DIR, "SceneConverterImplementationTestFiles", data.expected}),
        TestSuite::Compare::StringToFile);
}

void SceneConverterImplementationTest::infoAnimations() {
    auto&& data = InfoOneOrAllData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doAnimationCount() const override { return 2; }
        Containers::String doAnimationName(UnsignedInt id) override {
            return id == 1 ? "Custom track duration and interpolator function" : "";
        }
        Containers::Optional<Trade::AnimationData> doAnimation(UnsignedInt id) override {
            /* First has two tracks with a shared time and implicit duration,
               one with a different result type, one with a custom target. */
            if(id == 0) {
                Containers::ArrayView<Float> time;
                Containers::StridedArrayView1D<Vector2> translation;
                Containers::StridedArrayView1D<CubicHermite2D> rotation;
                Containers::StridedArrayView1D<bool> visibility;
                Containers::ArrayTuple data{
                    {ValueInit, 3, time},
                    {ValueInit, 3, translation},
                    {ValueInit, 3, rotation},
                    {ValueInit, 3, visibility},
                };
                Utility::copy({0.5f, 1.0f, 1.25f}, time);
                return Trade::AnimationData{Utility::move(data), {
                    Trade::AnimationTrackData{Trade::AnimationTrackTarget::Translation2D, 17, time, translation, Animation::Interpolation::Linear, Animation::Extrapolation::DefaultConstructed, Animation::Extrapolation::Constant},
                    Trade::AnimationTrackData{Trade::AnimationTrackTarget::Rotation2D, 17, time, rotation, Animation::Interpolation::Constant, Animation::Extrapolation::Extrapolated},
                    Trade::AnimationTrackData{Trade::animationTrackTargetCustom(333), 666, time, visibility, Animation::Interpolation::Constant, Animation::Extrapolation::Constant},
                }};
            }

            /* Second has track duration different from animation duration and
               a custom interpolator. Stored as an external memory. */
            if(id == 1) {
                return Trade::AnimationData{Trade::DataFlag::ExternallyOwned, animation2Data, {
                    Trade::AnimationTrackData{Trade::AnimationTrackTarget::Scaling3D, 666, animation2Data->time, Containers::stridedArrayView(animation2Data->scaling), Math::lerp, Animation::Extrapolation::DefaultConstructed, Animation::Extrapolation::Constant},
                }, {0.1f, 1.3f}};
            }

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        Containers::String doAnimationTrackTargetName(Trade::AnimationTrackTarget name) override {
            if(name == Trade::animationTrackTargetCustom(333))
                return "visibility";
            return {};
        }

        struct {
            Float time[5];
            Vector3 scaling[5];
        } animation2Data[1]{{
            {0.75f, 0.75f, 1.0f, 1.0f, 1.25f},
            {}
        }};
    } importer;

    const char* argv[]{"", data.oneOrAll ? "--info-animations" : "--info"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    /* Print to visually verify coloring */
    if(data.printVisualCheck) {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, Debug::isTty(), _infoArgs, importer, time);
        Debug{} << "======================== visual color verification end =========================";
    }

    Containers::String out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, {}, _infoArgs, importer, time) == false);
    CORRADE_COMPARE_AS(out,
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterImplementationTestFiles/info-animations.txt"),
        TestSuite::Compare::StringToFile);
}

void SceneConverterImplementationTest::infoSkins() {
    auto&& data = InfoOneOrAllData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSkin2DCount() const override { return 2; }
        Containers::String doSkin2DName(UnsignedInt id) override {
            return id == 1 ? "Second 2D skin, external data" : "";
        }
        Containers::Optional<Trade::SkinData2D> doSkin2D(UnsignedInt id) override {
            /* First a regular skin, second externally owned */
            if(id == 0) return Trade::SkinData2D{
                {3, 6, 7, 12, 22},
                {{}, {}, {}, {}, {}}
            };

            if(id == 1) return Trade::SkinData2D{Trade::DataFlag::ExternallyOwned, skin2JointData, Trade::DataFlag::ExternallyOwned, skin2MatrixData};

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt doSkin3DCount() const override { return 3; }
        Containers::String doSkin3DName(UnsignedInt id) override {
            return id == 0 ? "First 3D skin, external data" : "";
        }
        Containers::Optional<Trade::SkinData3D> doSkin3D(UnsignedInt id) override {
            /* Reverse order in 3D, plus one more to ensure the count isn't
               mismatched between 2D and 3D */
            if(id == 0) return Trade::SkinData3D{Trade::DataFlag::ExternallyOwned, skin3JointData, Trade::DataFlag::ExternallyOwned, skin3MatrixData};

            if(id == 1) return Trade::SkinData3D{
                {3, 22},
                {{}, {}}
            };

            if(id == 2) return Trade::SkinData3D{
                {3},
                {{}}
            };

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt skin2JointData[15];
        Matrix3 skin2MatrixData[15];
        UnsignedInt skin3JointData[12];
        Matrix4 skin3MatrixData[12];
    } importer;

    const char* argv[]{"", data.oneOrAll ? "--info-skins" : "--info"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    /* Print to visually verify coloring */
    if(data.printVisualCheck) {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, Debug::isTty(), _infoArgs, importer, time);
        Debug{} << "======================== visual color verification end =========================";
    }

    Containers::String out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, {}, _infoArgs, importer, time) == false);
    CORRADE_COMPARE_AS(out,
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterImplementationTestFiles/info-skins.txt"),
        TestSuite::Compare::StringToFile);
}

void SceneConverterImplementationTest::infoLights() {
    auto&& data = InfoOneOrAllData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doLightCount() const override { return 2; }
        Containers::String doLightName(UnsignedInt id) override {
            return id == 1 ?  "Directional light with always-implicit attenuation and range" : "";
        }
        Containers::Optional<Trade::LightData> doLight(UnsignedInt id) override {
            /* First a blue spot light */
            if(id == 0) return Trade::LightData{
                Trade::LightType::Spot,
                0x3457ff_rgbf,
                15.0f,
                {1.2f, 0.3f, 0.04f},
                100.0f,
                55.0_degf,
                85.0_degf
            };

            /* Second a yellow directional light with infinite range */
            if(id == 1) return Trade::LightData{
                Trade::LightType::Directional,
                0xff5734_rgbf,
                5.0f
            };

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }
    } importer;

    const char* argv[]{"", data.oneOrAll ? "--info-lights" : "--info"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    /* Print to visually verify coloring */
    if(data.printVisualCheck) {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, Debug::isTty(), _infoArgs, importer, time);
        Debug{} << "======================== visual color verification end =========================";
    }

    Containers::String out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, false, _infoArgs, importer, time) == false);
    CORRADE_COMPARE_AS(out,
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterImplementationTestFiles/info-lights.txt"),
        TestSuite::Compare::StringToFile);
}

void SceneConverterImplementationTest::infoCameras() {
    auto&& data = InfoOneOrAllData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doCameraCount() const override { return 3; }
        Containers::String doCameraName(UnsignedInt id) override {
            return id == 0 ? "Orthographic 2D" : "";
        }
        Containers::Optional<Trade::CameraData> doCamera(UnsignedInt id) override {
            /* First 2D ortho camera, where near/far will get omited */
            if(id == 0) return Trade::CameraData{
                Trade::CameraType::Orthographic2D,
                {5.0f, 6.0f},
                0.0f, 0.0f
            };

            /* 3D ortho camera */
            if(id == 1) return Trade::CameraData{
                Trade::CameraType::Orthographic3D,
                {2.0f, 3.0f},
                -1.0f, 0.5f
            };

            /* Third a perspective camera, specified with size, but printed
               with FoV */
            if(id == 2) return Trade::CameraData{
                Trade::CameraType::Perspective3D,
                35.0_degf, 4.0f/3.0f, 0.01f, 100.0f
            };

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }
    } importer;

    const char* argv[]{"", data.oneOrAll ? "--info-cameras" : "--info"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    /* Print to visually verify coloring */
    if(data.printVisualCheck) {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, Debug::isTty(), _infoArgs, importer, time);
        Debug{} << "======================== visual color verification end =========================";
    }

    Containers::String out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, false, _infoArgs, importer, time) == false);
    CORRADE_COMPARE_AS(out,
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterImplementationTestFiles/info-cameras.txt"),
        TestSuite::Compare::StringToFile);
}

void SceneConverterImplementationTest::infoMaterials() {
    auto&& data = InfoOneOrAllData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMaterialCount() const override { return 2; }
        Containers::String doMaterialName(UnsignedInt id) override {
            return id == 1 ? "Lots o' laierz" : "";
        }
        Containers::Optional<Trade::MaterialData> doMaterial(UnsignedInt id) override {
            /* First has custom attributes */
            if(id == 0) return Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
                {Trade::MaterialAttribute::BaseColor, 0x3bd26799_rgbaf},
                {Trade::MaterialAttribute::DoubleSided, true},
                {Trade::MaterialAttribute::EmissiveColor, 0xe9eca_rgbf},
                {Trade::MaterialAttribute::RoughnessTexture, 67u},
                {Trade::MaterialAttribute::RoughnessTextureMatrix, Matrix3::translation({0.25f, 0.75f})},
                {Trade::MaterialAttribute::RoughnessTextureSwizzle, Trade::MaterialTextureSwizzle::B},
                {"reflectionAngle", 35.0_degf},
                /* These shouldn't have a color swatch rendered */
                {"notAColour4", Vector4{0.1f, 0.2f, 0.3f, 0.4f}},
                {"notAColour3", Vector3{0.2f, 0.3f, 0.4f}},
                {"data", Containers::ArrayView<const void>{"0123456789abcdef", 17}},
                {"deadBeef", reinterpret_cast<const void*>(std::size_t(0xdeadbeef))},
                {"undeadBeef", reinterpret_cast<void*>(std::size_t(0xbeefbeef))},
            }};

            /* Second has layers, custom layers, unnamed layers and a name */
            if(id == 1) return Trade::MaterialData{Trade::MaterialType::PbrClearCoat|Trade::MaterialType::Phong, {
                {Trade::MaterialAttribute::DiffuseColor, 0xc7cf2f99_rgbaf},
                {Trade::MaterialLayer::ClearCoat},
                {Trade::MaterialAttribute::LayerFactor, 0.5f},
                {Trade::MaterialAttribute::LayerFactorTexture, 3u},
                {Trade::MaterialAttribute::LayerName, "anEmptyLayer"},
                {Trade::MaterialAttribute::LayerFactor, 0.25f},
                {Trade::MaterialAttribute::LayerFactorTexture, 2u},
                {"yes", "a string"},
            }, {1, 4, 5, 8}};

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }
    } importer;

    const char* argv[]{"", data.oneOrAll ? "--info-materials" : "--info"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    /* Print to visually verify coloring */
    if(data.printVisualCheck) {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, Debug::isTty(), _infoArgs, importer, time);
        Debug{} << "======================== visual color verification end =========================";
    }

    Containers::String out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, false, _infoArgs, importer, time) == false);
    CORRADE_COMPARE_AS(out,
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterImplementationTestFiles/info-materials.txt"),
        TestSuite::Compare::StringToFile);
}

void SceneConverterImplementationTest::infoMeshes() {
    auto&& data = InfoOneOrAllData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMeshCount() const override { return 3; }
        UnsignedInt doMeshLevelCount(UnsignedInt id) override {
            return id == 1 ? 2 : 1;
        }
        Containers::String doMeshName(UnsignedInt id) override {
            return id == 1 ? "LODs? No, meshlets." : "";
        }
        Containers::String doMeshAttributeName(Trade::MeshAttribute name) override {
            if(name == Trade::meshAttributeCustom(25)) return "vertices";
            if(name == Trade::meshAttributeCustom(26)) return "triangles";
            /* 37 (triangleCount) deliberately not named */
            if(name == Trade::meshAttributeCustom(116)) return "vertexCount";

            return "";
        }
        Containers::Optional<Trade::MeshData> doMesh(UnsignedInt id, UnsignedInt level) override {
            /* First is indexed & externally owned */
            if(id == 0 && level == 0) return Trade::MeshData{MeshPrimitive::Points,
                Trade::DataFlag::ExternallyOwned, indices,
                Trade::MeshIndexData{indices},
                Trade::DataFlag::ExternallyOwned|Trade::DataFlag::Mutable, points, {
                    Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::stridedArrayView(points).slice(&Point::base)},
                    Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::stridedArrayView(points).slice(&Point::morphTarget), 66},
                }};

            /* Second is multi-level, with second level being indexed meshlets
               with custom (array) attributes */
            if(id == 1 && level == 0) {
                Containers::ArrayView<Vector3> positions;
                Containers::ArrayView<Vector4> tangents;
                Containers::ArrayTuple data{
                    {NoInit, 250, positions},
                    {NoInit, 250, tangents},
                };
                return Trade::MeshData{MeshPrimitive::Triangles, Utility::move(data), {
                    Trade::MeshAttributeData{Trade::MeshAttribute::Position, positions},
                    Trade::MeshAttributeData{Trade::MeshAttribute::Tangent, tangents},
                }};
            }
            if(id == 1 && level == 1) {
                Containers::StridedArrayView2D<UnsignedInt> vertices;
                Containers::StridedArrayView2D<Vector3ub> indices;
                Containers::ArrayView<UnsignedByte> triangleCount;
                Containers::ArrayView<UnsignedByte> vertexCount;
                Containers::ArrayTuple data{
                    {NoInit, {135, 64}, vertices},
                    {NoInit, {135, 126}, indices},
                    {NoInit, 135, triangleCount},
                    {NoInit, 135, vertexCount},
                };
                return Trade::MeshData{MeshPrimitive::Meshlets, Utility::move(data), {
                    Trade::MeshAttributeData{Trade::meshAttributeCustom(25), vertices},
                    Trade::MeshAttributeData{Trade::meshAttributeCustom(26), indices},
                    Trade::MeshAttributeData{Trade::meshAttributeCustom(37), triangleCount},
                    Trade::MeshAttributeData{Trade::meshAttributeCustom(116), vertexCount},
                }};
            }

            /* Third is an empty instance mesh */
            if(id == 2 && level == 0) return Trade::MeshData{MeshPrimitive::Instances, 15};

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedShort indices[70];
        struct Point {
            Vector3 base;
            Vector2 morphTarget;
        } points[50];
    } importer;

    const char* argv[]{"", data.oneOrAll ? "--info-meshes" : "--info"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    /* Print to visually verify coloring */
    if(data.printVisualCheck) {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, Debug::isTty(), _infoArgs, importer, time);
        Debug{} << "======================== visual color verification end =========================";
    }

    Containers::String out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, false, _infoArgs, importer, time) == false);
    CORRADE_COMPARE_AS(out,
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterImplementationTestFiles/info-meshes.txt"),
        TestSuite::Compare::StringToFile);
}

void SceneConverterImplementationTest::infoMeshesBounds() {
    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMeshCount() const override { return 1; }
        Containers::Optional<Trade::MeshData> doMesh(UnsignedInt, UnsignedInt) override {
            return Trade::MeshData{MeshPrimitive::Lines,
                {}, indexData, Trade::MeshIndexData{indexData},
                {}, vertexData, {
                    Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(vertexData->positions)},
                    Trade::MeshAttributeData{Trade::MeshAttribute::Tangent, Containers::arrayView(vertexData->tangent)},
                    Trade::MeshAttributeData{Trade::MeshAttribute::Bitangent, Containers::arrayView(vertexData->bitangent)},
                    Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId, Containers::arrayView(vertexData->objectId)},
                    Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3bNormalized, Containers::arrayView(vertexData->normal)},
                    /* This has the same data as Normal, but it won't have its
                       bounds printed as it's custom -- there it's unknown what
                       the canonical type should be and printing a range of
                       an arbitrary packed type is useless in most cases */
                    Trade::MeshAttributeData{Trade::meshAttributeCustom(25), Containers::arrayView(vertexData->normal)},
                    Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, Containers::arrayView(vertexData->textureCoordinates)},
                    Trade::MeshAttributeData{Trade::MeshAttribute::Color, Containers::arrayView(vertexData->color)},
                    Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId, Containers::arrayView(vertexData->objectIdSecondary)},
                    Trade::MeshAttributeData{Trade::MeshAttribute::JointIds, VertexFormat::UnsignedByte, Containers::arrayView(vertexData->jointIds), 3},
                    Trade::MeshAttributeData{Trade::MeshAttribute::Weights, VertexFormat::Half, Containers::arrayView(vertexData->weights), 3},
                }};
        }

        Containers::String doMeshAttributeName(Trade::MeshAttribute name) override {
            if(name == Trade::meshAttributeCustom(25))
                return "normalButCustomSoNoBoundsPrinted";
            return "";
        }

        UnsignedByte indexData[3]{15, 3, 176};

        struct {
            Vector3 positions[2];
            Vector3 tangent[2];
            Vector3 bitangent[2];
            UnsignedShort objectId[2];
            Vector3b normal[2];
            Vector2 textureCoordinates[2];
            Vector4 color[2];
            UnsignedInt objectIdSecondary[2];
            Vector3ub jointIds[2];
            Vector3h weights[2];
        } vertexData[1]{{
            {{0.1f, -0.1f, 0.2f}, {0.2f, 0.0f, -0.2f}},
            {{0.2f, -0.2f, 0.8f}, {0.3f, 0.8f, 0.2f}},
            {{0.4f, 0.2f, 1.0f}, {0.3f, 0.9f, 0.0f}},
            {155, 12},
            {{0, 127, 0}, {-127, 0, 127}},
            {{0.5f, 0.5f}, {1.5f, 0.5f}},
            {0x99336600_rgbaf, 0xff663333_rgbaf},
            {15, 337},
            {{22, 6, 27}, {15, 12, 23}},
            {{0.5_h, 0.25_h, 0.25_h}, {0.125_h, 0.75_h, 0.125_h}}
        }};
    } importer;

    const char* argv[]{"", "--info-meshes", "--bounds"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    /* Print to visually verify coloring */
    {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, Debug::isTty(), _infoArgs, importer, time);
        Debug{} << "======================== visual color verification end =========================";
    }

    Containers::String out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, false, _infoArgs, importer, time) == false);
    CORRADE_COMPARE_AS(out,
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterImplementationTestFiles/info-meshes-bounds.txt"),
        TestSuite::Compare::StringToFile);
}

void SceneConverterImplementationTest::infoTextures() {
    auto&& data = InfoOneOrAllData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doTextureCount() const override { return 2; }
        Containers::String doTextureName(UnsignedInt id) override {
            return id == 1 ? "Name!" : "";
        }
        Containers::Optional<Trade::TextureData> doTexture(UnsignedInt id) override {
            /* First a 1D texture */
            if(id == 0) return Trade::TextureData{
                Trade::TextureType::Texture1D,
                SamplerFilter::Nearest,
                SamplerFilter::Linear,
                SamplerMipmap::Nearest,
                SamplerWrapping::Repeat,
                666
            };

            /* Second a 2D array texture */
            if(id == 1) return Trade::TextureData{
                Trade::TextureType::Texture2DArray,
                SamplerFilter::Linear,
                SamplerFilter::Nearest,
                SamplerMipmap::Linear,
                {SamplerWrapping::MirroredRepeat, SamplerWrapping::ClampToEdge, SamplerWrapping::MirrorClampToEdge},
                3
            };

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }
    } importer;

    const char* argv[]{"", data.oneOrAll ? "--info-textures" : "--info"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    /* Print to visually verify coloring */
    if(data.printVisualCheck) {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, Debug::isTty(), _infoArgs, importer, time);
        Debug{} << "======================== visual color verification end =========================";
    }

    Containers::String out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, false, _infoArgs, importer, time) == false);
    CORRADE_COMPARE_AS(out,
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterImplementationTestFiles/info-textures.txt"),
        TestSuite::Compare::StringToFile);
}

void SceneConverterImplementationTest::infoImages() {
    auto&& data = InfoOneOrAllData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Just the very basics to ensure image info *is* printed. Tested in full
       in ImageConverterTest. */
    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage1DCount() const override { return 1; }
        Containers::Optional<Trade::ImageData1D> doImage1D(UnsignedInt, UnsignedInt) override {
            return Trade::ImageData1D{PixelFormat::R32F, 1024, Containers::Array<char>{NoInit, 4096}};
        }
    } importer;

    const char* argv[]{"", data.oneOrAll ? "--info-images" : "--info"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    /* Print to visually verify coloring */
    if(data.printVisualCheck) {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, Debug::isTty(), _infoArgs, importer, time);
        Debug{} << "======================== visual color verification end =========================";
    }

    Containers::String out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, false, _infoArgs, importer, time) == false);
    CORRADE_COMPARE_AS(out,
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterImplementationTestFiles/info-images.txt"),
        TestSuite::Compare::StringToFile);
}

void SceneConverterImplementationTest::infoReferenceCount() {
    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        /* One data of each kind should be always referenced twice+, one once,
           one not at all, and one reference should be OOB */

        UnsignedLong doObjectCount() const override { return 4; }
        Containers::String doObjectName(UnsignedLong id) override {
            return id == 2 ? "Not referenced" : "";
        }
        UnsignedInt doSceneCount() const override { return 2; }
        Containers::Optional<Trade::SceneData> doScene(UnsignedInt id) override {
            if(id == 0) return Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 2, {}, sceneData3D, {
                /* To mark the scene as 3D */
                Trade::SceneFieldData{Trade::SceneField::Transformation, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Matrix4x4, nullptr},
                Trade::SceneFieldData{Trade::SceneField::Mesh,
                    Containers::arrayView(sceneData3D->mapping),
                    Containers::arrayView(sceneData3D->meshes)},
                Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
                    Containers::arrayView(sceneData3D->mapping),
                    Containers::arrayView(sceneData3D->materials)},
                Trade::SceneFieldData{Trade::SceneField::Light,
                    Containers::arrayView(sceneData3D->mapping),
                    Containers::arrayView(sceneData3D->lights)},
                Trade::SceneFieldData{Trade::SceneField::Camera,
                    Containers::arrayView(sceneData3D->mapping),
                    Containers::arrayView(sceneData3D->cameras)},
                Trade::SceneFieldData{Trade::SceneField::Skin,
                    Containers::arrayView(sceneData3D->mapping),
                    Containers::arrayView(sceneData3D->skins)},
            }};
            if(id == 1) return Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 4, {}, sceneData2D, {
                /* To mark the scene as 2D */
                Trade::SceneFieldData{Trade::SceneField::Transformation, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Matrix3x3, nullptr},
                Trade::SceneFieldData{Trade::SceneField::Mesh,
                    Containers::arrayView(sceneData2D->mapping),
                    Containers::arrayView(sceneData2D->meshes)},
                Trade::SceneFieldData{Trade::SceneField::Skin,
                    Containers::arrayView(sceneData2D->mapping),
                    Containers::arrayView(sceneData2D->skins)},
            }};

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt doSkin2DCount() const override { return 3; }
        Containers::String doSkin2DName(UnsignedInt id) override {
            return id == 2 ? "Not referenced" : "";
        }
        Containers::Optional<Trade::SkinData2D> doSkin2D(UnsignedInt id) override {
            if(id == 0) return Trade::SkinData2D{
                {35, 22},
                {{}, {}}
            };
            if(id == 1) return Trade::SkinData2D{
                {33, 10, 100},
                {{}, {}, {}}
            };
            if(id == 2) return Trade::SkinData2D{
                {66},
                {{}}
            };

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt doSkin3DCount() const override { return 3; }
        Containers::String doSkin3DName(UnsignedInt id) override {
            return id == 0 ? "Not referenced" : "";
        }
        Containers::Optional<Trade::SkinData3D> doSkin3D(UnsignedInt id) override {
            if(id == 0) return Trade::SkinData3D{
                {35, 22},
                {{}, {}}
            };
            if(id == 1) return Trade::SkinData3D{
                {37},
                {{}}
            };
            if(id == 2) return Trade::SkinData3D{
                {300, 10, 1000},
                {{}, {}, {}}
            };

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt doLightCount() const override { return 3; }
        Containers::String doLightName(UnsignedInt id) override {
            return id == 1 ? "Not referenced" : "";
        }
        Containers::Optional<Trade::LightData> doLight(UnsignedInt id) override {
            if(id == 0) return Trade::LightData{
                Trade::LightType::Directional,
                0x57ff34_rgbf,
                5.0f
            };
            if(id == 1) return Trade::LightData{
                Trade::LightType::Ambient,
                0xff5734_rgbf,
                0.1f
            };
            if(id == 2) return Trade::LightData{
                Trade::LightType::Directional,
                0x3457ff_rgbf,
                1.0f
            };
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt doCameraCount() const override { return 3; }
        Containers::String doCameraName(UnsignedInt id) override {
            return id == 0 ? "Not referenced" : "";
        }
        Containers::Optional<Trade::CameraData> doCamera(UnsignedInt id) override {
            if(id == 0) return Trade::CameraData{
                Trade::CameraType::Orthographic3D,
                {2.0f, 3.0f},
                -1.0f, 0.5f
            };
            if(id == 1) return Trade::CameraData{
                Trade::CameraType::Orthographic3D,
                {2.0f, 2.0f},
                0.0f, 1.0f
            };
            if(id == 2) return Trade::CameraData{
                Trade::CameraType::Orthographic2D,
                {2.0f, 2.0f},
                0.0f, 0.0f
            };
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt doMaterialCount() const override { return 3; }
        Containers::String doMaterialName(UnsignedInt id) override {
            return id == 2 ? "Not referenced" : "";
        }
        Containers::Optional<Trade::MaterialData> doMaterial(UnsignedInt id) override {
            if(id == 0) return Trade::MaterialData{{}, {
                {Trade::MaterialAttribute::DiffuseTexture, 2u},
                {Trade::MaterialAttribute::BaseColorTexture, 2u},
            }};
            if(id == 1) return Trade::MaterialData{{}, {
                {"lookupTexture", 0u},
                {"volumeTexture", 3u},
                {Trade::MaterialAttribute::NormalTexture, 17u},
                {Trade::MaterialAttribute::EmissiveTexture, 4u},
            }};
            if(id == 2) return Trade::MaterialData{{}, {}};
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt doMeshCount() const override { return 3; }
        Containers::String doMeshName(UnsignedInt id) override {
            return id == 1 ? "Not referenced" : "";
        }
        Containers::Optional<Trade::MeshData> doMesh(UnsignedInt id, UnsignedInt) override {
            if(id == 0) return Trade::MeshData{MeshPrimitive::Points, 5};
            if(id == 1) return Trade::MeshData{MeshPrimitive::Lines, 4};
            if(id == 2) return Trade::MeshData{MeshPrimitive::TriangleFan, 4};
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt doTextureCount() const override { return 5; }
        Containers::String doTextureName(UnsignedInt id) override {
            return id == 1 ? "Not referenced" : "";
        }
        Containers::Optional<Trade::TextureData> doTexture(UnsignedInt id) override {
            if(id == 0) return Trade::TextureData{
                Trade::TextureType::Texture1D,
                SamplerFilter::Nearest,
                SamplerFilter::Linear,
                SamplerMipmap::Nearest,
                SamplerWrapping::Repeat,
                1
            };
            if(id == 1) return Trade::TextureData{
                Trade::TextureType::Texture1DArray,
                SamplerFilter::Nearest,
                SamplerFilter::Linear,
                SamplerMipmap::Nearest,
                SamplerWrapping::Repeat,
                225
            };
            if(id == 2) return Trade::TextureData{
                Trade::TextureType::Texture2D,
                SamplerFilter::Nearest,
                SamplerFilter::Linear,
                SamplerMipmap::Nearest,
                SamplerWrapping::Repeat,
                0
            };
            if(id == 3) return Trade::TextureData{
                Trade::TextureType::Texture3D,
                SamplerFilter::Nearest,
                SamplerFilter::Linear,
                SamplerMipmap::Nearest,
                SamplerWrapping::Repeat,
                1
            };
            if(id == 4) return Trade::TextureData{
                Trade::TextureType::Texture2D,
                SamplerFilter::Nearest,
                SamplerFilter::Linear,
                SamplerMipmap::Nearest,
                SamplerWrapping::Repeat,
                0
            };
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt doImage1DCount() const override { return 2; }
        Containers::String doImage1DName(UnsignedInt id) override {
            return id == 0 ? "Not referenced" : "";
        }
        Containers::Optional<Trade::ImageData1D> doImage1D(UnsignedInt id, UnsignedInt) override {
            if(id == 0)
                return Trade::ImageData1D{PixelFormat::RGBA8I, 1, Containers::Array<char>{NoInit, 4}};
            if(id == 1)
                return Trade::ImageData1D{PixelFormat::R8I, 4, Containers::Array<char>{NoInit, 4}};
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt doImage2DCount() const override { return 2; }
        Containers::String doImage2DName(UnsignedInt id) override {
            return id == 1 ? "Not referenced" : "";
        }
        Containers::Optional<Trade::ImageData2D> doImage2D(UnsignedInt id, UnsignedInt) override {
            if(id == 0)
                return Trade::ImageData2D{PixelFormat::RGBA8I, {1, 2}, Containers::Array<char>{NoInit, 8}};
            if(id == 1)
                return Trade::ImageData2D{PixelFormat::R8I, {4, 1}, Containers::Array<char>{NoInit, 4}};
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt doImage3DCount() const override { return 2; }
        Containers::String doImage3DName(UnsignedInt id) override {
            return id == 0 ? "Not referenced" : "";
        }
        Containers::Optional<Trade::ImageData3D> doImage3D(UnsignedInt id, UnsignedInt) override {
            if(id == 0)
                return Trade::ImageData3D{PixelFormat::RGBA8I, {1, 2, 1}, Containers::Array<char>{NoInit, 8}};
            if(id == 1)
                return Trade::ImageData3D{PixelFormat::R8I, {4, 1, 1}, Containers::Array<char>{NoInit, 4}};
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        struct {
            UnsignedInt mapping[4];
            UnsignedInt meshes[4];
            Int materials[4];
            UnsignedInt lights[4];
            UnsignedInt cameras[4];
            UnsignedInt skins[4];
        } sceneData3D[1]{{
            {0, 1, 1, 25},
            {2, 0, 2, 67},
            {0, 1, 23, 0},
            {0, 17, 0, 2},
            {166, 1, 2, 1},
            {1, 1, 22, 2}
        }};

        struct {
            UnsignedInt mapping[3];
            UnsignedInt meshes[3];
            UnsignedInt skins[3];
        } sceneData2D[1]{{
            {3, 116, 1},
            {2, 0, 23},
            {177, 0, 1}
        }};
    } importer;

    const char* argv[]{"", "--info"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    /* Print to visually verify coloring */
    {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, Debug::isTty(), _infoArgs, importer, time);
        Debug{} << "======================== visual color verification end =========================";
    }

    Containers::String out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, false, _infoArgs, importer, time) == false);
    CORRADE_COMPARE_AS(out,
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterImplementationTestFiles/info-references.txt"),
        TestSuite::Compare::StringToFile);
}

void SceneConverterImplementationTest::infoError() {
    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        /* The one single object is named, and that name should be printed
           after all error messages */
        UnsignedLong doObjectCount() const override { return 1; }
        Containers::String doObjectName(UnsignedLong) override { return "A name"; }

        UnsignedInt doSceneCount() const override { return 2; }
        Containers::Optional<Trade::SceneData> doScene(UnsignedInt id) override {
            Error{} << "Scene" << id << "error!";
            return {};
        }

        Int doDefaultScene() const override { return 1; }

        UnsignedInt doAnimationCount() const override { return 2; }
        Containers::Optional<Trade::AnimationData> doAnimation(UnsignedInt id) override {
            Error{} << "Animation" << id << "error!";
            return {};
        }

        UnsignedInt doSkin2DCount() const override { return 2; }
        Containers::Optional<Trade::SkinData2D> doSkin2D(UnsignedInt id) override {
            Error{} << "2D skin" << id << "error!";
            return {};
        }

        UnsignedInt doSkin3DCount() const override { return 2; }
        Containers::Optional<Trade::SkinData3D> doSkin3D(UnsignedInt id) override {
            Error{} << "3D skin" << id << "error!";
            return {};
        }

        UnsignedInt doLightCount() const override { return 2; }
        Containers::Optional<Trade::LightData> doLight(UnsignedInt id) override {
            Error{} << "Light" << id << "error!";
            return {};
        }

        UnsignedInt doCameraCount() const override { return 2; }
        Containers::Optional<Trade::CameraData> doCamera(UnsignedInt id) override {
            Error{} << "Camera" << id << "error!";
            return {};
        }

        UnsignedInt doMaterialCount() const override { return 2; }
        Containers::Optional<Trade::MaterialData> doMaterial(UnsignedInt id) override {
            Error{} << "Material" << id << "error!";
            return {};
        }

        UnsignedInt doMeshCount() const override { return 2; }
        Containers::Optional<Trade::MeshData> doMesh(UnsignedInt id, UnsignedInt) override {
            Error{} << "Mesh" << id << "error!";
            return {};
        }

        UnsignedInt doTextureCount() const override { return 2; }
        Containers::Optional<Trade::TextureData> doTexture(UnsignedInt id) override {
            Error{} << "Texture" << id << "error!";
            return {};
        }

        /* Errors for all image types tested in ImageConverterTest */
        UnsignedInt doImage2DCount() const override { return 2; }
        Containers::Optional<Trade::ImageData2D> doImage2D(UnsignedInt id, UnsignedInt) override {
            Error{} << "Image" << id << "error!";
            return {};
        }
    } importer;

    const char* argv[]{"", "--info"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    Containers::String out;
    Debug redirectOutput{&out};
    Error redirectError{&out};
    /* It should return a failure */
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, {}, _infoArgs, importer, time) == true);
    CORRADE_COMPARE(out,
        /* It should not exit after first error... */
        "Scene 0 error!\n"
        "Can't import scene 0\n"
        "Scene 1 error!\n"
        "Can't import scene 1\n"
        "Animation 0 error!\n"
        "Can't import animation 0\n"
        "Animation 1 error!\n"
        "Can't import animation 1\n"
        "2D skin 0 error!\n"
        "Can't import 2D skin 0\n"
        "2D skin 1 error!\n"
        "Can't import 2D skin 1\n"
        "3D skin 0 error!\n"
        "Can't import 3D skin 0\n"
        "3D skin 1 error!\n"
        "Can't import 3D skin 1\n"
        "Light 0 error!\n"
        "Can't import light 0\n"
        "Light 1 error!\n"
        "Can't import light 1\n"
        "Camera 0 error!\n"
        "Can't import camera 0\n"
        "Camera 1 error!\n"
        "Can't import camera 1\n"
        "Material 0 error!\n"
        "Can't import material 0\n"
        "Material 1 error!\n"
        "Can't import material 1\n"
        "Mesh 0 error!\n"
        "Can't import mesh 0 level 0\n"
        "Mesh 1 error!\n"
        "Can't import mesh 1 level 0\n"
        "Texture 0 error!\n"
        "Can't import texture 0\n"
        "Texture 1 error!\n"
        "Can't import texture 1\n"
        "Image 0 error!\n"
        "Can't import 2D image 0 level 0\n"
        "Image 1 error!\n"
        "Can't import 2D image 1 level 0\n"
        /* It should print the default scene even if all of them failed to
           import */
        "Default scene: 1\n"
        /* ... and it should print all info output after the errors */
        "Object 0: A name\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneTools::Test::SceneConverterImplementationTest)
