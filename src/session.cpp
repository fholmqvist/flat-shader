#include "mesh_static.hpp"
#include "pch.hpp"

#include "session.hpp"

#include "base.hpp"
#include "light.hpp"
#include "shader.hpp"
#include "window.hpp"

GLuint shadow_fbo;
GLuint shadow_depth;

GLuint color_fbo;
GLuint color_depth;
GLuint color_sector;
GLuint color_texture;

GLuint lines_fbo;
GLuint lines_depth;
GLuint lines_texture;

#define uloc(name) glGetUniformLocation(s.ID, name)

Session::Session() {
    log_info("Starting");

    if (!window.init()) {
        throw std::runtime_error("Window failed to initialize");
    }

    load_glsl_helpers();

    load_objects();

    generate_buffers();

    textures.add("palette", "assets/antiquity16.png", GL_RGB);

    camera.pos = { 5, 4, 6 };
    camera.rot = { 220, -30 };

    init_shaders();

    log_info("Started in %s (total)", time_to_string(timer.stop()).c_str());
}

void Session::update() {
    window.update();
    running = input.update(camera);
    camera.update();
}

void Session::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    TRIANGLES_DRAWN = 0;

    depth.render(*this);
    shadows.render(*this);
    lines.render(*this);
    fxaa.render(*this);

    window.swap();
}

void Session::draw_objects(u32 shader_id) {
    monorail.position.x = 0;
    monorail.position.z = 1.1;
    monorail.draw(shader_id);
    monorail.position.z = 2.2;
    monorail.draw(shader_id);
    monorail.position.z = 3.3;
    monorail.draw(shader_id);
    monorail.position.z = 4.4;
    monorail.draw(shader_id);

    monorail.position.x = 3;
    monorail.position.z = 1.1;
    monorail.draw(shader_id);
    monorail.position.z = 2.2;
    monorail.draw(shader_id);
    monorail.position.z = 3.3;
    monorail.draw(shader_id);
    monorail.position.z = 4.4;
    monorail.draw(shader_id);

    for (int z = 0; z < 6; z++) {
        for (int x = 1; x < 3; x++) {
            floor.position = vec3(x, 0, z);
            floor.draw(shader_id);
        }
    }

    rail.position.x = 0;
    wall.position.x = 0;
    wall.rotation.y = DEG2RAD(0);
    for (int i = 0; i < 6; i++) {
        rail.position.z = i;
        rail.draw(shader_id);

        wall.position.z = i;
        wall.draw(shader_id);
    }

    rail.position.x = 3;
    wall.position.x = 3;
    wall.rotation.y = DEG2RAD(180);
    for (int i = 0; i < 6; i++) {
        rail.position.z = i;
        rail.draw(shader_id);

        wall.position.z = i;
        wall.draw(shader_id);
    }

    rail_turn.position = vec3(0);
    rail_turn.rotation = vec3(0);
    rail_turn.draw(shader_id);
    rail_turn.position = vec3(1, 0, -2);
    rail_turn.rotation.y = DEG2RAD(-90);
    rail_turn.draw(shader_id);
    rail_turn.position.z = 7;
    rail_turn.position.x = 2;
    rail_turn.rotation.y = DEG2RAD(90);
    rail_turn.draw(shader_id);
    rail_turn.draw(shader_id);
    rail_turn.position.z = 5;
    rail_turn.position.x = 3;
    rail_turn.rotation.y = DEG2RAD(180);
    rail_turn.draw(shader_id);

    seagull.position = vec3(1, 0, 1);
    seagull.draw(shader_id);
}

void Session::load_glsl_helpers() {
    store_glsl_helper("assets/model.glsl");
    store_glsl_helper("assets/lights.glsl");
    store_glsl_helper("assets/lights_calc.glsl");
}

void Session::load_objects() {
    monorail = MeshStatic::from_scene("assets/monorail.obj");
    rail = MeshStatic::from_scene("assets/rail.obj");
    rail_turn = MeshStatic::from_scene("assets/rail_turn.obj");
    wall = MeshStatic::from_scene("assets/wall.obj");
    floor = MeshStatic::from_scene("assets/floor.obj");
    seagull = MeshStatic::from_scene("assets/seagull.obj");
}

