/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Directory.h>

#ifdef CORRADE_TARGET_APPLE
#include <Magnum/Platform/WindowlessCglApplication.h>
#elif defined(CORRADE_TARGET_UNIX)
#include <Magnum/Platform/WindowlessGlxApplication.h>
#elif defined(CORRADE_TARGET_WINDOWS)
#include <Magnum/Platform/WindowlessWglApplication.h>
#else
#error no windowless application available on this platform
#endif

#include <Magnum/Buffer.h>
#include <Magnum/Framebuffer.h>
#include <Magnum/Image.h>
#include <Magnum/Mesh.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/Renderbuffer.h>
#include <Magnum/RenderbufferFormat.h>
#include <Magnum/Renderer.h>
#include <Magnum/Texture.h>
#include <Magnum/TextureFormat.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/Primitives/Square.h>
#include <Magnum/Primitives/Circle.h>
#include <Magnum/Primitives/Icosphere.h>
#include <Magnum/Primitives/UVSphere.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Shaders/MeshVisualizer.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Shaders/VertexColor.h>
#include <Magnum/Shaders/Vector.h>
#include <Magnum/Shaders/DistanceFieldVector.h>
#include <Magnum/Trade/AbstractImageConverter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshData2D.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/Trade/AbstractImporter.h>

#include "configure.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

struct ShaderVisualizer: Platform::WindowlessApplication {
    using Platform::WindowlessApplication::WindowlessApplication;

    int exec() override;

    std::string phong();
    std::string meshVisualizer();
    std::string flat();
    std::string vertexColor();

    std::string vector();
    std::string distanceFieldVector();

    std::unique_ptr<Trade::AbstractImporter> _importer;
};

namespace {
    constexpr const Vector2i ImageSize{256};
}

int ShaderVisualizer::exec() {
    PluginManager::Manager<Trade::AbstractImageConverter> converterManager{MAGNUM_PLUGINS_IMAGECONVERTER_DIR};
    std::unique_ptr<Trade::AbstractImageConverter> converter = converterManager.loadAndInstantiate("PngImageConverter");
    if(!converter) {
        Error() << "Cannot load image converter plugin";
        std::exit(1);
    }

    PluginManager::Manager<Trade::AbstractImporter> importerManager{MAGNUM_PLUGINS_IMPORTER_DIR};
    _importer = importerManager.loadAndInstantiate("PngImporter");
    if(!_importer) {
        Error() << "Cannot load image importer plugin";
        std::exit(1);
    }

    Renderbuffer multisampleColor, multisampleDepth;
    multisampleColor.setStorageMultisample(16, RenderbufferFormat::RGBA8, ImageSize);
    multisampleDepth.setStorageMultisample(16, RenderbufferFormat::DepthComponent24, ImageSize);

    Framebuffer multisampleFramebuffer{{{}, ImageSize}};
    multisampleFramebuffer.attachRenderbuffer(Framebuffer::ColorAttachment{0}, multisampleColor)
        .attachRenderbuffer(Framebuffer::BufferAttachment::Depth, multisampleDepth)
        .bind();
    CORRADE_INTERNAL_ASSERT(multisampleFramebuffer.checkStatus(FramebufferTarget::Draw) == Framebuffer::Status::Complete);

    Renderbuffer color;
    color.setStorage(RenderbufferFormat::RGBA8, ImageSize);
    Framebuffer framebuffer{{{}, ImageSize}};
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment{0}, color);

    Renderer::enable(Renderer::Feature::DepthTest);

    for(auto fun: {&ShaderVisualizer::phong,
                   &ShaderVisualizer::meshVisualizer,
                   &ShaderVisualizer::flat,
                   &ShaderVisualizer::vertexColor,
                   &ShaderVisualizer::vector,
                   &ShaderVisualizer::distanceFieldVector}) {
        multisampleFramebuffer.clear(FramebufferClear::Color|FramebufferClear::Depth);

        std::string filename = (this->*fun)();

        AbstractFramebuffer::blit(multisampleFramebuffer, framebuffer, framebuffer.viewport(), FramebufferBlit::Color);
        Image2D result = framebuffer.read(framebuffer.viewport(), {PixelFormat::RGBA, PixelType::UnsignedByte});
        converter->exportToFile(result, Utility::Directory::join("../", "shaders-" + filename));
    }

    _importer.reset();

    return 0;
}

namespace {
    const auto Projection = Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.001f, 100.0f);
    const auto Transformation = Matrix4::translation(Vector3::zAxis(-5.0f));
    const auto BaseColor = Color3::fromHSV(216.0_degf, 0.85f, 1.0f);
    const auto OutlineColor = Color3{0.95f};
}

std::string ShaderVisualizer::phong() {
    std::unique_ptr<Buffer> vertices, indices;
    Mesh mesh{NoCreate};
    std::tie(mesh, vertices, indices) = MeshTools::compile(Primitives::UVSphere::solid(16, 32), BufferUsage::StaticDraw);

    Shaders::Phong shader;
    shader.setAmbientColor(Color3(0.025f))
        .setDiffuseColor(BaseColor)
        .setShininess(200.0f)
        .setLightPosition({5.0f, 5.0f, 7.0f})
        .setProjectionMatrix(Projection)
        .setTransformationMatrix(Transformation)
        .setNormalMatrix(Transformation.rotationScaling());

    mesh.draw(shader);

    return "phong.png";
}

