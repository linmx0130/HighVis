#version 330
in vec2 texCoord;
in vec3 normalVec;
in vec3 fragPosition;
out vec4 fragColor;

uniform sampler3D ourTexture;
uniform vec3 dataSize;
// lookFrom: from where you look at the 3D cube. In the coordination of data
uniform vec3 lookFrom;
// lookAt: you look at what point. In the coordination of data
uniform vec3 lookAt;
// lookUpVec: up vector for the view
uniform vec3 lookUpVec;
// lightPos: light source fragPosition
uniform vec3 lightPos;
uniform float diffuseK;
uniform vec4 lightColor;
// interpolationType: control what interpolation will be used
// 0 for bezier tricubic interpolation
// 1 for trilinear interpolation
uniform float interpolationType;
uniform float alphaThreshold;
bool inDataCube(vec3 point) {
    return ((point.x >= 0.0f) && (point.x <= 1.0f) && (point.y >= 0.0f) && (point.y <=1.0f) && (point.z >= 0.0f) && (point.z <=1.0f));
}
float[8] getNearestValues(vec3 coord) {
    float ret[8];
    vec3 scaledCoord = coord * dataSize;
    for (int i=0;i<2;++i) {
        for (int j=0;j<2;++j) {
            for (int k=0;k<2;++k) {
                vec3 newCoord = floor(scaledCoord + vec3(i, j, k)) / dataSize;
                int idx = (i << 2) | (j << 1) | k;
                ret[idx] = texture(ourTexture, newCoord).r;
            }
        }
    }
    return ret;
}
vec3[8] getNearestGradients(vec3 coord) {
    vec3 ret[8];
    vec3 scaledCoord = coord * dataSize;
    for (int i=0;i<2;++i) {
        for (int j=0;j<2;++j) {
            for (int k=0;k<2;++k) {
                vec3 dp = vec3(i, j, k);
                vec3 p0 = floor(scaledCoord + dp) / dataSize;
                vec3 p1 = floor(scaledCoord + dp + 1.0f) / dataSize;
                vec3 p2 = floor(scaledCoord + dp - 1.0f) / dataSize;
                float gx = (texture(ourTexture, vec3(p1.x, p0.y, p0.z)).r - texture(ourTexture, vec3(p2.x, p0.y, p0.z)).r);
                float gy = (texture(ourTexture, vec3(p0.x, p1.y, p0.z)).r - texture(ourTexture, vec3(p0.x, p2.y, p0.z)).r);
                float gz = (texture(ourTexture, vec3(p0.x, p0.y, p1.z)).r - texture(ourTexture, vec3(p0.x, p0.y, p2.z)).r);
                ret[i*4 + j*2+k] = vec3(gx, gy, gz) * 0.5f;
            }
        }
    }
    return ret;
}
float[64] getBezierControlPoints(vec3 coord) {
    float fValue[8] = getNearestValues(coord);
    vec3 gradient[8] = getNearestGradients(coord);
    float ret[64];
    for (int i=0;i<4;++i) {
        for (int j=0;j<4;++j) {
            for (int k=0;k<4;++k) {
                int oi = i / 2;
                int oj = j / 2;
                int ok = k / 2;
                int fCoord = oi * 4 + oj * 2 + ok /2;
                vec3 gd = gradient[fCoord] * 0.333f;
                // float tmp = fValue[fCoord] + gW[i] * gd.x + gW[j] * gd.y + gW[k] * gd.z;
                float tmp = fValue[fCoord];
                if (i==1) tmp += gd.x;
                if (i==2) tmp -= gd.x;
                if (j==1) tmp += gd.y;
                if (j==2) tmp -= gd.y;
                if (k==1) tmp += gd.z;
                if (k==2) tmp -= gd.z;
                ret[i * 16 + j * 4 + k] = tmp;
            }
        }
    }
    return ret;
}
float bezier3(int i, float x){
    if (i==0) {
        return (1.0f-x) * (1.0f-x) * (1.0f-x);
    }
    if (i==1) {
        return 3.0f * x * (1.0f-x) * (1.0f-x);
    }
    if (i==2) {
        return 3.0f * x * x * (1.0f-x);
    }
    return x * x* x;
}
float bezier2(int i, float x) {
    if (i==0) {
        return (1.0f-x) * (1.0f-x);
    }
    if (i==1) {
        return 2 * x * (1.0f - x);
    }
    return x * x;
}
float bezierTricubicApprox(float[64]beziers, vec3 coord) {
    vec3 delta = coord * dataSize;
    delta = delta - floor(delta);
    float ret = 0.0f;
    for (int i=0;i<4;++i) {
        for (int j=0;j<4; ++j) {
            for (int k=0;k<4; ++k) {
                int idx = (i << 4) | (j<<2) | k;
                float term = beziers[idx] * bezier3(i, delta.x) * bezier3(j, delta.y) * bezier3(k, delta.z);
                ret += term;
            }
        }
    }
    return ret;
}
vec3 bezierTricubicGradient(float[64]beziers, vec3 coord) {
    vec3 delta = coord * dataSize;
    delta = delta - floor(delta);
    vec3 ret;
    float tmp = 0.0f;
    for (int i=0;i<3;++i) {
        for (int j=0;j<4;++j) {
            for (int k=0;k<4;++k) {
                float term = beziers[(i+1) * 16 + j * 4 + k] - beziers[i * 16 + j * 4 + k];
                term = term * bezier2(i, delta.x) * bezier3(j, delta.y) * bezier3(k, delta.z);
                tmp += term;
            }
        }
    }
    ret.x = tmp;
    tmp = 0.0f;
    for (int i=0;i<4;++i) {
        for (int j=0;j<3;++j) {
            for (int k=0;k<4;++k) {
                float term = beziers[i * 16 + (j+1) * 4 + k] - beziers[i * 16 + j * 4 + k];
                term = term * bezier3(i, delta.x) * bezier2(j, delta.y) * bezier3(k, delta.z);
                tmp += term;
            }
        }
    }
    ret.y = tmp;
    tmp = 0.0f;
    for (int i=0;i<4;++i) {
        for (int j=0;j<4;++j) {
            for (int k=0;k<3;++k) {
                float term = beziers[i * 16 + j * 4 + k+1] - beziers[i * 16 + j * 4 + k];
                term = term * bezier3(i, delta.x) * bezier3(j, delta.y) * bezier2(k, delta.z);
                tmp += term;
            }
        }
    }
    ret.z = tmp;
    ret = ret * 3.0f;
    return ret;
}
float getTrilinearApprox(float[8] f, vec3 coord){
    vec3 delta = coord * dataSize;
    delta = delta - floor(delta);
    float ret = 0.0f;
    ret += f[0] * (1.0f-delta.x) * (1.0f-delta.y) * (1.0f-delta.z);
    ret += f[1] * (1.0f-delta.x) * (1.0f-delta.y) * delta.z;
    ret += f[2] * (1.0f-delta.x) * delta.y * (1.0f-delta.z);
    ret += f[3] * (1.0f-delta.x) * delta.y * delta.z;
    ret += f[4] * delta.x * (1.0f-delta.y) * (1.0f-delta.z);
    ret += f[5] * delta.x * (1.0f-delta.y) * delta.z;
    ret += f[6] * delta.x * delta.y * (1.0f-delta.z);
    ret += f[7] * delta.x * delta.y * delta.z;
    return ret;
}
vec3 getTrilinearGApprox(float[8] f, vec3 coord){
    vec3 delta = coord * dataSize;
    delta = delta - floor(delta);
    vec3 ret = vec3(0.0f, 0.0f, 0.0f);
    for (int i=0;i<=1;++i) {
        for (int j=0;j<=1;++j) {
            for (int k=0;k<=1;++k) {
                int idx = i*4 + j *2 + k;
                float dx = (1.0f - delta.y) * (1.0f - j) + delta.y * j;
                dx *= (1.0f - delta.z) * (1.0f - k) + delta.z * k;
                dx *= f[idx];
                dx *= (i==0)?-1.0f : 1.0f;
                ret.x += dx;
                float dy = (1.0f - delta.x) * (1.0f - i) + delta.x * i;
                dy *= (1.0f - delta.z) * (1.0f - k) + delta.z * k;
                dy *= f[idx];
                dy *= (j==0)? -1.0f: 1.0f;
                ret.y += dy;
                float dz = (1.0f - delta.y)* (1.0f - j) + delta.y * j;
                dz *= (1.0f - delta.x) * (1.0f-i) + delta.x * i;
                dz *= (k==0)? -1.0f: 1.0f;
                ret.y += dz;
            }
        }
    }
    return ret;
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
    if (interpolationType < 0.5f){
        while (t >=0.0f){
            vec3 newCoord = myPos + t * lookVec;
            if (inDataCube(newCoord)) {
                float currentAlpha = 0.0f;
                float[64] beziers = getBezierControlPoints(newCoord);
                currentAlpha = bezierTricubicApprox(beziers, newCoord);
                if (currentAlpha >= alphaThreshold) {
                    vec3 norm = normalize(bezierTricubicGradient(beziers, newCoord));
                    vec3 lightVec = normalize(lightPos - newCoord);
                    float currentColor = diffuseK * dot(lightVec, norm) + (1-diffuseK);
                    tempColor = tempColor * (1-currentAlpha) + currentAlpha * currentColor;
                }
            }
            t = t - .0005f;
        }
    } else {
        while (t >=0.0f){
            vec3 newCoord = myPos + t * lookVec;
            if (inDataCube(newCoord)) {
                float currentAlpha = 0.0f;
                float[8] nearestF = getNearestValues(newCoord);
                currentAlpha = getTrilinearApprox(nearestF, newCoord);
                if (currentAlpha >= alphaThreshold) {
                    vec3 norm = normalize(getTrilinearGApprox(nearestF, newCoord));
                    vec3 lightVec = normalize(lightPos - newCoord);
                    float currentColor = diffuseK * dot(lightVec, norm) + (1-diffuseK);
                    tempColor = tempColor * (1-currentAlpha) + currentAlpha * currentColor;
                }
            }
            t = t - .0005f;
        }
    }
    fragColor = tempColor * lightColor;
}
