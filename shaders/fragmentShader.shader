#version 330
in vec2 texCoord;
in vec3 normalVec;
in vec3 fragPosition;
out vec4 fragColor;

uniform sampler3D ourTexture;
// lookFrom: from where you look at the 3D cube. In the coordination of data
uniform vec3 lookFrom;
// lookAt: you look at what point. In the coordination of data
uniform vec3 lookAt;
// lookUpVec: up vector for the view
uniform vec3 lookUpVec;

// compute the angles with respect to the origin
vec2 computeArcAngle(vec3 point){
    return vec2(atan(point.y, point.x), atan(point.z, length(vec2(point.x, point.y))));
}
bool inDataCube(vec3 point) {
    return ((point.x >= 0.0f) && (point.x <= 1.0f) && (point.y >= 0.0f) && (point.y <=1.0f) && (point.z >= 0.0f) && (point.z <=1.0f));
}
void main() {
    // compute the distance from lookFrom to lookAt
    vec3 lookVec = lookAt - lookFrom;
    float distance = length(lookVec);
    // compute look at information
    vec3 lookUp = normalize(lookUpVec);
    vec3 lookRight = normalize(cross(lookVec, lookUp));
    //compute the real position for this pixel
    vec3 myPos = lookAt + lookUp * (texCoord.y - 0.5f) + lookRight * (texCoord.x - 0.5f);
    //vec3 oppositePos = myPos + lookVec * 2;
    float t = 2.0f;
    float tempColor = 0.0f;
    while (t >=0.0f){
        vec3 newCoord = myPos + t * lookVec;
        float currentAlpha = 0.0f;
        if (inDataCube(newCoord)) {
            currentAlpha = texture(ourTexture, newCoord).r;
            if (currentAlpha < 0.15) currentAlpha = 0.0f;
        }
        tempColor = tempColor * (1-currentAlpha) + currentAlpha * 1.0f;
        t = t - .001f;
    }
    // vec3 newCoord = vec3(zCoord, texCoord.y, texCoord.x);
    // fragColor = texture(ourTexture, newCoord).r * vec4(1.0f, 1.0f, 1.0f, 1.0f);
    fragColor = tempColor * vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
