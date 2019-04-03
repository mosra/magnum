/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include <tuple>
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
#include <Magnum/PixelFormat.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/MeshTools/Transform.h>
#include <Magnum/Primitives/Axis.h>
#include <Magnum/Primitives/Capsule.h>
#include <Magnum/Primitives/Circle.h>
#include <Magnum/Primitives/Crosshair.h>
#include <Magnum/Primitives/Cone.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Primitives/Cylinder.h>
#include <Magnum/Primitives/Gradient.h>
#include <Magnum/Primitives/Grid.h>
#include <Magnum/Primitives/Icosphere.h>
#include <Magnum/Primitives/Line.h>
#include <Magnum/Primitives/Plane.h>
#include <Magnum/Primitives/Square.h>
#include <Magnum/Primitives/UVSphere.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Shaders/MeshVisualizer.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Shaders/VertexColor.h>
#include <Magnum/Trade/AbstractImageConverter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshData2D.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/Trade/AbstractImporter.h>

using namespace Magnum;
using namespace Magnum::Math::Literals;

struct PrimitiveVisualizer: Platform::WindowlessApplication {
    explicit PrimitiveVisualizer(const Arguments& arguments): Platform::WindowlessApplication{arguments,
        #ifndef CORRADE_TARGET_APPLE
        /* So we can have wide lines */
        Configuration{}.clearFlags(Configuration::Flag::ForwardCompatible)
        #endif
    } {}

    int exec() override;

    std::pair<Trade::MeshData2D, std::string> axis2D();
    std::pair<Trade::MeshData3D, std::string> axis3D();

    std::pair<Trade::MeshData2D, std::string> capsule2DWireframe();
    std::pair<Trade::MeshData2D, std::string> circle2DWireframe();
    std::pair<Trade::MeshData2D, std::string> crosshair2D();
    std::pair<Trade::MeshData2D, std::string> line2D();
    std::pair<Trade::MeshData2D, std::string> squareWireframe();

    std::pair<Trade::MeshData3D, std::string> capsule3DWireframe();
    std::pair<Trade::MeshData3D, std::string> circle3DWireframe();
    std::pair<Trade::MeshData3D, std::string> crosshair3D();
    std::pair<Trade::MeshData3D, std::string> coneWireframe();
    std::pair<Trade::MeshData3D, std::string> cubeWireframe();
    std::pair<Trade::MeshData3D, std::string> cylinderWireframe();
    std::pair<Trade::MeshData3D, std::string> grid3DWireframe();
    std::pair<Trade::MeshData3D, std::string> line3D();
    std::pair<Trade::MeshData3D, std::string> planeWireframe();
    std::pair<Trade::MeshData3D, std::string> uvSphereWireframe();

    std::pair<Trade::MeshData2D, std::string> circle2DSolid();
    std::pair<Trade::MeshData2D, std::string> squareSolid();

    std::pair<Trade::MeshData3D, std::string> capsule3DSolid();
    std::pair<Trade::MeshData3D, std::string> circle3DSolid();
    std::pair<Trade::MeshData3D, std::string> coneSolid();
    std::pair<Trade::MeshData3D, std::string> cubeSolid();
    std::pair<Trade::MeshData3D, std::string> cylinderSolid();
    std::pair<Trade::MeshData3D, std::string> grid3DSolid();
    std::pair<Trade::MeshData3D, std::string> icosphereSolid();
    std::pair<Trade::MeshData3D, std::string> planeSolid();
    std::pair<Trade::MeshData3D, std::string> uvSphereSolid();

    std::pair<Trade::MeshData2D, std::string> gradient2D();
    std::pair<Trade::MeshData2D, std::string> gradient2DHorizontal();
    std::pair<Trade::MeshData2D, std::string> gradient2DVertical();

    std::pair<Trade::MeshData3D, std::string> gradient3D();
    std::pair<Trade::MeshData3D, std::string> gradient3DHorizontal();
    std::pair<Trade::MeshData3D, std::string> gradient3DVertical();
};

