#version 330

uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float shininess;

uniform vec3 lightAmbientColor;
uniform vec3 lightDiffuseColor;
uniform vec3 lightSpecularColor;

in vec3 transformedNormal;
in vec3 lightDirection;

out vec4 color;

void main() {
    /* Ambient color */
    color.rgb = ambientColor*lightAmbientColor;

    /* Add diffuse color */
    float intensity = max(0.0, dot(transformedNormal, lightDirection));
    color.rgb += diffuseColor*lightDiffuseColor*intensity;

    /* Add specular color, if needed */
    if(intensity != 0) {
        vec3 reflection = reflect(-lightDirection, transformedNormal);
        float specularity = pow(max(0.0, dot(transformedNormal, reflection)), shininess);
        color.rgb += specularColor*lightSpecularColor*specularity;
    }

    /* Force alpha to 1 */
    color.a = 1.0;
}
