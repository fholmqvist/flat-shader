#include "pch.hpp"

#include "base.hpp"
#include "shader.hpp"

void Shader::init() {
    if (ID == 0) {
        ID = glCreateProgram();
        compile_shaders(ID, vertex_source, fragment_source);
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
    }
    if (init_func) {
        bind();
        init_func(*this);
        unbind();
    }
}

void Shader::render(Session &s) {
    assert(ID);
    bind();
    draw_func(*this, s);
    unbind();
}

void Shader::bind() {
    glUseProgram(ID);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}

void Shader::unbind() {
    glBindVertexArray(0);
    glUseProgram(0);
}

std::unordered_map<std::string, std::string> gl_functions;

void store_glsl_helper(std::string path) {
    std::string result;
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error(
            frmt("Storing GLSL helper file: Could not open file \"%s\"", path.c_str()));
    }

    std::string line;
    while (std::getline(file, line)) {
        result += line + "\n";
    }

    file.close();

    gl_functions[path.substr(path.find_last_of("/") + 1)] = result;
}

void read_shader_file(std::string &result, std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error(
            frmt("Reading shader: Could not open file \"%s\"", filename.c_str()));
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.starts_with("#include ")) {
            const auto start = line.find("\"") + 1;
            const auto end = line.rfind("\"");
            const auto name = line.substr(start, end - start);
            result += gl_functions[name];
            continue;
        }

        result += line + "\n";
    }

    file.close();
}

void compile_shaders(u32 shader_id, std::string vertex_source, std::string fragment_source) {
    int success;

    u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    std::string vertex_shader_source;
    read_shader_file(vertex_shader_source, vertex_source);
    const char* vertex_shader_source_cstr = vertex_shader_source.c_str();
    glShaderSource(vertex_shader, 1, &vertex_shader_source_cstr, nullptr);
    glCompileShader(vertex_shader);

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
        throw std::runtime_error("Vertex shader compilation failed!\n" + std::string(info_log));
    }

    u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    std::string fragment_shader_source;
    read_shader_file(fragment_shader_source, fragment_source);
    const char* fragment_shader_source_cstr = fragment_shader_source.c_str();
    glShaderSource(fragment_shader, 1, &fragment_shader_source_cstr, nullptr);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
        throw std::runtime_error("Fragment shader compilation failed!\n" + std::string(info_log));
    }

    glAttachShader(shader_id, vertex_shader);
    glAttachShader(shader_id, fragment_shader);
    glLinkProgram(shader_id);

    glGetProgramiv(shader_id, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(shader_id, 512, nullptr, info_log);
        throw std::runtime_error("Shader program linking failed!\n" + std::string(info_log));
    }
}