namespace {
    constexpr const Vector2i ImageSize{512};
    const auto BaseColor = 0x2f83cc_srgbf;
    const auto OutlineColor = 0xdcdcdc_srgbf;
    const auto Projection2D = Matrix3::projection({3.0f, 3.0f});
    const auto Projection3D = Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.001f, 100.0f);
    const auto Transformation2D = Matrix3::rotation(13.2_degf);
    const auto Transformation3D = Matrix4::translation(Vector3::zAxis(-6.0f))*
                                  Matrix4::rotationY(-10.82_degf)*
                                  Matrix4::rotationX(24.37_degf)*
                                  Matrix4::rotationZ(18.3_degf);
}

int PrimitiveVisualizer::exec() {
    PluginManager::Manager<Trade::AbstractImageConverter> converterManager;
    Containers::Pointer<Trade::AbstractImageConverter> converter = converterManager.loadAndInstantiate("PngImageConverter");
    if(!converter) {
        Error() << "Cannot load image converter plugin";
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

    /* Cheating a bit and enabling only face culling instead of depth test in
       order to draw the wireframe over. I couldn't get polygon offset to work
       on the first try so I gave up. This will of course break with things
       like torus later. */
    GL::Renderer::enable(GL::Renderer::Feature::FramebufferSrgb);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::One, GL::Renderer::BlendFunction::One);
    GL::Renderer::setClearColor(0x000000_srgbaf);
    CORRADE_INTERNAL_ASSERT(GL::Renderer::lineWidthRange().contains(2.0f));
    GL::Renderer::setLineWidth(2.0f);

    {
        Shaders::VertexColor2D shader;
        shader.setTransformationProjectionMatrix(Projection2D*Transformation2D);

        for(auto fun: {&PrimitiveVisualizer::axis2D}) {
            multisampleFramebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth);

            std::string filename;
            Containers::Optional<Trade::MeshData2D> data;
            std::tie(data, filename) = (this->*fun)();

            MeshTools::compile(*data).draw(shader);

            GL::AbstractFramebuffer::blit(multisampleFramebuffer, framebuffer, framebuffer.viewport(), GL::FramebufferBlit::Color);
            Image2D result = framebuffer.read(framebuffer.viewport(), {PixelFormat::RGBA8Unorm});
            converter->exportToFile(result, Utility::Directory::join("../", "primitives-" + filename));
        }
    }

    {
        Shaders::VertexColor3D shader;
        shader.setTransformationProjectionMatrix(Projection3D*Transformation3D);

        for(auto fun: {&PrimitiveVisualizer::axis3D}) {
            multisampleFramebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth);

            std::string filename;
            Containers::Optional<Trade::MeshData3D> data;
            std::tie(data, filename) = (this->*fun)();

            MeshTools::compile(*data).draw(shader);

            GL::AbstractFramebuffer::blit(multisampleFramebuffer, framebuffer, framebuffer.viewport(), GL::FramebufferBlit::Color);
            Image2D result = framebuffer.read(framebuffer.viewport(), {PixelFormat::RGBA8Unorm});
            converter->exportToFile(result, Utility::Directory::join("../", "primitives-" + filename));
        }
    }

    {
        Shaders::Flat2D shader;
        shader.setColor(OutlineColor)
            .setTransformationProjectionMatrix(Projection2D*Transformation2D);

        for(auto fun: {&PrimitiveVisualizer::capsule2DWireframe,
                       &PrimitiveVisualizer::circle2DWireframe,
                       &PrimitiveVisualizer::crosshair2D,
                       &PrimitiveVisualizer::line2D,
                       &PrimitiveVisualizer::squareWireframe})
        {
            multisampleFramebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth);

            std::string filename;
            Containers::Optional<Trade::MeshData2D> data;
            std::tie(data, filename) = (this->*fun)();

            MeshTools::compile(*data).draw(shader);

            GL::AbstractFramebuffer::blit(multisampleFramebuffer, framebuffer, framebuffer.viewport(), GL::FramebufferBlit::Color);
            Image2D result = framebuffer.read(framebuffer.viewport(), {PixelFormat::RGBA8Unorm});
            converter->exportToFile(result, Utility::Directory::join("../", "primitives-" + filename));
        }
    }

    {
        Shaders::Flat3D shader;
        shader.setColor(OutlineColor)
            .setTransformationProjectionMatrix(Projection3D*Transformation3D);

        for(auto fun: {&PrimitiveVisualizer::capsule3DWireframe,
                       &PrimitiveVisualizer::circle3DWireframe,
                       &PrimitiveVisualizer::crosshair3D,
                       &PrimitiveVisualizer::coneWireframe,
                       &PrimitiveVisualizer::cubeWireframe,
                       &PrimitiveVisualizer::cylinderWireframe,
                       &PrimitiveVisualizer::grid3DWireframe,
                       &PrimitiveVisualizer::line3D,
                       &PrimitiveVisualizer::planeWireframe,
                       &PrimitiveVisualizer::uvSphereWireframe})
        {
            multisampleFramebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth);

            std::string filename;
            Containers::Optional<Trade::MeshData3D> data;
            std::tie(data, filename) = (this->*fun)();

            MeshTools::compile(*data).draw(shader);

            GL::AbstractFramebuffer::blit(multisampleFramebuffer, framebuffer, framebuffer.viewport(), GL::FramebufferBlit::Color);
            Image2D result = framebuffer.read(framebuffer.viewport(), {PixelFormat::RGBA8Unorm});
            converter->exportToFile(result, Utility::Directory::join("../", "primitives-" + filename));
        }
    }

    Shaders::MeshVisualizer wireframe2D{Shaders::MeshVisualizer::Flag::Wireframe};
    wireframe2D.setColor(0x00000000_srgbaf)
        .setWireframeColor(OutlineColor)
        .setWireframeWidth(2.0f)
        .setViewportSize(Vector2{ImageSize})
        .setTransformationProjectionMatrix(Matrix4{
            /** @todo clean up once Matrix4 from Matrix3 constructor exists */
            {(Projection2D*Transformation2D)[0], 0.0f},
            {(Projection2D*Transformation2D)[1], 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f},
            {{(Projection2D*Transformation2D)[2].xy(), 0.0f}, 1.0f}});

    {
        Shaders::Flat2D flat;
        flat.setColor(BaseColor)
            .setTransformationProjectionMatrix(Projection2D*Transformation2D);

        for(auto fun: {&PrimitiveVisualizer::circle2DSolid,
                       &PrimitiveVisualizer::squareSolid})
        {
            multisampleFramebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth);

            std::string filename;
            Containers::Optional<Trade::MeshData2D> data;
            std::tie(data, filename) = (this->*fun)();

            /* TODO: use MeshTools::compile() and MeshVisualizer2D once it exists */
            GL::Buffer vertices;
            vertices.setData(data->positions(0), GL::BufferUsage::StaticDraw);
            GL::Mesh mesh;
            mesh.addVertexBuffer(vertices, 0, Shaders::MeshVisualizer::Position{Shaders::MeshVisualizer::Position::Components::Two})
                .setCount(data->positions(0).size())
                .setPrimitive(data->primitive());

            mesh.draw(flat)
                .draw(wireframe2D);

            GL::AbstractFramebuffer::blit(multisampleFramebuffer, framebuffer, framebuffer.viewport(), GL::FramebufferBlit::Color);
            Image2D result = framebuffer.read(framebuffer.viewport(), {PixelFormat::RGBA8Unorm});
            converter->exportToFile(result, Utility::Directory::join("../", "primitives-" + filename));
        }
    }

    Shaders::MeshVisualizer wireframe3D{Shaders::MeshVisualizer::Flag::Wireframe};
    wireframe3D.setColor(0x00000000_srgbaf)
        .setWireframeColor(OutlineColor)
        .setWireframeWidth(2.0f)
        .setViewportSize(Vector2{ImageSize})
        .setTransformationProjectionMatrix(Projection3D*Transformation3D);

    {
        Shaders::Phong phong;
        phong.setAmbientColor(0x22272e_srgbf)
            .setDiffuseColor(BaseColor)
            .setSpecularColor(0x000000_srgbf)
            .setLightPosition({5.0f, 5.0f, 7.0f})
            .setProjectionMatrix(Projection3D)
            .setTransformationMatrix(Transformation3D)
            .setNormalMatrix(Transformation3D.rotationScaling());

        for(auto fun: {&PrimitiveVisualizer::capsule3DSolid,
                       &PrimitiveVisualizer::circle3DSolid,
                       &PrimitiveVisualizer::coneSolid,
                       &PrimitiveVisualizer::cubeSolid,
                       &PrimitiveVisualizer::cylinderSolid,
                       &PrimitiveVisualizer::grid3DSolid,
                       &PrimitiveVisualizer::icosphereSolid,
                       &PrimitiveVisualizer::planeSolid,
                       &PrimitiveVisualizer::uvSphereSolid})
        {
            multisampleFramebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth);

            std::string filename;
            Containers::Optional<Trade::MeshData3D> data;
            std::tie(data, filename) = (this->*fun)();

            MeshTools::compile(*data)
                .draw(phong)
                .draw(wireframe3D);

            GL::AbstractFramebuffer::blit(multisampleFramebuffer, framebuffer, framebuffer.viewport(), GL::FramebufferBlit::Color);
            Image2D result = framebuffer.read(framebuffer.viewport(), {PixelFormat::RGBA8Unorm});
            converter->exportToFile(result, Utility::Directory::join("../", "primitives-" + filename));
        }
    }

    {
        Shaders::VertexColor2D shader;
        shader.setTransformationProjectionMatrix(Projection2D*Transformation2D);

        for(auto fun: {&PrimitiveVisualizer::gradient2D,
                       &PrimitiveVisualizer::gradient2DHorizontal,
                       &PrimitiveVisualizer::gradient2DVertical}) {
            multisampleFramebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth);

            std::string filename;
            Containers::Optional<Trade::MeshData2D> data;
            std::tie(data, filename) = (this->*fun)();

            MeshTools::compile(*data)
                .draw(shader)
                .draw(wireframe2D);

            GL::AbstractFramebuffer::blit(multisampleFramebuffer, framebuffer, framebuffer.viewport(), GL::FramebufferBlit::Color);
            Image2D result = framebuffer.read(framebuffer.viewport(), {PixelFormat::RGBA8Unorm});
            converter->exportToFile(result, Utility::Directory::join("../", "primitives-" + filename));
        }
    }

    {
        Shaders::VertexColor3D shader;
        shader.setTransformationProjectionMatrix(Projection3D*Transformation3D);

        for(auto fun: {&PrimitiveVisualizer::gradient3D,
                       &PrimitiveVisualizer::gradient3DHorizontal,
                       &PrimitiveVisualizer::gradient3DVertical}) {
            multisampleFramebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth);

            std::string filename;
            Containers::Optional<Trade::MeshData3D> data;
            std::tie(data, filename) = (this->*fun)();

            MeshTools::compile(*data)
                .draw(shader)
                .draw(wireframe3D);

            GL::AbstractFramebuffer::blit(multisampleFramebuffer, framebuffer, framebuffer.viewport(), GL::FramebufferBlit::Color);
            Image2D result = framebuffer.read(framebuffer.viewport(), {PixelFormat::RGBA8Unorm});
            converter->exportToFile(result, Utility::Directory::join("../", "primitives-" + filename));
        }
    }

    return 0;
}

