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

#include <new>
#include <sstream>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Vk/Image.h"
#include "Magnum/Vk/RenderPassCreateInfo.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct RenderPassTest: TestSuite::Tester {
    explicit RenderPassTest();

    /* While *ConstructFromVk() tests that going from VkFromThing -> Vk::Thing
       -> VkToThing doesn't result in information loss, the *ConvertToVk()
       tests additionally check that all calls both on our APIs and by editing
       the contained structure are correctly propagated to the resulting
       structures. */

    void attachmentDescriptionConstruct();
    void attachmentDescriptionConstructImplicitLayout();
    void attachmentDescriptionConstructDepthStencil();
    void attachmentDescriptionConstructDepthStencilImplicitLayout();
    void attachmentDescriptionConstructImplicitLoadStoreLayout();
    void attachmentDescriptionConstructNoInit();
    template<class From, class To> void attachmentDescriptionConstructFromVk();
    template<class T> void attachmentDescriptionConvertToVk();

    void attachmentReferenceConstruct();
    void attachmentReferenceConstructUnused();
    void attachmentReferenceConstructNoInit();
    template<class From, class To> void attachmentReferenceConstructFromVk();
    template<class T> void attachmentReferenceConvertToVk();

    void subpassDescriptionConstruct();
    void subpassDescriptionConstructNoInit();
    void subpassDescriptionConstructInputAttachments();
    void subpassDescriptionConstructColorAttachments();
    void subpassDescriptionConstructColorResolveAttachments();
    void subpassDescriptionConstructColorResolveAttachmentsWrongCount();
    void subpassDescriptionConstructDepthStencilAttachment();
    void subpassDescriptionConstructPreserveAttachments();
    void subpassDescriptionConstructEmptyAttachmentLists();
    template<class From, class To> void subpassDescriptionConstructFromVk();
    void subpassDescriptionConstructCopy();
    void subpassDescriptionConstructMove();
    template<class T> void subpassDescriptionConvertToVk();
    template<class T> void subpassDescriptionConvertToVkNoAttachments();
    template<class T> void subpassDescriptionConvertToVkNoResolveAttachments();
    void subpassDescriptionRvalue();

    void subpassDependencyConstructNoInit();
    template<class From, class To> void subpassDependencyConstructFromVk();
    template<class T> void subpassDependencyConvertToVk();

    void createInfoConstruct();
    void createInfoConstructNoInit();
    void createInfoConstructAttachments();
    void createInfoConstructSubpasses();
    void createInfoConstructDependencies();
    template<class From, class To> void createInfoConstructFromVk();
    void createInfoConstructCopy();
    void createInfoConstructMove();
    template<class T> void createInfoConvertToVk();

    void constructNoCreate();
    void constructCopy();
};

RenderPassTest::RenderPassTest() {
    addTests({&RenderPassTest::attachmentDescriptionConstruct,
              &RenderPassTest::attachmentDescriptionConstructImplicitLayout,
              &RenderPassTest::attachmentDescriptionConstructDepthStencil,
              &RenderPassTest::attachmentDescriptionConstructDepthStencilImplicitLayout,
              &RenderPassTest::attachmentDescriptionConstructImplicitLoadStoreLayout,
              &RenderPassTest::attachmentDescriptionConstructNoInit,
              &RenderPassTest::attachmentDescriptionConstructFromVk<VkAttachmentDescription2, VkAttachmentDescription2>,
              &RenderPassTest::attachmentDescriptionConstructFromVk<VkAttachmentDescription, VkAttachmentDescription2>,
              &RenderPassTest::attachmentDescriptionConstructFromVk<VkAttachmentDescription2, VkAttachmentDescription>,
              &RenderPassTest::attachmentDescriptionConstructFromVk<VkAttachmentDescription, VkAttachmentDescription>,
              &RenderPassTest::attachmentDescriptionConvertToVk<VkAttachmentDescription2>,
              &RenderPassTest::attachmentDescriptionConvertToVk<VkAttachmentDescription>,

              &RenderPassTest::attachmentReferenceConstruct,
              &RenderPassTest::attachmentReferenceConstructUnused,
              &RenderPassTest::attachmentReferenceConstructNoInit,
              &RenderPassTest::attachmentReferenceConstructFromVk<VkAttachmentReference2, VkAttachmentReference2>,
              &RenderPassTest::attachmentReferenceConstructFromVk<VkAttachmentReference, VkAttachmentReference2>,
              &RenderPassTest::attachmentReferenceConstructFromVk<VkAttachmentReference2, VkAttachmentReference>,
              &RenderPassTest::attachmentReferenceConstructFromVk<VkAttachmentReference, VkAttachmentReference>,
              &RenderPassTest::attachmentReferenceConvertToVk<VkAttachmentReference2>,
              &RenderPassTest::attachmentReferenceConvertToVk<VkAttachmentReference>,

              &RenderPassTest::subpassDescriptionConstruct,
              &RenderPassTest::subpassDescriptionConstructNoInit,
              &RenderPassTest::subpassDescriptionConstructInputAttachments,
              &RenderPassTest::subpassDescriptionConstructColorAttachments,
              &RenderPassTest::subpassDescriptionConstructColorResolveAttachments,
              &RenderPassTest::subpassDescriptionConstructColorResolveAttachmentsWrongCount,
              &RenderPassTest::subpassDescriptionConstructDepthStencilAttachment,
              &RenderPassTest::subpassDescriptionConstructPreserveAttachments,
              &RenderPassTest::subpassDescriptionConstructEmptyAttachmentLists,
              &RenderPassTest::subpassDescriptionConstructFromVk<VkSubpassDescription2, VkSubpassDescription2>,
              &RenderPassTest::subpassDescriptionConstructFromVk<VkSubpassDescription, VkSubpassDescription2>,
              &RenderPassTest::subpassDescriptionConstructFromVk<VkSubpassDescription2, VkSubpassDescription>,
              &RenderPassTest::subpassDescriptionConstructFromVk<VkSubpassDescription, VkSubpassDescription>,
              &RenderPassTest::subpassDescriptionConstructCopy,
              &RenderPassTest::subpassDescriptionConstructMove,
              &RenderPassTest::subpassDescriptionConvertToVk<VkSubpassDescription2>,
              &RenderPassTest::subpassDescriptionConvertToVk<VkSubpassDescription>,
              &RenderPassTest::subpassDescriptionConvertToVkNoAttachments<VkSubpassDescription2>,
              &RenderPassTest::subpassDescriptionConvertToVkNoAttachments<VkSubpassDescription>,
              &RenderPassTest::subpassDescriptionConvertToVkNoResolveAttachments<VkSubpassDescription2>,
              &RenderPassTest::subpassDescriptionConvertToVkNoResolveAttachments<VkSubpassDescription>,
              &RenderPassTest::subpassDescriptionRvalue,

              &RenderPassTest::subpassDependencyConstructNoInit,
              &RenderPassTest::subpassDependencyConstructFromVk<VkSubpassDependency2, VkSubpassDependency2>,
              &RenderPassTest::subpassDependencyConstructFromVk<VkSubpassDependency, VkSubpassDependency2>,
              &RenderPassTest::subpassDependencyConstructFromVk<VkSubpassDependency2, VkSubpassDependency>,
              &RenderPassTest::subpassDependencyConstructFromVk<VkSubpassDependency, VkSubpassDependency>,
              &RenderPassTest::subpassDependencyConvertToVk<VkSubpassDependency2>,
              &RenderPassTest::subpassDependencyConvertToVk<VkSubpassDependency>,

              &RenderPassTest::createInfoConstruct,
              &RenderPassTest::createInfoConstructNoInit,
              &RenderPassTest::createInfoConstructAttachments,
              &RenderPassTest::createInfoConstructSubpasses,
              &RenderPassTest::createInfoConstructDependencies,
              &RenderPassTest::createInfoConstructFromVk<VkRenderPassCreateInfo2, VkRenderPassCreateInfo2>,
              &RenderPassTest::createInfoConstructFromVk<VkRenderPassCreateInfo, VkRenderPassCreateInfo2>,
              &RenderPassTest::createInfoConstructFromVk<VkRenderPassCreateInfo2, VkRenderPassCreateInfo>,
              &RenderPassTest::createInfoConstructFromVk<VkRenderPassCreateInfo, VkRenderPassCreateInfo>,
              &RenderPassTest::createInfoConstructCopy,
              &RenderPassTest::createInfoConstructMove,
              &RenderPassTest::createInfoConvertToVk<VkRenderPassCreateInfo2>,
              &RenderPassTest::createInfoConvertToVk<VkRenderPassCreateInfo>,

              &RenderPassTest::constructNoCreate,
              &RenderPassTest::constructCopy});
}

