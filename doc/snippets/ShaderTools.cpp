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

/* In order to have the CORRADE_PLUGIN_REGISTER() macro not a no-op. Doesn't
   affect anything else. */
#define CORRADE_STATIC_PLUGIN

#include <string> /** @todo drop when file callbacks are <string>-free */
#include <unordered_map>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringStl.h> /** @todo drop when file callbacks are <string>-free */
#include <Corrade/Utility/Macros.h> /* CORRADE_LINE_STRING */
#include <Corrade/Utility/Resource.h>

#include "Magnum/FileCallback.h"
#include "Magnum/ShaderTools/AbstractConverter.h"
#include "Magnum/ShaderTools/Stage.h"

#define DOXYGEN_ELLIPSIS(...) __VA_ARGS__

using namespace Magnum;

namespace MyNamespace {

struct MyShaderConverter: ShaderTools::AbstractConverter {
    explicit MyShaderConverter(PluginManager::AbstractManager& manager, Containers::StringView plugin): ShaderTools::AbstractConverter{manager, plugin} {}

    ShaderTools::ConverterFeatures doFeatures() const override { return {}; }
    void doSetInputFormat(ShaderTools::Format, Containers::StringView) override {}
    void doSetOutputFormat(ShaderTools::Format, Containers::StringView) override {}
};

}

/* [MAGNUM_SHADERTOOLS_ABSTRACTCONVERTER_PLUGIN_INTERFACE] */
CORRADE_PLUGIN_REGISTER(MyShaderConverter, MyNamespace::MyShaderConverter,
    MAGNUM_SHADERTOOLS_ABSTRACTCONVERTER_PLUGIN_INTERFACE)
/* [MAGNUM_SHADERTOOLS_ABSTRACTCONVERTER_PLUGIN_INTERFACE] */

/* Make sure the name doesn't conflict with any other snippets to avoid linker
   warnings, unlike with `int main()` there now has to be a declaration to
   avoid -Wmisssing-prototypes */
void mainShaderTools();
void mainShaderTools() {
{
/* [AbstractConverter-usage-validation] */
PluginManager::Manager<ShaderTools::AbstractConverter> manager;
Containers::Pointer<ShaderTools::AbstractConverter> converter =
    manager.loadAndInstantiate("AnyShaderConverter");

Containers::Pair<bool, Containers::String> validMessage;
if(converter) validMessage =
    converter->validateFile(ShaderTools::Stage::Unspecified, "file.spv");
if(!converter || !validMessage.first())
    Error{} << "Validation failed:" << validMessage.second();
else if(!validMessage.second().isEmpty())
    Warning{} << "Validation succeeded with warnings:" << validMessage.second();
else
    Debug{} << "Validation passed";
/* [AbstractConverter-usage-validation] */
}

{
PluginManager::Manager<ShaderTools::AbstractConverter> manager;
/* [AbstractConverter-usage-compilation] */
Containers::Pointer<ShaderTools::AbstractConverter> converter =
    manager.loadAndInstantiate("GlslToSpirvShaderConverter");

/* Using CORRADE_LINE_STRING will make the compiler report line info that
   matches the source */
Containers::StringView glsl = "#line " CORRADE_LINE_STRING "\n" R"GLSL(
#version 450 core

layout(binding=0) uniform Material {
    vec4 color;
};

#ifdef TEXTURED
layout(binding=1) uniform sampler2D colorTexture;
layout(location=0) in vec2 textureCoordinates;
#endif

layout(location=0) out vec4 fragmentColor;

void main() {
    fragmentColor = color
        #ifdef TEXTURED
        *texture(colorTexture, textureCoordinates)
        #endif
        ;
}
)GLSL";

converter->setDefinitions({
    {"TEXTURED", ""}
});
Containers::Optional<Containers::Array<char>> spirv =
    converter->convertDataToData(ShaderTools::Stage::Fragment, glsl);
/* [AbstractConverter-usage-compilation] */
}

{
/* -Wnonnull in GCC 11+  "helpfully" says "this is null" if I don't initialize
   the converter pointer. I don't care, I just want you to check compilation
   errors, not more! */
PluginManager::Manager<ShaderTools::AbstractConverter> manager;
Containers::Pointer<ShaderTools::AbstractConverter> converter = manager.loadAndInstantiate("SomethingWhatever");
Containers::Array<char> extract(const std::string&, const std::string&);
/* [AbstractConverter-usage-callbacks] */
struct Data {
    std::unordered_map<std::string, Containers::Optional<
        Containers::Array<char>>> files;
} data;

converter->setInputFileCallback([](const std::string& filename,
    InputFileCallbackPolicy policy, Data& data)
        -> Containers::Optional<Containers::ArrayView<const char>>
    {
        auto found = data.files.find(filename);

        /* Discard the loaded file, if not needed anymore */
        if(policy == InputFileCallbackPolicy::Close) {
            if(found != data.files.end()) data.files.erase(found);
            return {};
        }

        /* Extract from an archive if not there yet. If the extraction fails,
           remember that to not attempt to extract the same file again next
           time. */
        if(found == data.files.end()) found = data.files.emplace(
            filename, extract("shaders.zip", filename)).first;

        if(!found->second) return {};
        return Containers::ArrayView<const char>{*found->second};
    }, data);

/* extracted from a ZIP */
auto result = converter->validateFile(ShaderTools::Stage::Fragment, "ssao.frag");
/* [AbstractConverter-usage-callbacks] */
}

{
/* -Wnonnull in GCC 11+  "helpfully" says "this is null" if I don't initialize
   the converter pointer. I don't care, I just want you to check compilation
   errors, not more! */
PluginManager::Manager<ShaderTools::AbstractConverter> manager;
Containers::Pointer<ShaderTools::AbstractConverter> converter = manager.loadAndInstantiate("SomethingWhatever");
/* [AbstractConverter-setInputFileCallback] */
converter->setInputFileCallback([](const std::string& filename,
    InputFileCallbackPolicy, void*) {
        Utility::Resource rs("data");
        return Containers::optional(rs.getRaw(filename));
    });
/* [AbstractConverter-setInputFileCallback] */
}

{
/* -Wnonnull in GCC 11+  "helpfully" says "this is null" if I don't initialize
   the converter pointer. I don't care, I just want you to check compilation
   errors, not more! */
PluginManager::Manager<ShaderTools::AbstractConverter> manager;
Containers::Pointer<ShaderTools::AbstractConverter> converter = manager.loadAndInstantiate("SomethingWhatever");
/* [AbstractConverter-setInputFileCallback-template] */
const Utility::Resource rs{"data"};
converter->setInputFileCallback([](const std::string& filename,
    InputFileCallbackPolicy, const Utility::Resource& rs) {
        return Containers::optional(rs.getRaw(filename));
    }, rs);
/* [AbstractConverter-setInputFileCallback-template] */
}

}