std::pair<Trade::MeshData2D, std::string> PrimitiveVisualizer::axis2D() {
    return {Primitives::axis2D(), "axis2d.png"};
}

std::pair<Trade::MeshData2D, std::string> PrimitiveVisualizer::gradient2D() {
    return {Primitives::gradient2D({1.0f, -2.0f}, 0x2f83cc_srgbf, {-1.0f, 2.0f}, 0x3bd267_srgbf), "gradient2d.png"};
}

namespace {
    /* End colors for axis-aligned gradients are 20%/80% blends of the above to
       match the range */
    const Color3 Gradient20Percent = Math::lerp(0x2f83cc_srgbf, 0x3bd267_srgbf, 0.2f);
    const Color3 Gradient80Percent = Math::lerp(0x2f83cc_srgbf, 0x3bd267_srgbf, 0.8f);
}

std::pair<Trade::MeshData2D, std::string> PrimitiveVisualizer::gradient2DHorizontal() {
    return {Primitives::gradient2DHorizontal(Gradient20Percent, Gradient80Percent), "gradient2dhorizontal.png"};
}

std::pair<Trade::MeshData2D, std::string> PrimitiveVisualizer::gradient2DVertical() {
    /* End colors are 20%/80% blends of the above to match the range */
    return {Primitives::gradient2DVertical(Gradient20Percent, Gradient80Percent), "gradient2dvertical.png"};
}

