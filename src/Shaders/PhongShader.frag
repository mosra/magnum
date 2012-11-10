#ifndef NEW_GLSL
#define in varying
#define color gl_FragColor
#endif

uniform lowp vec3 ambientColor = vec3(0.0, 0.0, 0.0);
uniform lowp vec3 diffuseColor;
uniform lowp vec3 specularColor = vec3(1.0, 1.0, 1.0);
uniform lowp vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform mediump float shininess = 80.0;

in mediump vec3 transformedNormal;
in highp vec3 lightDirection;
in highp vec3 cameraDirection;

#ifdef NEW_GLSL
out lowp vec4 color;
#endif

void main() {
    /* Ambient color */
    color.rgb = ambientColor;

    mediump vec3 normalizedTransformedNormal = normalize(transformedNormal);
    highp vec3 normalizedLightDirection = normalize(lightDirection);

    /* Add diffuse color */
    lowp float intensity = max(0.0, dot(normalizedTransformedNormal, normalizedLightDirection));
    color.rgb += diffuseColor*lightColor*intensity;

    /* Add specular color, if needed */
    if(intensity != 0) {
        highp vec3 reflection = reflect(-normalizedLightDirection, normalizedTransformedNormal);
        mediump float specularity = pow(max(0.0, dot(normalize(cameraDirection), reflection)), shininess);
        color.rgb += specularColor*specularity;
    }

    /* Force alpha to 1 */
    color.a = 1.0;
}
