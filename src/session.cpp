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
GLuint buffer_depth;
GLuint buffer2;
GLuint buffer2_color;

vec3 LIGHT_DIR(0, -1, 0);
mat4 LIGHT_SPACE;

#define uloc(name) glGetUniformLocation(s.ID, name)

void update_light_space();

Session::Session() {
    log_info("Starting");

    if (!window.init()) {
        throw std::runtime_error("Window failed to initialize");
    }

    load_glsl_helpers();

    load_objects();

    generate_buffers();

    camera.pos = { 0.7, 1.1, 1.2 };
    camera.rot = { -120, -30 };

    geo = Shader(
        "assets/sector.vs", "assets/sector.fs",
        [](Shader &) {
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void*)(offsetof(Vertex, pos)));
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void*)(offsetof(Vertex, normal)));
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void*)(offsetof(Vertex, uv)));
            glEnableVertexAttribArray(2);

            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void*)(offsetof(Vertex, sector_color)));
            glEnableVertexAttribArray(3);
        },
        [](Shader &s, Session &se) {
            glBindFramebuffer(GL_FRAMEBUFFER, buffer);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUniformMatrix4fv(uloc("view_projection"), 1, GL_FALSE,
                               value_ptr(se.camera.perspective() * se.camera.view_matrix()));
            glUniform3f(uloc("view_pos"), se.camera.pos.x, se.camera.pos.y, se.camera.pos.z);

            glUniform1f(uloc("dir_light.intensity"), 1);
            glUniform3f(uloc("dir_light.dir"), 0, -1, -1);

            glUniform3f(uloc("color"), 0.99, 0.67, 0.12);

            se.sofa.position = vec3(-0.2, 0, 0);
            se.sofa.rotation.y = DEG2RAD(0);
            se.sofa.draw(s.ID);
            se.sofa.position = vec3(0.2, 0, 0);
            se.sofa.rotation.y = DEG2RAD(180);
            se.sofa.draw(s.ID);

            se.table.draw(s.ID);

            se.chair.position = vec3(0, 0, -0.5);
            se.chair.rotation.y = DEG2RAD(-90);
            se.chair.draw(s.ID);

            se.chair.position = vec3(0, 0, 0.3);
            se.chair.rotation.y = DEG2RAD(90);
            se.chair.draw(s.ID);

            se.desk.position = vec3(0, 0, -0.35);
            se.desk.rotation.y = DEG2RAD(-90);
            se.desk.draw(s.ID);

            se.bookshelf.position = vec3(-0.5, 0, -0.25);
            se.bookshelf.draw(s.ID);

            se.room.position.z = 0.1;
            se.room.draw(s.ID);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        });

    shadows = Shader(
        "assets/shadows.vs", "assets/shadows.fs",
        [](Shader &) {
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void*)(offsetof(Vertex, pos)));
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void*)(offsetof(Vertex, normal)));
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void*)(offsetof(Vertex, uv)));
            glEnableVertexAttribArray(2);
        },
        [](Shader &s, Session &se) {
            // TODO: Depth shader needs to use light information.
            glBindFramebuffer(GL_FRAMEBUFFER, buffer2);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUniformMatrix4fv(uloc("view_projection"), 1, GL_FALSE,
                               value_ptr(se.camera.perspective() * se.camera.view_matrix()));

            update_light_space();
            glUniformMatrix4fv(uloc("light_space"), 1, GL_FALSE, &LIGHT_SPACE[0][0]);
            glUniform3fv(uloc("light_dir"), 1, &LIGHT_DIR[0]);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, buffer_sector);
            glUniform1i(glGetUniformLocation(s.ID, "sector_t"), 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, buffer_depth);
            glUniform1i(glGetUniformLocation(s.ID, "shadow_t"), 1);

            se.sofa.position = vec3(-0.2, 0, 0);
            se.sofa.rotation.y = DEG2RAD(0);
            se.sofa.draw(s.ID);
            se.sofa.position = vec3(0.2, 0, 0);
            se.sofa.rotation.y = DEG2RAD(180);
            se.sofa.draw(s.ID);

            se.table.draw(s.ID);

            se.chair.position = vec3(0, 0, -0.5);
            se.chair.rotation.y = DEG2RAD(-90);
            se.chair.draw(s.ID);

            se.chair.position = vec3(0, 0, 0.3);
            se.chair.rotation.y = DEG2RAD(90);
            se.chair.draw(s.ID);

            se.desk.position = vec3(0, 0, -0.35);
            se.desk.rotation.y = DEG2RAD(-90);
            se.desk.draw(s.ID);

            se.bookshelf.position = vec3(-0.5, 0, -0.25);
            se.bookshelf.draw(s.ID);

            se.room.position.z = 0.1;
            se.room.draw(s.ID);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        });

    lines = Shader(
        "assets/final.vs", "assets/final.fs",
        [](Shader &) {
            glGenFramebuffers(1, &buffer2);
            glBindFramebuffer(GL_FRAMEBUFFER, buffer2);

            glGenTextures(1, &buffer2_color);
            glBindTexture(GL_TEXTURE_2D, buffer2_color);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, SCREEN_W, SCREEN_H, 0, GL_RGB, GL_UNSIGNED_BYTE,
                         nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                   buffer2_color, 0);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                                  (void*)(2 * sizeof(float)));
            glEnableVertexAttribArray(1);

            float quad_verts[] = { -1, 1, 0, 1, -1, -1, 0, 0, 1, -1, 1, 0,
                                   -1, 1, 0, 1, 1,  -1, 1, 0, 1, 1,  1, 1 };

            glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), quad_verts, GL_STATIC_DRAW);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        },
        [](Shader &s, Session &) {
            glBindFramebuffer(GL_FRAMEBUFFER, buffer2);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        });

    fxaa = Shader(
        "assets/final.vs", "assets/fxaa.fs",
        [](Shader &) {
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                                  (void*)(2 * sizeof(float)));
            glEnableVertexAttribArray(1);

            float quad_verts[] = { -1, 1, 0, 1, -1, -1, 0, 0, 1, -1, 1, 0,
                                   -1, 1, 0, 1, 1,  -1, 1, 0, 1, 1,  1, 1 };

            glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), quad_verts, GL_STATIC_DRAW);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        },
        [](Shader &s, Session &) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, buffer2_color);
            glUniform1i(uloc("input_t"), 0);

            glUniform2f(uloc("texel_size"), 1.0f / SCREEN_W, 1.0f / SCREEN_H);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        });

    geo.init();
    shadows.init();
    lines.init();
    fxaa.init();

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
    shadows.render(*this);
    // lines.render(*this);
    fxaa.render(*this);

    window.swap();
}