std::pair<Trade::MeshData3D, std::string> PrimitiveVisualizer::axis3D() {
    return {Primitives::axis3D(), "axis3d.png"};
}

std::pair<Trade::MeshData3D, std::string> PrimitiveVisualizer::gradient3D() {
    return {Primitives::gradient3D({1.0f, -2.0f, -1.5f}, 0x2f83cc_srgbf, {-1.0f, 2.0f, -1.5f}, 0x3bd267_srgbf), "gradient3d.png"};
}

std::pair<Trade::MeshData3D, std::string> PrimitiveVisualizer::gradient3DHorizontal() {
    return {Primitives::gradient3DHorizontal(Gradient20Percent, Gradient80Percent), "gradient3dhorizontal.png"};
}

std::pair<Trade::MeshData3D, std::string> PrimitiveVisualizer::gradient3DVertical() {
    return {Primitives::gradient3DVertical(Gradient20Percent, Gradient80Percent), "gradient3dvertical.png"};
}

std::pair<Trade::MeshData2D, std::string> PrimitiveVisualizer::capsule2DWireframe() {
    auto capsule = Primitives::capsule2DWireframe(8, 1, 0.75f);
    MeshTools::transformPointsInPlace(Matrix3::scaling(Vector2{0.75f}), capsule.positions(0));
    return {std::move(capsule), "capsule2dwireframe.png"};
}