template<class> struct Traits;
#define _c(type)                                                            \
    template<> struct Traits<Vk ## type> {                                  \
        static const char* name() { return #type; }                         \
        static Vk ## type convert(const type& instance) {                   \
            return instance.vk ## type ();                                  \
        }                                                                   \
    };                                                                      \
    template<> struct Traits<Vk ## type ## 2> {                             \
        static const char* name() { return #type "2"; }                     \
        static Vk ## type ## 2 convert(const type& instance) {              \
            return instance;                                                \
        }                                                                   \
    };
#define _ca(type)                                                           \
    template<> struct Traits<Vk ## type> {                                  \
        static const char* name() { return #type; }                         \
        static Containers::Array<Vk ## type> convert(const type& instance) { \
            return instance.vk ## type ();                                  \
        }                                                                   \
    };                                                                      \
    template<> struct Traits<Vk ## type ## 2> {                             \
        static const char* name() { return #type "2"; }                     \
        /* So we have the same interface in both cases */                   \
        static Containers::Array<Vk ## type ## 2> convert(const type& instance) { \
            return {Containers::InPlaceInit, {*instance}};                  \
        }                                                                   \
    };
_c(AttachmentDescription)
_c(AttachmentReference)
_ca(SubpassDescription)
_c(SubpassDependency)
_ca(RenderPassCreateInfo)
#undef _c
#undef _ca

