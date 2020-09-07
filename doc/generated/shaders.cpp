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

#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Containers/Optional.h>
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

#include <Magnum/Image.h>
#include <Magnum/ImageView.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/DebugTools/ColorMap.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/Matrix4.h>
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
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Trade/AbstractImporter.h>

using namespace Magnum;
using namespace Magnum::Math::Literals;

struct ShaderVisualizer: Platform::WindowlessApplication {
    explicit ShaderVisualizer(const Arguments& arguments): Platform::WindowlessApplication{arguments} {}

    int exec() override;

    std::string phong();
    std::string meshVisualizer2D();
    std::string meshVisualizer2DPrimitiveId();
    std::string meshVisualizer3D();
    std::string meshVisualizer3DPrimitiveId();
    std::string flat();
    std::string vertexColor();

    std::string vector();
    std::string distanceFieldVector();

    Containers::Pointer<Trade::AbstractImporter> _importer;
};

namespace {
    constexpr const Vector2i ImageSize{512};
}

int ShaderVisualizer::exec() {
    PluginManager::Manager<Trade::AbstractImageConverter> converterManager;
    Containers::Pointer<Trade::AbstractImageConverter> converter = converterManager.loadAndInstantiate("PngImageConverter");
    if(!converter) {
        Error() << "Cannot load image converter plugin";
        std::exit(1);
    }

    PluginManager::Manager<Trade::AbstractImporter> importerManager;
    _importer = importerManager.loadAndInstantiate("PngImporter");
    if(!_importer) {
        Error() << "Cannot load image importer plugin";
        std::exit(1);
    }

    GL::Renderbuffer multisampleColor, multisampleDepth;
    multisampleColor.setStorageMultisample(16, GL::RenderbufferFormat::SRGB8Alpha8, ImageSize);
    multisampleDepth.setStorageMultisample(16, GL::RenderbufferFormat::DepthComponent24, ImageSize);

    GL::Framebuffer multisampleFramebuffer{{{}, ImageSize}};
    multisampleFramebuffer.attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, multisampleColor)
        .attachRenderbuffer(GL::Framebuffer::BufferAttachment::Depth, multisampleDepth)
        .bind();
    CORRADE_INTERNAL_ASSERT(multisampleFramebuffer.checkStatus(GL::FramebufferTarget::Draw) == GL::Framebuffer::Status::Complete);

    GL::Renderbuffer color;
    color.setStorage(GL::RenderbufferFormat::SRGB8Alpha8, ImageSize);
    GL::Framebuffer framebuffer{{{}, ImageSize}};
    framebuffer.attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, color);

    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FramebufferSrgb);
    GL::Renderer::setClearColor(0x000000_srgbaf);

    for(auto fun: {&ShaderVisualizer::phong,
                   &ShaderVisualizer::meshVisualizer2D,
                   &ShaderVisualizer::meshVisualizer2DPrimitiveId,
                   &ShaderVisualizer::meshVisualizer3D,
                   &ShaderVisualizer::meshVisualizer3DPrimitiveId,
                   &ShaderVisualizer::flat,
                   &ShaderVisualizer::vertexColor,
                   &ShaderVisualizer::vector,
                   &ShaderVisualizer::distanceFieldVector}) {
        multisampleFramebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth);

        std::string filename = (this->*fun)();

        GL::AbstractFramebuffer::blit(multisampleFramebuffer, framebuffer, framebuffer.viewport(), GL::FramebufferBlit::Color);
        Image2D result = framebuffer.read(framebuffer.viewport(), {PixelFormat::RGBA8Unorm});
        converter->exportToFile(result, Utility::Directory::join("../", "shaders-" + filename));
    }

    _importer.reset();

    return 0;
}

namespace {
    const auto Projection = Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.001f, 100.0f);
    const auto Transformation = Matrix4::translation(Vector3::zAxis(-5.0f));
    const auto BaseColor = 0x2f83cc_srgbf;
    const auto OutlineColor = 0xdcdcdc_srgbf;
}

