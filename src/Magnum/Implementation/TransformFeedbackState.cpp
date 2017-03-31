#include "TransformFeedbackState.h"

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/TransformFeedback.h"

#include "State.h"

namespace Magnum { namespace Implementation {

TransformFeedbackState::TransformFeedbackState(Context& context, std::vector<std::string>& extensions): maxInterleavedComponents{0}, maxSeparateAttributes{0}, maxSeparateComponents{0}
    #ifndef MAGNUM_TARGET_GLES
    , maxBuffers{0}, maxVertexStreams{0}
    #endif
{
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::ARB::direct_state_access>()) {
        extensions.emplace_back(Extensions::GL::ARB::direct_state_access::string());

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

}}
