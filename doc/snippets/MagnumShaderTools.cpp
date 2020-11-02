/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include <unordered_map>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/FileCallback.h"
#include "Magnum/ShaderTools/AbstractConverter.h"

#define DOXYGEN_IGNORE(...) __VA_ARGS__

using namespace Magnum;

int main() {
{
/* [AbstractConverter-usage-validation] */
PluginManager::Manager<ShaderTools::AbstractConverter> manager;
Containers::Pointer<ShaderTools::AbstractConverter> converter =
    manager.loadAndInstantiate("AnyShaderConverter");

bool valid;
Containers::String message;
if(converter) std::tie(valid, message) =
    converter->validateFile(ShaderTools::Stage::Unspecified, "file.spv");
if(!converter || !valid)
    Error{} << "Validation failed:" << message;
else if(!message.isEmpty())
    Warning{} << "Validation succeeded with warnings:" << message;
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
Containers::Array<char> spirv = converter->convertDataToData(
    ShaderTools::Stage::Fragment, glsl);
/* [AbstractConverter-usage-compilation] */
}

{
Containers::Pointer<ShaderTools::AbstractConverter> converter;
Containers::Array<char> extract(const std::string&, const std::string&);
/* [AbstractConverter-usage-callbacks] */
struct Data {
    std::unordered_map<std::string, Containers::Array<char>> files;
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

        /* Extract from an archive if not there yet; fail if not extraction
           failed */
        if(found == data.files.end()) {
            Containers::Array<char> file = extract("shaders.zip", filename);
            if(!file) return {};

            found = data.files.emplace(filename, std::move(file)).first;
        }

        return Containers::ArrayView<const char>{found->second};
    }, data);

/* extracted from a ZIP */
auto result = converter->validateFile(ShaderTools::Stage::Fragment, "ssao.frag");
/* [AbstractConverter-usage-callbacks] */
}

{
Containers::Pointer<ShaderTools::AbstractConverter> converter;
/* [AbstractConverter-setInputFileCallback] */
converter->setInputFileCallback([](const std::string& filename,
    InputFileCallbackPolicy, void*) {
        Utility::Resource rs("data");
        return Containers::optional(rs.getRaw(filename));
    });
/* [AbstractConverter-setInputFileCallback] */
}

{
Containers::Pointer<ShaderTools::AbstractConverter> converter;
/* [AbstractConverter-setInputFileCallback-template] */
const Utility::Resource rs{"data"};
converter->setInputFileCallback([](const std::string& filename,
    InputFileCallbackPolicy, const Utility::Resource& rs) {
        return Containers::optional(rs.getRaw(filename));
    }, rs);
/* [AbstractConverter-setInputFileCallback-template] */
}

}