std::string ShaderVisualizer::phong() {
    Shaders::Phong{}
        .setAmbientColor(0x22272e_srgbf)
        .setDiffuseColor(BaseColor)
        .setShininess(200.0f)
        .setLightPositions({{5.0f, 5.0f, 7.0f, 0.0f}})
        .setProjectionMatrix(Projection)
        .setTransformationMatrix(Transformation)
        .setNormalMatrix(Transformation.normalMatrix())
        .draw(MeshTools::compile(Primitives::uvSphereSolid(16, 32)));

    return "phong.png";
}

std::string ShaderVisualizer::meshVisualizer2D() {
    const Matrix3 projection =
        Matrix3::projection(Vector2{3.0f})*
        Matrix3::rotation(13.7_degf);

    Shaders::MeshVisualizer2D{Shaders::MeshVisualizer2D::Flag::Wireframe}
        .setColor(BaseColor)
        .setWireframeColor(OutlineColor)
        .setWireframeWidth(2.0f)
        .setViewportSize(Vector2{ImageSize})
        .setTransformationProjectionMatrix(projection)
        .draw(MeshTools::compile(Primitives::circle2DSolid(8)));

    return "meshvisualizer2d.png";
}

std::string ShaderVisualizer::meshVisualizer2DPrimitiveId() {
    const Matrix3 projection =
        Matrix3::projection(Vector2{3.0f})*
        Matrix3::rotation(13.7_degf);

    const auto map = DebugTools::ColorMap::turbo();
    const Vector2i size{Int(map.size()), 1};
    GL::Texture2D colorMapTexture;
    colorMapTexture
        .setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::Repeat)
        .setStorage(1, GL::TextureFormat::SRGB8Alpha8, size)
        .setSubImage(0, {}, ImageView2D{PixelFormat::RGB8Srgb, size, map});

    Shaders::MeshVisualizer2D{Shaders::MeshVisualizer2D::Flag::PrimitiveId}
        .setTransformationProjectionMatrix(projection)
        .setColorMapTransformation(1.0f/255.0f, 1.0f/8.0f)
        .bindColorMapTexture(colorMapTexture)
        .draw(MeshTools::compile(Primitives::circle2DSolid(8)));

    return "meshvisualizer2d-primitiveid.png";
}

std::string ShaderVisualizer::meshVisualizer3D() {
    const Matrix4 transformation = Transformation*
        Matrix4::rotationZ(13.7_degf)*
        Matrix4::rotationX(-12.6_degf);

    Shaders::MeshVisualizer3D{Shaders::MeshVisualizer3D::Flag::Wireframe|
                              Shaders::MeshVisualizer3D::Flag::TangentDirection|
                              Shaders::MeshVisualizer3D::Flag::BitangentFromTangentDirection|
                              Shaders::MeshVisualizer3D::Flag::NormalDirection}
        .setColor(BaseColor)
        .setWireframeColor(OutlineColor)
        .setWireframeWidth(2.0f)
        .setLineLength(0.3333333333f)
        .setLineWidth(3.0f)
        .setViewportSize(Vector2{ImageSize})
        .setTransformationMatrix(transformation)
        .setProjectionMatrix(Projection)
        .setNormalMatrix(transformation.normalMatrix())
        .draw(MeshTools::compile(Primitives::uvSphereSolid(4, 8, Primitives::UVSphereFlag::TextureCoordinates|Primitives::UVSphereFlag::Tangents)));

    return "meshvisualizer3d.png";
}

std::string ShaderVisualizer::meshVisualizer3DPrimitiveId() {
    const Matrix4 transformation = Transformation*
        Matrix4::rotationZ(13.7_degf)*
        Matrix4::rotationX(-12.6_degf);

    const auto map = DebugTools::ColorMap::turbo();
    const Vector2i size{Int(map.size()), 1};
    GL::Texture2D colorMapTexture;
    colorMapTexture
        .setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::Repeat)
        .setStorage(1, GL::TextureFormat::SRGB8Alpha8, size)
        .setSubImage(0, {}, ImageView2D{PixelFormat::RGB8Srgb, size, map});

    Shaders::MeshVisualizer3D{Shaders::MeshVisualizer3D::Flag::PrimitiveId}
        .setTransformationMatrix(transformation)
        .setProjectionMatrix(Projection)
        .setColorMapTransformation(1.0f/255.0f, 1.0f/32.0f)
        .bindColorMapTexture(colorMapTexture)
        .draw(MeshTools::compile(Primitives::uvSphereSolid(4, 8)));

    return "meshvisualizer3d-primitiveid.png";
}