void Session::generate_buffers() {
    {
        glGenFramebuffers(1, &shadow_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);

        glGenTextures(1, &shadow_depth);
        glBindTexture(GL_TEXTURE_2D, shadow_depth);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, SHADOW_SIZE, SHADOW_SIZE, 0,
                     GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_depth, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            log_dang("Shadow framebuffer not complete");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    {
        glGenFramebuffers(1, &color_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, color_fbo);

        glGenTextures(1, &color_depth);
        glBindTexture(GL_TEXTURE_2D, color_depth);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, SCREEN_W, SCREEN_H, 0,
                     GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, color_depth, 0);

        glGenTextures(1, &color_sector);
        glBindTexture(GL_TEXTURE_2D, color_sector);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_W, SCREEN_H, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_sector,
                               0);

        glGenTextures(1, &color_texture);
        glBindTexture(GL_TEXTURE_2D, color_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_W, SCREEN_H, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, color_texture,
                               0);

        GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            log_dang("Framebuffer not complete");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    {
        glGenFramebuffers(1, &lines_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, lines_fbo);

        glGenRenderbuffers(1, &lines_depth);
        glBindRenderbuffer(GL_RENDERBUFFER, lines_depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, SCREEN_W, SCREEN_H);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                                  lines_depth);

        glGenTextures(1, &lines_texture);
        glBindTexture(GL_TEXTURE_2D, lines_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_W, SCREEN_H, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lines_texture,
                               0);

        GLenum attachments[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, attachments);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            log_dang("Framebuffer not complete");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void Session::init_shaders() {

    depth = Shader(
        "assets/depth.vs", "assets/depth.fs",
        [](Shader &) {
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void*)(offsetof(Vertex, pos)));
            glEnableVertexAttribArray(0);
        },
        [](Shader &s, Session &se) {
            glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
            glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            update_light_space(se.camera);
            glUniformMatrix4fv(uloc("light_space"), 1, GL_FALSE, &LIGHT_SPACE[0][0]);

            se.draw_objects(s.ID);

            glViewport(0, 0, SCREEN_W, SCREEN_H);
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

            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void*)(offsetof(Vertex, sector_color)));
            glEnableVertexAttribArray(3);
        },
        [](Shader &s, Session &se) {
            glBindFramebuffer(GL_FRAMEBUFFER, color_fbo);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUniformMatrix4fv(uloc("view_projection"), 1, GL_FALSE,
                               value_ptr(se.camera.perspective() * se.camera.view_matrix()));

            glUniformMatrix4fv(uloc("light_space"), 1, GL_FALSE, &LIGHT_SPACE[0][0]);
            glUniform3fv(uloc("light_dir"), 1, &LIGHT_DIR[0]);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, shadow_depth);
            glUniform1i(glGetUniformLocation(s.ID, "shadow_t"), 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, se.textures.get("palette").id);
            glUniform1i(glGetUniformLocation(s.ID, "palette_t"), 1);

            se.draw_objects(s.ID);

            glEnable(GL_CULL_FACE);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        });

    lines = Shader(
        "assets/lines.vs", "assets/lines.fs",
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
            glBindFramebuffer(GL_FRAMEBUFFER, lines_fbo);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, color_sector);
            glUniform1i(uloc("sector_t"), 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, color_texture);
            glUniform1i(uloc("texture_t"), 1);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, color_depth);
            glUniform1i(uloc("depth_t"), 2);

            glUniform2f(uloc("texel_size"), 1.0f / SCREEN_W, 1.0f / SCREEN_H);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        });

    fxaa = Shader(
        "assets/lines.vs", "assets/fxaa.fs",
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
            glBindTexture(GL_TEXTURE_2D, lines_texture);
            glUniform1i(uloc("input_t"), 0);

            glUniform2f(uloc("texel_size"), 1.0f / SCREEN_W, 1.0f / SCREEN_H);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        });

    depth.init();
    shadows.init();
    lines.init();
    fxaa.init();
}
