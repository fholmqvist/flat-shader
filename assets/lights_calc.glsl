vec3 dir_light_calc(DirectionalLight l, vec3 normal, vec3 view_dir, vec3 diffuse) {
    vec3 light_dir = normalize(-l.dir);
    float diff = max(dot(normal, light_dir), 0);
    return l.color * diff * diffuse * l.intensity;;
}