std::pair<Trade::MeshData2D, std::string> PrimitiveVisualizer::circle2DWireframe() {
    return {Primitives::circle2DWireframe(32), "circle2dwireframe.png"};
}

std::pair<Trade::MeshData2D, std::string> PrimitiveVisualizer::crosshair2D() {
    return {Primitives::crosshair2D(), "crosshair2d.png"};
}

std::pair<Trade::MeshData2D, std::string> PrimitiveVisualizer::line2D() {
    auto line = Primitives::line2D();
    MeshTools::transformPointsInPlace(Matrix3::translation(Vector2::xAxis(-1.0f))*Matrix3::scaling(Vector2::xScale(2.0f)), line.positions(0));
    return {std::move(line), "line2d.png"};
}

std::pair<Trade::MeshData2D, std::string> PrimitiveVisualizer::squareWireframe() {
    return {Primitives::squareWireframe(), "squarewireframe.png"};
}

std::pair<Trade::MeshData3D, std::string> PrimitiveVisualizer::capsule3DWireframe() {
    auto capsule = Primitives::capsule3DWireframe(8, 1, 16, 1.0f);
    MeshTools::transformPointsInPlace(Matrix4::scaling(Vector3{0.75f}), capsule.positions(0));
    return {std::move(capsule), "capsule3dwireframe.png"};
}

std::pair<Trade::MeshData3D, std::string> PrimitiveVisualizer::circle3DWireframe() {
    return {Primitives::circle3DWireframe(32), "circle3dwireframe.png"};
}

std::pair<Trade::MeshData3D, std::string> PrimitiveVisualizer::crosshair3D() {
    return {Primitives::crosshair3D(), "crosshair3d.png"};
}

