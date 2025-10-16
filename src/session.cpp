#include "pch.hpp"

#include "session.hpp"

#include "base.hpp"
#include "shader.hpp"
#include "window.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

GLuint buffer;
GLuint color_t;
// GLuint normal_t;
// GLuint depth_t;

u32 sector_t = 0;

#define uloc(name) glGetUniformLocation(s.ID, name)

Session::Session() {
    log_info("Starting");
    if (!window.init()) {
        throw std::runtime_error("Window failed to initialize");
    }

    store_glsl_helper("assets/model.glsl");

    // camera.pos = { 1.4, 1, 1.2 };
    camera.pos = { 0.4, 0.15, 0.8 };
    // camera.rot = { -140, -25 };
    camera.rot = { -120, -6.5 };

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

            glGenTextures(1, &color_t);
            glBindTexture(GL_TEXTURE_2D, color_t);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_W, SCREEN_H, 0, GL_RGB, GL_FLOAT,
                         nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_t, 0);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void*)(offsetof(Vertex, pos)));
            glEnableVertexAttribArray(0);

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

            se.mesh.gl_uniforms(s.ID);
            glDrawElements(GL_TRIANGLES, se.mesh.indices.size(), GL_UNSIGNED_SHORT, 0);

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
            glBindTexture(GL_TEXTURE_2D, color_t);
            glUniform1i(uloc("t"), 0);

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

/*

geo = Shader(
        "assets/flat_geo.vs", "assets/flat_geo.fs",
        [this](Shader &) {
            glGenFramebuffers(1, &buffer);
            glBindFramebuffer(GL_FRAMEBUFFER, buffer);

            // main color
            glGenTextures(1, &color_t);
            glBindTexture(GL_TEXTURE_2D, color_t);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_W, SCREEN_H, 0, GL_RGB, GL_FLOAT,
                         nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_t, 0);

            // normals
            glGenTextures(1, &normal_t);
            glBindTexture(GL_TEXTURE_2D, normal_t);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_W, SCREEN_H, 0, GL_RGB, GL_FLOAT,
                         nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal_t,
                                   0);

            // depth
            glGenTextures(1, &depth_t);
            glBindTexture(GL_TEXTURE_2D, depth_t);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, SCREEN_W, SCREEN_H, 0,
                         GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_t, 0);

            GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
            glDrawBuffers(2, attachments);
            assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void*)(offsetof(Vertex, pos)));
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void*)(offsetof(Vertex, normal)));
            glEnableVertexAttribArray(1);

            mesh.gl_buffer_data();
        },
        [](Shader &s, Session &se) {
            glUniformMatrix4fv(uloc("view"), 1, GL_FALSE, value_ptr(se.camera.view_matrix()));
            glUniformMatrix4fv(uloc("projection"), 1, GL_FALSE, value_ptr(se.camera.perspective()));

            glBindFramebuffer(GL_FRAMEBUFFER, buffer);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            const int n = 1;

            for (int y = 0; y < n; y++) {
                for (int x = 0; x < n; x++) {
                    se.mesh.world_pos = { x, 0, y * 1.45 };
                    se.mesh.gl_uniforms(s.ID);
                    glDrawElements(GL_TRIANGLES, se.mesh.indices.size(), GL_UNSIGNED_SHORT, 0);
                }
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        });
    result = Shader(
        "assets/flat.vs", "assets/flat.fs",
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
        [](Shader &s, Session &se) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, depth_t);
            glUniform1i(uloc("depth_t"), 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, normal_t);
            glUniform1i(uloc("normal_t"), 1);

            glUniform2f(uloc("texel_size"), 1.0f / SCREEN_W, 1.0f / SCREEN_H);

            glUniform2f(uloc("resolution"), SCREEN_W, SCREEN_H);

            glUniformMatrix4fv(uloc("inv_proj"), 1, GL_FALSE,
                               value_ptr(inverse(se.camera.perspective())));

            glDrawArrays(GL_TRIANGLES, 0, 6);
        });

*/