void RenderPassTest::attachmentDescriptionConstruct() {
    AttachmentDescription description{VK_FORMAT_R8G8B8A8_SNORM,
        AttachmentLoadOperation::Clear, AttachmentStoreOperation::DontCare,
        ImageLayout::ColorAttachment, ImageLayout::TransferDestination,
        4, AttachmentDescription::Flag::MayAlias};
    CORRADE_COMPARE(description->flags, VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT);
    CORRADE_COMPARE(description->format, VK_FORMAT_R8G8B8A8_SNORM);
    CORRADE_COMPARE(description->samples, VK_SAMPLE_COUNT_4_BIT);
    CORRADE_COMPARE(description->loadOp, VK_ATTACHMENT_LOAD_OP_CLEAR);
    CORRADE_COMPARE(description->stencilLoadOp, VK_ATTACHMENT_LOAD_OP_LOAD);
    CORRADE_COMPARE(description->storeOp, VK_ATTACHMENT_STORE_OP_DONT_CARE);
    CORRADE_COMPARE(description->stencilStoreOp, VK_ATTACHMENT_STORE_OP_STORE);
    CORRADE_COMPARE(description->initialLayout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    CORRADE_COMPARE(description->finalLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
}

void RenderPassTest::attachmentDescriptionConstructImplicitLayout() {
    AttachmentDescription description{VK_FORMAT_R8G8B8A8_SNORM,
        AttachmentLoadOperation::Clear, AttachmentStoreOperation::DontCare,
        4, AttachmentDescription::Flag::MayAlias};
    CORRADE_COMPARE(description->flags, VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT);
    CORRADE_COMPARE(description->format, VK_FORMAT_R8G8B8A8_SNORM);
    CORRADE_COMPARE(description->samples, VK_SAMPLE_COUNT_4_BIT);
    CORRADE_COMPARE(description->loadOp, VK_ATTACHMENT_LOAD_OP_CLEAR);
    CORRADE_COMPARE(description->stencilLoadOp, VK_ATTACHMENT_LOAD_OP_LOAD);
    CORRADE_COMPARE(description->storeOp, VK_ATTACHMENT_STORE_OP_DONT_CARE);
    CORRADE_COMPARE(description->stencilStoreOp, VK_ATTACHMENT_STORE_OP_STORE);
    CORRADE_COMPARE(description->initialLayout, VK_IMAGE_LAYOUT_GENERAL);
    CORRADE_COMPARE(description->finalLayout, VK_IMAGE_LAYOUT_GENERAL);
}

void RenderPassTest::attachmentDescriptionConstructDepthStencil() {
    AttachmentDescription description{VK_FORMAT_R8G8B8A8_SNORM,
        {AttachmentLoadOperation::Clear, AttachmentLoadOperation::DontCare},
        {AttachmentStoreOperation::Store, AttachmentStoreOperation::DontCare},
        ImageLayout::DepthStencilAttachment, ImageLayout::ShaderReadOnly,
        4, AttachmentDescription::Flag::MayAlias};
    CORRADE_COMPARE(description->flags, VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT);
    CORRADE_COMPARE(description->format, VK_FORMAT_R8G8B8A8_SNORM);
    CORRADE_COMPARE(description->samples, VK_SAMPLE_COUNT_4_BIT);
    CORRADE_COMPARE(description->loadOp, VK_ATTACHMENT_LOAD_OP_CLEAR);
    CORRADE_COMPARE(description->stencilLoadOp, VK_ATTACHMENT_LOAD_OP_DONT_CARE);
    CORRADE_COMPARE(description->storeOp, VK_ATTACHMENT_STORE_OP_STORE);
    CORRADE_COMPARE(description->stencilStoreOp, VK_ATTACHMENT_STORE_OP_DONT_CARE);
    CORRADE_COMPARE(description->initialLayout, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    CORRADE_COMPARE(description->finalLayout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void RenderPassTest::attachmentDescriptionConstructDepthStencilImplicitLayout() {
    AttachmentDescription description{VK_FORMAT_R8G8B8A8_SNORM,
        {AttachmentLoadOperation::Clear, AttachmentLoadOperation::DontCare},
        {AttachmentStoreOperation::Store, AttachmentStoreOperation::DontCare},
        4, AttachmentDescription::Flag::MayAlias};
    CORRADE_COMPARE(description->flags, VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT);
    CORRADE_COMPARE(description->format, VK_FORMAT_R8G8B8A8_SNORM);
    CORRADE_COMPARE(description->samples, VK_SAMPLE_COUNT_4_BIT);
    CORRADE_COMPARE(description->loadOp, VK_ATTACHMENT_LOAD_OP_CLEAR);
    CORRADE_COMPARE(description->stencilLoadOp, VK_ATTACHMENT_LOAD_OP_DONT_CARE);
    CORRADE_COMPARE(description->storeOp, VK_ATTACHMENT_STORE_OP_STORE);
    CORRADE_COMPARE(description->stencilStoreOp, VK_ATTACHMENT_STORE_OP_DONT_CARE);
    CORRADE_COMPARE(description->initialLayout, VK_IMAGE_LAYOUT_GENERAL);
    CORRADE_COMPARE(description->finalLayout, VK_IMAGE_LAYOUT_GENERAL);
}

void RenderPassTest::attachmentDescriptionConstructImplicitLoadStoreLayout() {
    AttachmentDescription description{VK_FORMAT_R8G8B8A8_SNORM,
        4, AttachmentDescription::Flag::MayAlias};
    CORRADE_COMPARE(description->flags, VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT);
    CORRADE_COMPARE(description->format, VK_FORMAT_R8G8B8A8_SNORM);
    CORRADE_COMPARE(description->samples, VK_SAMPLE_COUNT_4_BIT);
    CORRADE_COMPARE(description->loadOp, VK_ATTACHMENT_LOAD_OP_LOAD);
    CORRADE_COMPARE(description->stencilLoadOp, VK_ATTACHMENT_LOAD_OP_LOAD);
    CORRADE_COMPARE(description->storeOp, VK_ATTACHMENT_STORE_OP_STORE);
    CORRADE_COMPARE(description->stencilStoreOp, VK_ATTACHMENT_STORE_OP_STORE);
    CORRADE_COMPARE(description->initialLayout, VK_IMAGE_LAYOUT_GENERAL);
    CORRADE_COMPARE(description->finalLayout, VK_IMAGE_LAYOUT_GENERAL);
}

void RenderPassTest::attachmentDescriptionConstructNoInit() {
    AttachmentDescription description{NoInit};
    description->sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    new(&description) AttachmentDescription{NoInit};
    CORRADE_COMPARE(description->sType, VK_STRUCTURE_TYPE_APPLICATION_INFO);

    CORRADE_VERIFY((std::is_nothrow_constructible<AttachmentDescription, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, AttachmentDescription>::value));
}

template<class From, class To> void RenderPassTest::attachmentDescriptionConstructFromVk() {
    setTestCaseTemplateName({Traits<From>::name(), Traits<To>::name()});

    From from{};
    from.flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
    from.format = VK_FORMAT_R8G8B8A8_SNORM;
    from.samples = VK_SAMPLE_COUNT_32_BIT;
    from.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    from.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    from.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    from.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
    from.initialLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL;
    from.finalLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

    AttachmentDescription description{from};
    To to = Traits<To>::convert(description);
    CORRADE_COMPARE(to.flags, VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT);
    CORRADE_COMPARE(to.format, VK_FORMAT_R8G8B8A8_SNORM);
    CORRADE_COMPARE(to.samples, VK_SAMPLE_COUNT_32_BIT);
    CORRADE_COMPARE(to.loadOp, VK_ATTACHMENT_LOAD_OP_CLEAR);
    CORRADE_COMPARE(to.stencilLoadOp, VK_ATTACHMENT_LOAD_OP_DONT_CARE);
    CORRADE_COMPARE(to.storeOp, VK_ATTACHMENT_STORE_OP_DONT_CARE);
    CORRADE_COMPARE(to.stencilStoreOp, VK_ATTACHMENT_STORE_OP_STORE);
    CORRADE_COMPARE(to.initialLayout, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL);
    CORRADE_COMPARE(to.finalLayout, VK_IMAGE_LAYOUT_PREINITIALIZED);
}

template<class T> void RenderPassTest::attachmentDescriptionConvertToVk() {
    setTestCaseTemplateName(Traits<T>::name());

    AttachmentDescription description{VK_FORMAT_R8G8B8A8_SNORM,
        {AttachmentLoadOperation::Clear, AttachmentLoadOperation::DontCare}, {AttachmentStoreOperation::Store, AttachmentStoreOperation::DontCare},
        ImageLayout::ShaderReadOnly, ImageLayout::TransferDestination,
        32, AttachmentDescription::Flag::MayAlias};

    T out = Traits<T>::convert(description);
    CORRADE_COMPARE(out.flags, VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT);
    CORRADE_COMPARE(out.format, VK_FORMAT_R8G8B8A8_SNORM);
    CORRADE_COMPARE(out.samples, VK_SAMPLE_COUNT_32_BIT);
    CORRADE_COMPARE(out.loadOp, VK_ATTACHMENT_LOAD_OP_CLEAR);
    CORRADE_COMPARE(out.stencilLoadOp, VK_ATTACHMENT_LOAD_OP_DONT_CARE);
    CORRADE_COMPARE(out.storeOp, VK_ATTACHMENT_STORE_OP_STORE);
    CORRADE_COMPARE(out.stencilStoreOp, VK_ATTACHMENT_STORE_OP_DONT_CARE);
    CORRADE_COMPARE(out.initialLayout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    CORRADE_COMPARE(out.finalLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
}

void RenderPassTest::attachmentReferenceConstruct() {
    AttachmentReference reference{3, ImageLayout::ColorAttachment};
    CORRADE_COMPARE(reference->attachment, 3);
    CORRADE_COMPARE(reference->layout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

void RenderPassTest::attachmentReferenceConstructUnused() {
    AttachmentReference reference;
    CORRADE_COMPARE(reference->attachment, VK_ATTACHMENT_UNUSED);
    CORRADE_COMPARE(reference->layout, VK_IMAGE_LAYOUT_UNDEFINED);
}

void RenderPassTest::attachmentReferenceConstructNoInit() {
    AttachmentReference reference{NoInit};
    reference->sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    new(&reference) AttachmentReference{NoInit};
    CORRADE_COMPARE(reference->sType, VK_STRUCTURE_TYPE_APPLICATION_INFO);

    CORRADE_VERIFY((std::is_nothrow_constructible<AttachmentReference, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, AttachmentReference>::value));
}

template<class From, class To> void RenderPassTest::attachmentReferenceConstructFromVk() {
    setTestCaseTemplateName({Traits<From>::name(), Traits<To>::name()});

    From from{};
    from.attachment = 3;
    from.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    AttachmentReference reference{from};
    To to = Traits<To>::convert(reference);
    CORRADE_COMPARE(to.attachment, 3);
    CORRADE_COMPARE(to.layout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

template<class T> void RenderPassTest::attachmentReferenceConvertToVk() {
    setTestCaseTemplateName(Traits<T>::name());

    AttachmentReference reference{3, ImageLayout::ShaderReadOnly};
    T out = Traits<T>::convert(reference);
    CORRADE_COMPARE(out.attachment, 3);
    CORRADE_COMPARE(out.layout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void RenderPassTest::subpassDescriptionConstruct() {
    /** @todo use a real flag once it exists */
    SubpassDescription description{SubpassDescription::Flag(VK_INCOMPLETE)};
    CORRADE_COMPARE(description->flags, VK_INCOMPLETE);
    CORRADE_COMPARE(description->inputAttachmentCount, 0);
    CORRADE_VERIFY(!description->pInputAttachments);
    CORRADE_COMPARE(description->colorAttachmentCount, 0);
    CORRADE_VERIFY(!description->pColorAttachments);
    CORRADE_VERIFY(!description->pResolveAttachments);
    CORRADE_VERIFY(!description->pDepthStencilAttachment);
    CORRADE_COMPARE(description->preserveAttachmentCount, 0);
    CORRADE_VERIFY(!description->pPreserveAttachments);
}

void RenderPassTest::subpassDescriptionConstructNoInit() {
    SubpassDescription description{NoInit};
    description->sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    new(&description) SubpassDescription{NoInit};
    CORRADE_COMPARE(description->sType, VK_STRUCTURE_TYPE_APPLICATION_INFO);

    CORRADE_VERIFY((std::is_nothrow_constructible<SubpassDescription, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, SubpassDescription>::value));
}

void RenderPassTest::subpassDescriptionConstructInputAttachments() {
    SubpassDescription description;
    description.setInputAttachments({15, {}, 2});
    CORRADE_COMPARE(description->inputAttachmentCount, 3);
    CORRADE_VERIFY(description->pInputAttachments);
    CORRADE_COMPARE(description->pInputAttachments[0].attachment, 15);
    CORRADE_COMPARE(description->pInputAttachments[1].attachment, VK_ATTACHMENT_UNUSED);
    CORRADE_COMPARE(description->pInputAttachments[2].attachment, 2);
}

void RenderPassTest::subpassDescriptionConstructColorAttachments() {
    SubpassDescription description;
    description.setColorAttachments({{}, 23});
    CORRADE_COMPARE(description->colorAttachmentCount, 2);
    CORRADE_VERIFY(description->pColorAttachments);
    CORRADE_VERIFY(!description->pResolveAttachments);
    CORRADE_COMPARE(description->pColorAttachments[0].attachment, VK_ATTACHMENT_UNUSED);
    CORRADE_COMPARE(description->pColorAttachments[1].attachment, 23);
}

void RenderPassTest::subpassDescriptionConstructColorResolveAttachments() {
    SubpassDescription description;
    description.setColorAttachments({{}, 23}, {1, 0});

    CORRADE_COMPARE(description->colorAttachmentCount, 2);
    CORRADE_VERIFY(description->pColorAttachments);
    CORRADE_COMPARE(description->pColorAttachments[0].attachment, VK_ATTACHMENT_UNUSED);
    CORRADE_COMPARE(description->pColorAttachments[1].attachment, 23);
    CORRADE_VERIFY(description->pResolveAttachments);
    CORRADE_COMPARE(description->pResolveAttachments[0].attachment, 1);
    CORRADE_COMPARE(description->pResolveAttachments[1].attachment, 0);
}

void RenderPassTest::subpassDescriptionConstructColorResolveAttachmentsWrongCount() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    SubpassDescription description;

    std::ostringstream out;
    Error redirectError{&out};
    description.setColorAttachments({0, 1}, {2, 3, 5});
    CORRADE_COMPARE(out.str(),
        "Vk::SubpassDescription::setColorAttachments(): resolve attachments expected to be either empty or have a size of 2 but got 3\n");
}

void RenderPassTest::subpassDescriptionConstructDepthStencilAttachment() {
    SubpassDescription description;
    description.setDepthStencilAttachment(11);
    CORRADE_VERIFY(description->pDepthStencilAttachment);
    CORRADE_COMPARE(description->pDepthStencilAttachment->attachment, 11);
}

void RenderPassTest::subpassDescriptionConstructPreserveAttachments() {
    SubpassDescription description;
    description.setPreserveAttachments({1, 0, 3, 14});
    CORRADE_COMPARE(description->preserveAttachmentCount, 4);
    CORRADE_VERIFY(description->pPreserveAttachments);
    CORRADE_COMPARE(description->pPreserveAttachments[0], 1);
    CORRADE_COMPARE(description->pPreserveAttachments[1], 0);
    CORRADE_COMPARE(description->pPreserveAttachments[2], 3);
    CORRADE_COMPARE(description->pPreserveAttachments[3], 14);
}

void RenderPassTest::subpassDescriptionConstructEmptyAttachmentLists() {
    SubpassDescription description;
    description.setColorAttachments({}, {})
        .setInputAttachments({})
        .setPreserveAttachments({});
    CORRADE_COMPARE(description->inputAttachmentCount, 0);
    CORRADE_VERIFY(!description->pInputAttachments);
    CORRADE_COMPARE(description->colorAttachmentCount, 0);
    CORRADE_VERIFY(!description->pColorAttachments);
    CORRADE_VERIFY(!description->pResolveAttachments);
    CORRADE_COMPARE(description->preserveAttachmentCount, 0);
    CORRADE_VERIFY(!description->pPreserveAttachments);
}

template<class From, class To> void RenderPassTest::subpassDescriptionConstructFromVk() {
    setTestCaseTemplateName({Traits<From>::name(), Traits<To>::name()});

    typedef typename std::remove_const<typename std::remove_pointer<decltype(From{}.pInputAttachments)>::type>::type FromAttachmentReference;

    FromAttachmentReference inputAttachments[3]{};
    inputAttachments[0].attachment = 24;
    inputAttachments[0].layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL;
    inputAttachments[1].attachment = 35;
    inputAttachments[2].attachment = VK_ATTACHMENT_UNUSED;

    FromAttachmentReference colorAttachments[2]{};
    colorAttachments[0].attachment = 1;
    colorAttachments[1].attachment = 3;
    colorAttachments[1].layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;

    FromAttachmentReference resolveAttachments[2]{};
    resolveAttachments[0].attachment = 25;
    resolveAttachments[0].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    resolveAttachments[1].attachment = 12;

    FromAttachmentReference depthAttachment{};
    depthAttachment.attachment = 5;
    depthAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;

    UnsignedInt preserveAttachments[4]{0, 15, 23, 17};

    From from{};
    from.flags = VK_NOT_READY; /* wrong, but to set at least something */
    from.pipelineBindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
    from.inputAttachmentCount = 3;
    from.pInputAttachments = inputAttachments;
    from.colorAttachmentCount = 2;
    from.pColorAttachments = colorAttachments;
    from.pResolveAttachments = resolveAttachments;
    from.pDepthStencilAttachment = &depthAttachment;
    from.preserveAttachmentCount = 4;
    from.pPreserveAttachments = preserveAttachments;

    SubpassDescription description{from};
    Containers::Array<To> array = Traits<To>::convert(description);
    const To& to = array[0];
    CORRADE_COMPARE(to.flags, VK_NOT_READY); /* wrong, but to set at least something */
    CORRADE_COMPARE(to.pipelineBindPoint, VK_PIPELINE_BIND_POINT_COMPUTE);

    CORRADE_COMPARE(to.inputAttachmentCount, 3);
    CORRADE_VERIFY(to.pInputAttachments);
    CORRADE_COMPARE(to.pInputAttachments[0].attachment, 24);
    CORRADE_COMPARE(to.pInputAttachments[0].layout, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL);
    CORRADE_COMPARE(to.pInputAttachments[1].attachment, 35);
    CORRADE_COMPARE(to.pInputAttachments[2].attachment, VK_ATTACHMENT_UNUSED);

    CORRADE_COMPARE(to.colorAttachmentCount, 2);
    CORRADE_VERIFY(to.pColorAttachments);
    CORRADE_COMPARE(to.pColorAttachments[0].attachment, 1);
    CORRADE_COMPARE(to.pColorAttachments[1].attachment, 3);
    CORRADE_COMPARE(to.pColorAttachments[1].layout, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
    CORRADE_VERIFY(to.pResolveAttachments);
    CORRADE_COMPARE(to.pResolveAttachments[0].attachment, 25);
    CORRADE_COMPARE(to.pResolveAttachments[0].layout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    CORRADE_COMPARE(to.pResolveAttachments[1].attachment, 12);

    CORRADE_VERIFY(to.pDepthStencilAttachment);
    CORRADE_COMPARE(to.pDepthStencilAttachment->attachment, 5);
    CORRADE_COMPARE(to.pDepthStencilAttachment->layout, VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL);

    CORRADE_COMPARE(to.preserveAttachmentCount, 4);
    CORRADE_VERIFY(to.pPreserveAttachments);
    CORRADE_COMPARE(to.pPreserveAttachments[0], 0);
    CORRADE_COMPARE(to.pPreserveAttachments[1], 15);
    CORRADE_COMPARE(to.pPreserveAttachments[2], 23);
    CORRADE_COMPARE(to.pPreserveAttachments[3], 17);
}

void RenderPassTest::subpassDescriptionConstructCopy() {
    CORRADE_VERIFY(!(std::is_copy_constructible<SubpassDescription>{}));
    CORRADE_VERIFY(!(std::is_copy_assignable<SubpassDescription>{}));
}

void RenderPassTest::subpassDescriptionConstructMove() {
    SubpassDescription a;
    a.setInputAttachments({24, 35});
    CORRADE_COMPARE(a->inputAttachmentCount, 2);
    CORRADE_COMPARE(a->pInputAttachments[1].attachment, 35);

    SubpassDescription b{std::move(a)};
    CORRADE_COMPARE(a->inputAttachmentCount, 0);
    CORRADE_VERIFY(!a->pInputAttachments);
    CORRADE_COMPARE(b->inputAttachmentCount, 2);
    CORRADE_COMPARE(b->pInputAttachments[1].attachment, 35);

    SubpassDescription c;
    c = std::move(b);
    CORRADE_COMPARE(b->inputAttachmentCount, 0);
    CORRADE_VERIFY(!b->pInputAttachments);
    CORRADE_COMPARE(c->inputAttachmentCount, 2);
    CORRADE_COMPARE(c->pInputAttachments[1].attachment, 35);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<SubpassDescription>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<SubpassDescription>::value);
}

template<class T> void RenderPassTest::subpassDescriptionConvertToVk() {
    setTestCaseTemplateName(Traits<T>::name());

    SubpassDescription description{};
    description.setInputAttachments({
            24, {35, ImageLayout::ShaderReadOnly}, 17
        })
        .setColorAttachments({1, 3}, {{25, ImageLayout::ColorAttachment}, 12})
        .setDepthStencilAttachment({5, ImageLayout::DepthStencilAttachment})
        .setPreserveAttachments({0, 15, 23, 17});

    Containers::Array<T> array = Traits<T>::convert(description);
    const T& to = array[0];
    CORRADE_COMPARE(to.inputAttachmentCount, 3);
    CORRADE_VERIFY(to.pInputAttachments);
    CORRADE_COMPARE(to.pInputAttachments[0].attachment, 24);
    CORRADE_COMPARE(to.pInputAttachments[1].attachment, 35);
    CORRADE_COMPARE(to.pInputAttachments[1].layout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    CORRADE_COMPARE(to.pInputAttachments[2].attachment, 17);

    CORRADE_COMPARE(to.colorAttachmentCount, 2);
    CORRADE_VERIFY(to.pColorAttachments);
    CORRADE_COMPARE(to.pColorAttachments[0].attachment, 1);
    CORRADE_COMPARE(to.pColorAttachments[1].attachment, 3);
    CORRADE_VERIFY(to.pResolveAttachments);
    CORRADE_COMPARE(to.pResolveAttachments[0].attachment, 25);
    CORRADE_COMPARE(to.pResolveAttachments[0].layout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    CORRADE_COMPARE(to.pResolveAttachments[1].attachment, 12);

    CORRADE_VERIFY(to.pDepthStencilAttachment);
    CORRADE_COMPARE(to.pDepthStencilAttachment->attachment, 5);
    CORRADE_COMPARE(to.pDepthStencilAttachment->layout, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    CORRADE_COMPARE(to.preserveAttachmentCount, 4);
    CORRADE_VERIFY(to.pPreserveAttachments);
    CORRADE_COMPARE(to.pPreserveAttachments[0], 0);
    CORRADE_COMPARE(to.pPreserveAttachments[1], 15);
    CORRADE_COMPARE(to.pPreserveAttachments[2], 23);
    CORRADE_COMPARE(to.pPreserveAttachments[3], 17);
}

template<class T> void RenderPassTest::subpassDescriptionConvertToVkNoAttachments() {
    setTestCaseTemplateName(Traits<T>::name());

    SubpassDescription description;

    Containers::Array<T> array = Traits<T>::convert(description);
    const T& to = array[0];

    CORRADE_COMPARE(to.inputAttachmentCount, 0);
    CORRADE_VERIFY(!to.pInputAttachments);

    CORRADE_COMPARE(to.colorAttachmentCount, 0);
    CORRADE_VERIFY(!to.pColorAttachments);
    CORRADE_VERIFY(!to.pResolveAttachments);

    CORRADE_VERIFY(!to.pDepthStencilAttachment);

    CORRADE_COMPARE(to.preserveAttachmentCount, 0);
    CORRADE_VERIFY(!to.pPreserveAttachments);
}

template<class T> void RenderPassTest::subpassDescriptionConvertToVkNoResolveAttachments() {
    setTestCaseTemplateName(Traits<T>::name());

    SubpassDescription description{};
    description.setColorAttachments({1, 3});

    Containers::Array<T> array = Traits<T>::convert(description);
    const T& to = array[0];
    CORRADE_COMPARE(to.colorAttachmentCount, 2);
    CORRADE_VERIFY(to.pColorAttachments);
    CORRADE_COMPARE(to.pColorAttachments[0].attachment, 1);
    CORRADE_COMPARE(to.pColorAttachments[1].attachment, 3);
    CORRADE_VERIFY(!to.pResolveAttachments);
}

void RenderPassTest::subpassDescriptionRvalue() {
    SubpassDescription&& description = SubpassDescription{}
        .setInputAttachments(Containers::ArrayView<const AttachmentReference>{})
        .setInputAttachments(std::initializer_list<AttachmentReference>{})
        .setColorAttachments(Containers::ArrayView<const AttachmentReference>{})
        .setColorAttachments(std::initializer_list<AttachmentReference>{})
        .setColorAttachments(Containers::ArrayView<const AttachmentReference>{}, Containers::ArrayView<const AttachmentReference>{})
        .setColorAttachments(std::initializer_list<AttachmentReference>{}, std::initializer_list<AttachmentReference>{})
        .setDepthStencilAttachment({})
        .setPreserveAttachments(Containers::ArrayView<const UnsignedInt>{})
        .setPreserveAttachments(Containers::Array<UnsignedInt>{})
        .setPreserveAttachments(std::initializer_list<UnsignedInt>{});

    /* Just to test something, main point is that the above compiles, links and
       returns a &&. Can't test anything related to the contents because the
       destructor gets called at the end of the expression. */
    CORRADE_VERIFY(&description);
}

void RenderPassTest::subpassDependencyConstructNoInit() {
    SubpassDependency dependency{NoInit};
    dependency->sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    new(&dependency) SubpassDependency{NoInit};
    CORRADE_COMPARE(dependency->sType, VK_STRUCTURE_TYPE_APPLICATION_INFO);

    CORRADE_VERIFY((std::is_nothrow_constructible<SubpassDependency, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, SubpassDependency>::value));
}

template<class From, class To> void RenderPassTest::subpassDependencyConstructFromVk() {
    setTestCaseTemplateName({Traits<From>::name(), Traits<To>::name()});

    From from{};
    from.srcSubpass = 3;
    from.dstSubpass = 4;
    from.srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    from.dstStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
    from.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    from.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    from.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    SubpassDependency dependency{from};
    To to = Traits<To>::convert(dependency);
    CORRADE_COMPARE(to.srcSubpass, 3);
    CORRADE_COMPARE(to.dstSubpass, 4);
    CORRADE_COMPARE(to.srcStageMask, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
    CORRADE_COMPARE(to.dstStageMask, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT);
    CORRADE_COMPARE(to.srcAccessMask, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT);
    CORRADE_COMPARE(to.dstAccessMask, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT);
    CORRADE_COMPARE(to.dependencyFlags, VK_DEPENDENCY_BY_REGION_BIT);
}

template<class T> void RenderPassTest::subpassDependencyConvertToVk() {
    setTestCaseTemplateName(Traits<T>::name());

    CORRADE_SKIP("No SubpassDependency APIs to test.");
}

void RenderPassTest::createInfoConstruct() {
    /** @todo use a real flag once it exists */
    RenderPassCreateInfo info{RenderPassCreateInfo::Flag(VK_INCOMPLETE)};
    CORRADE_COMPARE(info->flags, VK_INCOMPLETE);
    CORRADE_COMPARE(info->attachmentCount, 0);
    CORRADE_VERIFY(!info->pAttachments);
    CORRADE_COMPARE(info->subpassCount, 0);
    CORRADE_VERIFY(!info->pSubpasses);
    CORRADE_COMPARE(info->dependencyCount, 0);
    CORRADE_VERIFY(!info->pDependencies);
}

void RenderPassTest::createInfoConstructNoInit() {
    RenderPassCreateInfo info{NoInit};
    info->sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    new(&info) RenderPassCreateInfo{NoInit};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_APPLICATION_INFO);

    CORRADE_VERIFY((std::is_nothrow_constructible<RenderPassCreateInfo, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, RenderPassCreateInfo>::value));
}

void RenderPassTest::createInfoConstructAttachments() {
    RenderPassCreateInfo info;
    info.setAttachments({
        {VK_FORMAT_R16G16B16A16_SFLOAT, AttachmentLoadOperation::Clear, AttachmentStoreOperation::DontCare},
        {VK_FORMAT_R8G8B8_SNORM, 4}
    });
    CORRADE_COMPARE(info->attachmentCount, 2);
    CORRADE_VERIFY(info->pAttachments);
    CORRADE_COMPARE(info->pAttachments[0].format, VK_FORMAT_R16G16B16A16_SFLOAT);
    CORRADE_COMPARE(info->pAttachments[0].loadOp, VK_ATTACHMENT_LOAD_OP_CLEAR);
    CORRADE_COMPARE(info->pAttachments[0].storeOp, VK_ATTACHMENT_STORE_OP_DONT_CARE);
    CORRADE_COMPARE(info->pAttachments[1].format, VK_FORMAT_R8G8B8_SNORM);
    CORRADE_COMPARE(info->pAttachments[1].samples, VK_SAMPLE_COUNT_4_BIT);
}

void RenderPassTest::createInfoConstructSubpasses() {
    RenderPassCreateInfo info;
    info.addSubpass(SubpassDescription{}
        .setColorAttachments({15, 34, 1})
        .setPreserveAttachments({22})
    );
    info.addSubpass(SubpassDescription{}
        .setInputAttachments({17, {}})
        .setDepthStencilAttachment(1)
    );
    CORRADE_COMPARE(info->subpassCount, 2);
    CORRADE_VERIFY(info->pSubpasses);
    CORRADE_COMPARE(info->pSubpasses[0].colorAttachmentCount, 3);
    CORRADE_VERIFY(info->pSubpasses[0].pColorAttachments);
    CORRADE_COMPARE(info->pSubpasses[0].pColorAttachments[1].attachment, 34);
    CORRADE_COMPARE(info->pSubpasses[0].preserveAttachmentCount, 1);
    CORRADE_VERIFY(info->pSubpasses[0].pPreserveAttachments);
    CORRADE_COMPARE(info->pSubpasses[0].pPreserveAttachments[0], 22);
    CORRADE_COMPARE(info->pSubpasses[1].inputAttachmentCount, 2);
    CORRADE_VERIFY(info->pSubpasses[1].pInputAttachments);
    CORRADE_COMPARE(info->pSubpasses[1].pInputAttachments[1].attachment, VK_ATTACHMENT_UNUSED);
    CORRADE_VERIFY(info->pSubpasses[1].pDepthStencilAttachment);
    CORRADE_COMPARE(info->pSubpasses[1].pDepthStencilAttachment->attachment, 1);
}

void RenderPassTest::createInfoConstructDependencies() {
    RenderPassCreateInfo info;
    /** @todo update once we have a real API */
    VkSubpassDependency2 a{};
    a.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    VkSubpassDependency2 b{};
    b.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    info.setDependencies({
        SubpassDependency{a},
        SubpassDependency{b}
    });
    CORRADE_COMPARE(info->dependencyCount, 2);
    CORRADE_VERIFY(info->pDependencies);
    CORRADE_COMPARE(info->pDependencies[0].sType, VK_STRUCTURE_TYPE_APPLICATION_INFO);
    CORRADE_COMPARE(info->pDependencies[1].sType, VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO);
}

template<class From, class To> void RenderPassTest::createInfoConstructFromVk() {
    setTestCaseTemplateName({Traits<From>::name(), Traits<To>::name()});

    VkRenderPassInputAttachmentAspectCreateInfo aspectInfo{};

    typedef typename std::remove_const<typename std::remove_pointer<decltype(From{}.pAttachments)>::type>::type FromAttachmentDescription;
    typedef typename std::remove_const<typename std::remove_pointer<decltype(From{}.pSubpasses)>::type>::type FromSubpassDescription;
    typedef typename std::remove_const<typename std::remove_pointer<decltype(From{}.pSubpasses->pColorAttachments)>::type>::type FromAttachmentReference;
    typedef typename std::remove_const<typename std::remove_pointer<decltype(From{}.pDependencies)>::type>::type FromSubpassDependency;

    FromAttachmentDescription attachments[2]{};
    attachments[0].format = VK_FORMAT_A1R5G5B5_UNORM_PACK16;
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    FromAttachmentReference ref[2]{};
    ref[0].attachment = 1;
    ref[1].attachment = 15;
    ref[1].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    UnsignedInt fiftyseven = 57;

    FromSubpassDescription subpasses[3]{};
    subpasses[0].colorAttachmentCount = 2;
    subpasses[0].pColorAttachments = ref;
    subpasses[1].pDepthStencilAttachment = &ref[1];
    subpasses[2].preserveAttachmentCount = 1;
    subpasses[2].pPreserveAttachments = &fiftyseven;

    FromSubpassDependency dependency{};
    dependency.srcAccessMask = VK_ACCESS_INDEX_READ_BIT;

    From from{};
    from.pNext = &aspectInfo;
    from.flags = VK_NOT_READY; /** @todo use a real flag once it exists */
    from.attachmentCount = 2;
    from.pAttachments = attachments;
    from.subpassCount = 3;
    from.pSubpasses = subpasses;
    from.dependencyCount = 1;
    from.pDependencies = &dependency;

    RenderPassCreateInfo info{from};
    Containers::Array<To> array = Traits<To>::convert(info);
    const To& to = array[0];
    CORRADE_COMPARE(to.pNext, &aspectInfo);
    CORRADE_COMPARE(to.flags, VK_NOT_READY);

    CORRADE_COMPARE(to.attachmentCount, 2);
    CORRADE_VERIFY(to.pAttachments);
    CORRADE_COMPARE(to.pAttachments[0].format, VK_FORMAT_A1R5G5B5_UNORM_PACK16);
    CORRADE_COMPARE(to.pAttachments[1].stencilStoreOp, VK_ATTACHMENT_STORE_OP_DONT_CARE);

    CORRADE_COMPARE(to.subpassCount, 3);
    CORRADE_VERIFY(to.pSubpasses);
    /* Test also that unset arrays stay 0/nullptr */
    CORRADE_COMPARE(to.pSubpasses[0].inputAttachmentCount, 0);
    CORRADE_VERIFY(!to.pSubpasses[0].pInputAttachments);
    CORRADE_COMPARE(to.pSubpasses[0].colorAttachmentCount, 2);
    CORRADE_VERIFY(to.pSubpasses[0].pColorAttachments);
    CORRADE_COMPARE(to.pSubpasses[0].pColorAttachments[0].attachment, 1);
    CORRADE_COMPARE(to.pSubpasses[0].pColorAttachments[1].attachment, 15);
    CORRADE_COMPARE(to.pSubpasses[0].pColorAttachments[1].layout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    CORRADE_VERIFY(to.pSubpasses[1].pDepthStencilAttachment);
    CORRADE_COMPARE(to.pSubpasses[1].pDepthStencilAttachment->attachment, 15);
    CORRADE_COMPARE(to.pSubpasses[1].pDepthStencilAttachment->layout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    CORRADE_COMPARE(to.pSubpasses[2].preserveAttachmentCount, 1);
    CORRADE_VERIFY(to.pSubpasses[2].pPreserveAttachments);
    CORRADE_COMPARE(to.pSubpasses[2].pPreserveAttachments[0], 57);

    CORRADE_COMPARE(to.dependencyCount, 1);
    CORRADE_VERIFY(to.pDependencies);
    CORRADE_COMPARE(to.pDependencies[0].srcAccessMask, VK_ACCESS_INDEX_READ_BIT);
}

void RenderPassTest::createInfoConstructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<RenderPassCreateInfo>{});
    CORRADE_VERIFY(!std::is_copy_assignable<RenderPassCreateInfo>{});
}

void RenderPassTest::createInfoConstructMove() {
    RenderPassCreateInfo a;
    a.setAttachments({VK_FORMAT_D32_SFLOAT, VK_FORMAT_R8G8B8_SNORM});
    CORRADE_COMPARE(a->attachmentCount, 2);
    CORRADE_COMPARE(a->pAttachments[1].format, VK_FORMAT_R8G8B8_SNORM);

    RenderPassCreateInfo b{std::move(a)};
    CORRADE_COMPARE(a->attachmentCount, 0);
    CORRADE_VERIFY(!a->pAttachments);
    CORRADE_COMPARE(b->attachmentCount, 2);
    CORRADE_COMPARE(b->pAttachments[1].format, VK_FORMAT_R8G8B8_SNORM);

    RenderPassCreateInfo c;
    c = std::move(b);
    CORRADE_COMPARE(b->attachmentCount, 0);
    CORRADE_VERIFY(!b->pAttachments);
    CORRADE_COMPARE(c->attachmentCount, 2);
    CORRADE_COMPARE(c->pAttachments[1].format, VK_FORMAT_R8G8B8_SNORM);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<RenderPassCreateInfo>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<RenderPassCreateInfo>::value);
}

template<class T> void RenderPassTest::createInfoConvertToVk() {
    VkSubpassDependency dependency{};
    dependency.srcAccessMask = VK_ACCESS_INDEX_READ_BIT;

    RenderPassCreateInfo info;
    info.setAttachments({
            AttachmentDescription{VK_FORMAT_A1R5G5B5_UNORM_PACK16},
            AttachmentDescription{{}, {}, {AttachmentStoreOperation::Store, AttachmentStoreOperation::DontCare}}
        })
        .addSubpass(SubpassDescription{}.setColorAttachments({1, {15, ImageLayout::ShaderReadOnly}}))
        .addSubpass(SubpassDescription{}.setDepthStencilAttachment({15, ImageLayout::ShaderReadOnly}))
        .addSubpass(SubpassDescription{}.setPreserveAttachments({57}))
        .setDependencies({SubpassDependency{dependency}});
    Containers::Array<T> array = Traits<T>::convert(info);
    const T& to = array[0];

    CORRADE_COMPARE(to.attachmentCount, 2);
    CORRADE_VERIFY(to.pAttachments);
    CORRADE_COMPARE(to.pAttachments[0].format, VK_FORMAT_A1R5G5B5_UNORM_PACK16);
    CORRADE_COMPARE(to.pAttachments[1].stencilStoreOp, VK_ATTACHMENT_STORE_OP_DONT_CARE);

    CORRADE_COMPARE(to.subpassCount, 3);
    CORRADE_VERIFY(to.pSubpasses);
    /* Test also that unset arrays stay 0/nullptr */
    CORRADE_COMPARE(to.pSubpasses[0].inputAttachmentCount, 0);
    CORRADE_VERIFY(!to.pSubpasses[0].pInputAttachments);
    CORRADE_COMPARE(to.pSubpasses[0].colorAttachmentCount, 2);
    CORRADE_VERIFY(to.pSubpasses[0].pColorAttachments);
    CORRADE_COMPARE(to.pSubpasses[0].pColorAttachments[0].attachment, 1);
    CORRADE_COMPARE(to.pSubpasses[0].pColorAttachments[1].attachment, 15);
    CORRADE_COMPARE(to.pSubpasses[0].pColorAttachments[1].layout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    CORRADE_VERIFY(to.pSubpasses[1].pDepthStencilAttachment);
    CORRADE_COMPARE(to.pSubpasses[1].pDepthStencilAttachment->attachment, 15);
    CORRADE_COMPARE(to.pSubpasses[1].pDepthStencilAttachment->layout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    CORRADE_COMPARE(to.pSubpasses[2].preserveAttachmentCount, 1);
    CORRADE_VERIFY(to.pSubpasses[2].pPreserveAttachments);
    CORRADE_COMPARE(to.pSubpasses[2].pPreserveAttachments[0], 57);

    CORRADE_COMPARE(to.dependencyCount, 1);
    CORRADE_VERIFY(to.pDependencies);
    CORRADE_COMPARE(to.pDependencies[0].srcAccessMask, VK_ACCESS_INDEX_READ_BIT);
}

void RenderPassTest::constructNoCreate() {
    {
        RenderPass renderPass{NoCreate};
        CORRADE_VERIFY(!renderPass.handle());
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoCreateT, RenderPass>::value));
}

void RenderPassTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<RenderPass>{});
    CORRADE_VERIFY(!std::is_copy_assignable<RenderPass>{});
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::RenderPassTest)
