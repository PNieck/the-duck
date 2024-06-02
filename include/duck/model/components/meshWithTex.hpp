#pragma once

#include "mesh.hpp"


class MeshWithTex: private Mesh {
public:
    MeshWithTex(): Mesh() {}

    void Update(const std::vector<float>& vertices, const std::vector<uint32_t>& indices);

    inline unsigned int GetElementsCnt() const
        { return Mesh::GetElementsCnt(); }

    inline void Use() const
        { Mesh::Use(); }
};
