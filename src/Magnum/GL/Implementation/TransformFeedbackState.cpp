#include "TransformFeedbackState.h"

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/TransformFeedback.h"
#include "Magnum/GL/Implementation/State.h"

namespace Magnum { namespace GL { namespace Implementation {

TransformFeedbackState::TransformFeedbackState(Context& context, std::vector<std::string>& extensions): maxInterleavedComponents{0}, maxSeparateAttributes{0}, maxSeparateComponents{0},
    #ifndef MAGNUM_TARGET_GLES
    maxBuffers{0}, maxVertexStreams{0},
    #endif
    binding{0}
{
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()) {
        extensions.emplace_back(Extensions::ARB::direct_state_access::string());

        createImplementation = &TransformFeedback::createImplementationDSA;
        attachRangeImplementation = &TransformFeedback::attachImplementationDSA;
        attachBaseImplementation = &TransformFeedback::attachImplementationDSA;
        attachRangesImplementation = &TransformFeedback::attachImplementationDSA;
        attachBasesImplementation = &TransformFeedback::attachImplementationDSA;

    } else
    #endif
    {
        createImplementation = &TransformFeedback::createImplementationDefault;
        attachRangeImplementation = &TransformFeedback::attachImplementationFallback;
        attachBaseImplementation = &TransformFeedback::attachImplementationFallback;
        attachRangesImplementation = &TransformFeedback::attachImplementationFallback;
        attachBasesImplementation = &TransformFeedback::attachImplementationFallback;
    }

    #ifdef MAGNUM_TARGET_GLES
    static_cast<void>(context);
    static_cast<void>(extensions);
    #endif
}

void TransformFeedbackState::reset() {
    binding = State::DisengagedBinding;
}

}}}
