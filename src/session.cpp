#include "pch.hpp"

#include "session.hpp"

#include "base.hpp"
#include "shader.hpp"
#include "window.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

GLuint buffer;
GLuint buffer_sector;
GLuint buffer_texture;
// GLuint buffer_normal;
GLuint buffer_depth;

u32 sector_t = 0;

#define uloc(name) glGetUniformLocation(s.ID, name)

Session::Session() {
    log_info("Starting");
    if (!window.init()) {
        throw std::runtime_error("Window failed to initialize");
    }

    store_glsl_helper("assets/model.glsl");

    camera.pos = { 2.5, 1.5, 2.5 };
    camera.rot = { -135, -32 };

    // mesh = MeshStatic::from_scene("assets/room.obj", 0, 0, 0);
    mesh = MeshStatic::from_scene("assets/sofa.obj", 0, 0, 0);
    mesh.apply_perlin({ 0, 0, 0 }, vec3(0.2));

    std::string path = "assets/antiquity16.png";

    int width, height, nr_channels;
    auto texture_data = stbi_load(path.c_str(), &width, &height, &nr_channels, 0);
    if (!texture_data) {
        throw std::runtime_error("Failed to load texture " + path);
        return;
    }

    glGenTextures(1, &sector_t);
    glBindTexture(GL_TEXTURE_2D, sector_t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 texture_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(texture_data);

    geo = Shader(
        "assets/sector.vs", "assets/sector.fs",
        [this](Shader &) {
            glGenFramebuffers(1, &buffer);
            glBindFramebuffer(GL_FRAMEBUFFER, buffer);

            glGenTextures(1, &buffer_sector);
            glBindTexture(GL_TEXTURE_2D, buffer_sector);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_W, SCREEN_H, 0, GL_RGB, GL_FLOAT,
                         nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                   buffer_sector, 0);

            glGenTextures(1, &buffer_texture);
            glBindTexture(GL_TEXTURE_2D, buffer_texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_W, SCREEN_H, 0, GL_RGB, GL_FLOAT,
                         nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                                   buffer_texture, 0);

            glGenTextures(1, &buffer_depth);
            glBindTexture(GL_TEXTURE_2D, buffer_depth);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, SCREEN_W, SCREEN_H, 0,
                         GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer_depth,
                                   0);

            GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
            glDrawBuffers(2, attachments);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                log_dang("Framebuffer not complete");
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void*)(offsetof(Vertex, pos)));
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void*)(offsetof(Vertex, normal)));
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void*)(offsetof(Vertex, uv)));
            glEnableVertexAttribArray(3);

            mesh.gl_buffer_data();
        },
        [](Shader &s, Session &se) {
            glUniformMatrix4fv(uloc("view_projection"), 1, GL_FALSE,
                               value_ptr(se.camera.perspective() * se.camera.view_matrix()));

            glBindFramebuffer(GL_FRAMEBUFFER, buffer);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sector_t);
            glUniform1i(uloc("sector_t"), 0);

            int n = 4;

            glUniform3f(uloc("color"), 0.99, 0.67, 0.12);

            for (int y = 0; y < n; y++) {
                for (int x = 0; x < n; x++) {
                    se.mesh.world_pos = { (float)x / 2, 0, (float)y / 2 };
                    se.mesh.rotation.y += DELTA_TIME / 32;
                    se.mesh.gl_uniforms(s.ID);
                    glDrawElements(GL_TRIANGLES, se.mesh.indices.size(), GL_UNSIGNED_SHORT, 0);
                }
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        });
    result = Shader(
        "assets/sector_flat.vs", "assets/sector_flat.fs",
        [](Shader &) {
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                                  (void*)(2 * sizeof(float)));
            glEnableVertexAttribArray(1);

            float quad_verts[] = { -1, 1, 0, 1, -1, -1, 0, 0, 1, -1, 1, 0,
                                   -1, 1, 0, 1, 1,  -1, 1, 0, 1, 1,  1, 1 };

            glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), quad_verts, GL_STATIC_DRAW);
        },
        [](Shader &s, Session &) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, buffer_sector);
            glUniform1i(uloc("sector_t"), 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, buffer_texture);
            glUniform1i(uloc("texture_t"), 1);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, buffer_depth);
            glUniform1i(uloc("depth_t"), 2);

            glUniform2f(uloc("texel_size"), 1.0f / SCREEN_W, 1.0f / SCREEN_H);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        });

    geo.init();
    result.init();

    log_info("Started in %s (total)", time_to_string(timer.stop()).c_str());
}

void Session::update() {
    window.update();
    running = input.update(camera);
    camera.update();
}

void Session::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    geo.render(*this);
    result.render(*this);

    window.swap();
}