std::string ShaderVisualizer::flat() {
    Shaders::Flat3D{}
        .setColor(BaseColor)
        .setTransformationProjectionMatrix(Projection*Transformation)
        .draw(MeshTools::compile(Primitives::uvSphereSolid(16, 32)));

    return "flat.png";
}

std::string ShaderVisualizer::vertexColor() {
    Trade::MeshData sphere = Primitives::uvSphereSolid(32, 64);

    /* Color vertices nearest to given position */
    auto target = Vector3{2.0f, 2.0f, 7.0f}.normalized();
    std::vector<Color3> colors;
    colors.reserve(sphere.vertexCount());
    for(Vector3 position: sphere.attribute<Vector3>(Trade::MeshAttribute::Position))
        colors.push_back(Color3::fromHsv({Math::lerp(240.0_degf, 420.0_degf, Math::max(1.0f - (position - target).length(), 0.0f)), 0.85f, 0.666f}));

    GL::Buffer vertices, indices;
    vertices.setData(MeshTools::interleave(sphere.attribute<Vector3>(Trade::MeshAttribute::Position), colors), GL::BufferUsage::StaticDraw);
    indices.setData(sphere.indices<UnsignedInt>(), GL::BufferUsage::StaticDraw);

    GL::Mesh mesh;
    mesh.setPrimitive(GL::MeshPrimitive::Triangles)
        .setCount(sphere.indexCount())
        .addVertexBuffer(vertices, 0,
            Shaders::VertexColor3D::Position{},
            Shaders::VertexColor3D::Color3{})
        .setIndexBuffer(indices, 0, GL::MeshIndexType::UnsignedInt);

    Shaders::VertexColor3D shader;
    shader.setTransformationProjectionMatrix(Projection*Transformation)
        .draw(mesh);

    return "vertexcolor.png";
}

std::string ShaderVisualizer::vector() {
    Containers::Optional<Trade::ImageData2D> image;
    if(!_importer->openFile("vector.png") || !(image = _importer->image2D(0))) {
        Error() << "Cannot open vector.png";
        return "vector.png";
    }

    GL::Texture2D texture;
    texture.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, GL::TextureFormat::RGBA8, image->size())
        .setSubImage(0, {}, *image);

    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::One, GL::Renderer::BlendFunction::OneMinusSourceAlpha);
    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add, GL::Renderer::BlendEquation::Add);

    Shaders::Vector2D{}
        .setColor(BaseColor)
        .bindVectorTexture(texture)
        .setTransformationProjectionMatrix({})
        .draw(MeshTools::compile(Primitives::squareSolid(Primitives::SquareFlag::TextureCoordinates)));

    GL::Renderer::disable(GL::Renderer::Feature::Blending);

    return "vector.png";
}

std::string ShaderVisualizer::distanceFieldVector() {
    Containers::Optional<Trade::ImageData2D> image;
    if(!_importer->openFile("vector-distancefield.png") || !(image = _importer->image2D(0))) {
        Error() << "Cannot open vector-distancefield.png";
        return "distancefieldvector.png";
    }

    GL::Texture2D texture;
    texture.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, GL::TextureFormat::RGBA8, image->size())
        .setSubImage(0, {}, *image);

    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::One, GL::Renderer::BlendFunction::OneMinusSourceAlpha);
    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add, GL::Renderer::BlendEquation::Add);

    Shaders::DistanceFieldVector2D{}
        .setColor(BaseColor)
        .setOutlineColor(OutlineColor)
        .setOutlineRange(0.6f, 0.4f)
        .bindVectorTexture(texture)
        .setTransformationProjectionMatrix({})
        .draw(MeshTools::compile(Primitives::squareSolid(Primitives::SquareFlag::TextureCoordinates)));

    GL::Renderer::disable(GL::Renderer::Feature::Blending);

    return "distancefieldvector.png";
}

MAGNUM_WINDOWLESSAPPLICATION_MAIN(ShaderVisualizer)
