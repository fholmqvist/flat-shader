mat4 translate(mat4 m, vec3 v) {
    m[3] = m[3] + vec4(v, 0.0);
    return m;
}

mat4 mscale(mat4 m, vec3 v) {
    m[0] *= v.x;
    m[1] *= v.y;
    m[2] *= v.z;
    return m;
}

mat4 rotate(mat4 m, float angle, vec3 axis) {
    float c = cos(angle);
    float s = sin(angle);
    vec3 a = normalize(axis);
    vec3 temp = (1 - c) * a;

    mat4 rot = mat4(1);
    rot[0][0] = c + temp.x * a.x;
    rot[0][1] = temp.x * a.y + s * a.z;
    rot[0][2] = temp.x * a.z - s * a.y;

    rot[1][0] = temp.y * a.x - s * a.z;
    rot[1][1] = c + temp.y * a.y;
    rot[1][2] = temp.y * a.z + s * a.x;

    rot[2][0] = temp.z * a.x + s * a.y;
    rot[2][1] = temp.z * a.y - s * a.x;
    rot[2][2] = c + temp.z * a.z;

    return m * rot;
}

mat4 model_matrix(vec3 position, vec3 rotation, float scale) {
    mat4 m = mat4(1);
    m = translate(m, position);
    m = rotate(m, rotation.y, vec3(0, 1, 0));
    m = rotate(m, rotation.x, vec3(1, 0, 0));
    m = rotate(m, rotation.z, vec3(0, 0, 1));
    m = mscale(m, vec3(scale));
    return m;
}