std::pair<Trade::MeshData3D, std::string> PrimitiveVisualizer::coneWireframe() {
    return {Primitives::coneWireframe(32, 1.25f), "conewireframe.png"};
}

std::pair<Trade::MeshData3D, std::string> PrimitiveVisualizer::cubeWireframe() {
    return {Primitives::cubeWireframe(), "cubewireframe.png"};
}

std::pair<Trade::MeshData3D, std::string> PrimitiveVisualizer::cylinderWireframe() {
    return {Primitives::cylinderWireframe(1, 32, 1.0f), "cylinderwireframe.png"};
}

std::pair<Trade::MeshData3D, std::string> PrimitiveVisualizer::grid3DWireframe() {
    return {Primitives::grid3DWireframe({5, 3}), "grid3dwireframe.png"};
}

std::pair<Trade::MeshData3D, std::string> PrimitiveVisualizer::line3D() {
    auto line = Primitives::line3D();
    MeshTools::transformPointsInPlace(Matrix4::translation(Vector3::xAxis(-1.0f))*Matrix4::scaling(Vector3::xScale(2.0f)), line.positions(0));
    return {std::move(line), "line3d.png"};
}

std::pair<Trade::MeshData3D, std::string> PrimitiveVisualizer::planeWireframe() {
    return {Primitives::planeWireframe(), "planewireframe.png"};
}

std::pair<Trade::MeshData3D, std::string> PrimitiveVisualizer::uvSphereWireframe() {
    return {Primitives::uvSphereWireframe(16, 32), "uvspherewireframe.png"};
}

std::pair<Trade::MeshData2D, std::string> PrimitiveVisualizer::circle2DSolid() {
    return {Primitives::circle2DSolid(16), "circle2dsolid.png"};
}

std::pair<Trade::MeshData2D, std::string> PrimitiveVisualizer::squareSolid() {
    return {Primitives::squareSolid(), "squaresolid.png"};
}

std::pair<Trade::MeshData3D, std::string> PrimitiveVisualizer::capsule3DSolid() {
    auto capsule = Primitives::capsule3DSolid(4, 1, 12, 0.75f);
    MeshTools::transformPointsInPlace(Matrix4::scaling(Vector3{0.75f}), capsule.positions(0));
    return {std::move(capsule), "capsule3dsolid.png"};
}

std::pair<Trade::MeshData3D, std::string> PrimitiveVisualizer::circle3DSolid() {
    return {Primitives::circle3DSolid(16), "circle3dsolid.png"};
}

std::pair<Trade::MeshData3D, std::string> PrimitiveVisualizer::coneSolid() {
    return {Primitives::coneSolid(1, 12, 1.25f, Primitives::ConeFlag::CapEnd), "conesolid.png"};
}

std::pair<Trade::MeshData3D, std::string> PrimitiveVisualizer::cubeSolid() {
    return {Primitives::cubeSolid(), "cubesolid.png"};
}

std::pair<Trade::MeshData3D, std::string> PrimitiveVisualizer::cylinderSolid() {
    return {Primitives::cylinderSolid(1, 12, 1.0f, Primitives::CylinderFlag::CapEnds), "cylindersolid.png"};
}

std::pair<Trade::MeshData3D, std::string> PrimitiveVisualizer::grid3DSolid() {
    return {Primitives::grid3DSolid({5, 3}), "grid3dsolid.png"};
}

std::pair<Trade::MeshData3D, std::string> PrimitiveVisualizer::icosphereSolid() {
    return {Primitives::icosphereSolid(1), "icospheresolid.png"};
}

std::pair<Trade::MeshData3D, std::string> PrimitiveVisualizer::planeSolid() {
    return {Primitives::planeSolid(), "planesolid.png"};
}

std::pair<Trade::MeshData3D, std::string> PrimitiveVisualizer::uvSphereSolid() {
    return {Primitives::uvSphereSolid(8, 16), "uvspheresolid.png"};
}

MAGNUM_WINDOWLESSAPPLICATION_MAIN(PrimitiveVisualizer)