std::string ShaderVisualizer::meshVisualizer() {
    std::unique_ptr<Buffer> vertices, indices;
    Mesh mesh{NoCreate};
    std::tie(mesh, vertices, indices) = MeshTools::compile(Primitives::Icosphere::solid(1), BufferUsage::StaticDraw);

    const Matrix4 projection = Projection*Transformation*
        Matrix4::rotationZ(13.7_degf)*
        Matrix4::rotationX(-12.6_degf);

    Shaders::MeshVisualizer shader{Shaders::MeshVisualizer::Flag::Wireframe};
    shader.setColor(BaseColor)
        .setWireframeColor(OutlineColor)
        .setViewportSize(Vector2{ImageSize})
        .setTransformationProjectionMatrix(projection);

    mesh.draw(shader);

    return "meshvisualizer.png";
}

std::string ShaderVisualizer::flat() {
    std::unique_ptr<Buffer> vertices, indices;
    Mesh mesh{NoCreate};
    std::tie(mesh, vertices, indices) = MeshTools::compile(Primitives::UVSphere::solid(16, 32), BufferUsage::StaticDraw);

    Shaders::Flat3D shader;
    shader.setColor(BaseColor)
        .setTransformationProjectionMatrix(Projection*Transformation);

    mesh.draw(shader);

    return "flat.png";
}

std::string ShaderVisualizer::vertexColor() {
    Trade::MeshData3D sphere = Primitives::UVSphere::solid(32, 64);

    /* Color vertices nearest to given position */
    auto target = Vector3{2.0f, 2.0f, 7.0f}.normalized();
    std::vector<Color3> colors;
    colors.reserve(sphere.positions(0).size());
    for(Vector3 position: sphere.positions(0))
        colors.push_back(Color3::fromHSV(Math::lerp(240.0_degf, 420.0_degf, Math::max(1.0f - (position - target).length(), 0.0f)), 0.85f, 0.85f));

    Buffer vertices, indices;
    vertices.setData(MeshTools::interleave(sphere.positions(0), colors), BufferUsage::StaticDraw);
    indices.setData(sphere.indices(), BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setPrimitive(MeshPrimitive::Triangles)
        .setCount(sphere.indices().size())
        .addVertexBuffer(vertices, 0, Shaders::VertexColor3D::Position{}, Shaders::VertexColor3D::Color{})
        .setIndexBuffer(indices, 0, Mesh::IndexType::UnsignedInt);

    Shaders::VertexColor3D shader;
    shader.setTransformationProjectionMatrix(Projection*Transformation);

    mesh.draw(shader);

    return "vertexcolor.png";
}

std::string ShaderVisualizer::vector() {
    std::optional<Trade::ImageData2D> image;
    if(!_importer->openFile("vector.png") || !(image = _importer->image2D(0))) {
        Error() << "Cannot open vector.png";
        return "vector.png";
    }

    Texture2D texture;
    texture.setMinificationFilter(Sampler::Filter::Linear)
        .setMagnificationFilter(Sampler::Filter::Linear)
        .setWrapping(Sampler::Wrapping::ClampToEdge)
        .setStorage(1, TextureFormat::RGBA8, image->size())
        .setSubImage(0, {}, *image);

    Mesh mesh{NoCreate};
    std::unique_ptr<Buffer> vertices;
    std::tie(mesh, vertices, std::ignore) = MeshTools::compile(Primitives::Square::solid(Primitives::Square::TextureCoords::Generate), BufferUsage::StaticDraw);

    Shaders::Vector2D shader;
    shader.setColor(BaseColor)
        .setVectorTexture(texture)
        .setTransformationProjectionMatrix({});

    Renderer::enable(Renderer::Feature::Blending);
    Renderer::setBlendFunction(Renderer::BlendFunction::One, Renderer::BlendFunction::OneMinusSourceAlpha);
    Renderer::setBlendEquation(Renderer::BlendEquation::Add, Renderer::BlendEquation::Add);

    mesh.draw(shader);

    Renderer::disable(Renderer::Feature::Blending);

    return "vector.png";
}

std::string ShaderVisualizer::distanceFieldVector() {
    std::optional<Trade::ImageData2D> image;
    if(!_importer->openFile("vector-distancefield.png") || !(image = _importer->image2D(0))) {
        Error() << "Cannot open vector-distancefield.png";
        return "distancefieldvector.png";
    }

    Texture2D texture;
    texture.setMinificationFilter(Sampler::Filter::Linear)
        .setMagnificationFilter(Sampler::Filter::Linear)
        .setWrapping(Sampler::Wrapping::ClampToEdge)
        .setStorage(1, TextureFormat::RGBA8, image->size())
        .setSubImage(0, {}, *image);

    Mesh mesh{NoCreate};
    std::unique_ptr<Buffer> vertices;
    std::tie(mesh, vertices, std::ignore) = MeshTools::compile(Primitives::Square::solid(Primitives::Square::TextureCoords::Generate), BufferUsage::StaticDraw);

    Shaders::DistanceFieldVector2D shader;
    shader.setColor(BaseColor)
        .setOutlineColor(OutlineColor)
        .setOutlineRange(0.6f, 0.4f)
        .setVectorTexture(texture)
        .setTransformationProjectionMatrix({});

    Renderer::enable(Renderer::Feature::Blending);
    Renderer::setBlendFunction(Renderer::BlendFunction::One, Renderer::BlendFunction::OneMinusSourceAlpha);
    Renderer::setBlendEquation(Renderer::BlendEquation::Add, Renderer::BlendEquation::Add);

    mesh.draw(shader);

    Renderer::disable(Renderer::Feature::Blending);

    return "distancefieldvector.png";
}

MAGNUM_WINDOWLESSAPPLICATION_MAIN(ShaderVisualizer)