void Session::load_glsl_helpers() {
    store_glsl_helper("assets/model.glsl");
    store_glsl_helper("assets/lights.glsl");
    store_glsl_helper("assets/lights_calc.glsl");
}

void Session::load_objects() {
    sofa = MeshStatic::from_scene("assets/sofa.obj");
    chair = MeshStatic::from_scene("assets/chair.obj");
    table = MeshStatic::from_scene("assets/table.obj");
    desk = MeshStatic::from_scene("assets/desk.obj");
    bookshelf = MeshStatic::from_scene("assets/bookshelf.obj");
    room = MeshStatic::from_scene("assets/room.obj");
}

void Session::generate_buffers() {

    glGenFramebuffers(1, &buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, buffer);

    glGenTextures(1, &buffer_sector);
    glBindTexture(GL_TEXTURE_2D, buffer_sector);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_W, SCREEN_H, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer_sector, 0);

    glGenTextures(1, &buffer_texture);
    glBindTexture(GL_TEXTURE_2D, buffer_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_W, SCREEN_H, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, buffer_texture, 0);

    glGenTextures(1, &buffer_depth);
    glBindTexture(GL_TEXTURE_2D, buffer_depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, SCREEN_W, SCREEN_H, 0, GL_DEPTH_COMPONENT,
                 GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer_depth, 0);

    GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        log_dang("Framebuffer not complete");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void update_light_space() {
    const float OFFSET = 16;
    const float NEAR = 0.1;
    const float FAR = OFFSET * 4;

    mat4 light_view = lookAt(-LIGHT_DIR * OFFSET, vec3(0, 0, 0), vec3(0, 1, 0));

    mat4 light_projection = ortho(-OFFSET, OFFSET, -OFFSET, OFFSET, NEAR, FAR);

    LIGHT_SPACE = light_projection * light_view;
}
