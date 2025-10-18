#include "mesh_static.hpp"

#include "base.hpp"
#include "pch.hpp"
#include "random.hpp"
#include <SDL3/SDL_stdinc.h>

static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_real_distribution<float> dist(0.0f, 1.0f);

MeshStatic MeshStatic::from_scene(std::string file, u32 _diffuse_id, u32 _spectral_id,
                                  u32 _normal_map_id) {
    Assimp::Importer import;

    aiScene* scene = (aiScene*)import.ReadFile(
        file.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs |
                          aiProcess_ImproveCacheLocality | aiProcess_GlobalScale |
                          aiProcess_CalcTangentSpace);

    if (!scene) {
        log_dang("Missing scene file %s", file.c_str());
    }

    if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        log_dang("%s: %s", file.c_str(), import.GetErrorString());
    }

    aiNode* node = scene->mRootNode;
    if (node->mNumMeshes == 0) {
        for (size i = 0; i < node->mNumChildren; i++) {
            aiNode* child = node->mChildren[i];
            if (child->mNumMeshes == 0) {
                continue;
            }
            node = child;
        }
    }

    if (!node || node->mNumMeshes == 0) {
        log_dang("%s: no meshes found", file.c_str());
    }

    MeshStatic m;
    m.diffuse_id = _diffuse_id;
    m.specular_id = _spectral_id;
    m.normal_map_id = _normal_map_id;

    for (usize midx = 0; midx < scene->mNumMeshes; midx++) {
        aiMesh* mesh = scene->mMeshes[midx];

        vec3 sector_color = vec3(dist(gen), dist(gen), dist(gen));

        for (size i = 0; i < mesh->mNumFaces; i++) {
            auto &face = mesh->mFaces[i];

            for (size j = 0; j < face.mNumIndices; j++) {
                u32 idx = face.mIndices[j];

                Vertex v = {};

                v.pos = { mesh->mVertices[idx].x, mesh->mVertices[idx].y, mesh->mVertices[idx].z };

                if (mesh->mNormals) {
                    v.normal = { mesh->mNormals[idx].x, mesh->mNormals[idx].y,
                                 mesh->mNormals[idx].z };
                } else {
                    log_dang("%s is missing normals", file.c_str());
                }

                if (mesh->mTextureCoords[0]) {
                    v.uv = { mesh->mTextureCoords[0][idx].x, mesh->mTextureCoords[0][idx].y };
                } else {
                    log_dang("%s is missing uvs", file.c_str());
                }

                v.sector_color = sector_color;

                m.indices.push_back((u16)m.verts.size());
                m.verts.push_back(v);
            }
        }
    }

    assert(m.verts.size() > 0);
    assert(m.indices.size() > 0);

    return m;
}

void MeshStatic::draw(u32 shader_id) {
    gl_buffer_data();
    gl_uniforms(shader_id);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);
}

void MeshStatic::apply_perlin(vec3 wpos, vec3 strength) {
    const float zoom = 32;

    for (auto &v : verts) {
        auto p = Random::perlin2(wpos.x + (v.pos.x * zoom), wpos.z + (v.pos.z * zoom));

        v.pos.x += (p / 8) * strength.x;
        v.pos.z += (p / 8) * strength.z;

        if (v.pos.y > 0.01f) {
            v.pos.y += (p / 16) * strength.y;
        }
    }
}

void MeshStatic::gl_buffer_data() {
    assert(verts.size());
    assert(indices.size());

    glBufferData(GL_ARRAY_BUFFER, verts.size() * (size)sizeof(Vertex), verts.data(),
                 GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * (size)sizeof(u16), indices.data(),
                 GL_STATIC_DRAW);
}

void MeshStatic::gl_uniforms(u32 shader_id) {
    glUniform3f(glGetUniformLocation(shader_id, "world_pos"), world_pos.x, world_pos.y,
                world_pos.z);
    glUniform3f(glGetUniformLocation(shader_id, "rotation"), rotation.x, rotation.y, rotation.z);
    glUniform1f(glGetUniformLocation(shader_id, "scale"), scale);

    glUniform1f(glGetUniformLocation(shader_id, "rand"), world_pos.x);
}
