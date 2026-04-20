#pragma once

#include <Globals.h>
#include <block/Block.h>
#include <block/BlockMaterial.h>
#include <block/Geometry.h>
#include <data/RegistryManager.h>
#include <data/TextureManager.h>

inline void registerBlocks() {
    engine::TextureManager& texMgr = engine::TextureManager::Get();
    // NOTE: just pass geometry name, block will search for it?
    engine::Block ground =
        engine::Block(8, engine::Layers::Opaque, &engine::RegistryManager::Geometries().get("cube"))
            .isSolid(true)
            .material(engine::BlockMaterial().add(engine::FaceTag::All, texMgr.texture("ground")));

    engine::Block underground =
        engine::Block(9, engine::Layers::Opaque, &engine::RegistryManager::Geometries().get("cube"))
            .isSolid(true)
            .material(
                engine::BlockMaterial().add(engine::FaceTag::All, texMgr.texture("underground"))
            );

    engine::RegistryManager::Blocks().add(ground, "ground");
    engine::RegistryManager::Blocks().add(underground, "underground");
}

// NOTE: default geometries should be registered automatically by engine
inline void registerGeometries() {
    engine::RegistryManager::Geometries().add(engine::Geometry::Cube(), "cube");
}