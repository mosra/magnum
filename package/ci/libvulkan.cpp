/* Yes. I know what I am doing. Yes. Really. */

extern "C" {

int vkCreateInstance(const void*, const void*, void*) {
    return -9;
}

int vkEnumerateInstanceExtensionProperties(const void*, void*, void*) {
    return -9;
}

int vkEnumerateInstanceLayerProperties(void*, void*) {
    return -9;
}

void(*vkGetInstanceProcAddr(void*, const void*))() {
    return nullptr;
}

}
