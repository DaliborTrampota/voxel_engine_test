#pragma once

#include <Globals.h>
#include <block/Block.h>
#include <block/BlockMaterial.h>
#include <block/Geometry.h>
#include <data/RegistryManager.h>
#include <data/TextureManager.h>

inline void registerBlocks() {
    engine::TextureManager& texMgr = engine::TextureManager::Get();
    auto& geos = engine::RegistryManager::Geometries();
    auto& blocks = engine::RegistryManager::Blocks();


    // NOTE: just pass geometry name, block will search for it?
    engine::Block ground =
        engine::Block(9, engine::Layers::Opaque, &geos.get("cube"))
            .isSolid(true)
            .material(engine::BlockMaterial().add(engine::FaceTag::All, texMgr.texture("ground")));

    engine::Block underground =
        engine::Block(10, engine::Layers::Opaque, &geos.get("cube"))
            .isSolid(true)
            .material(
                engine::BlockMaterial().add(engine::FaceTag::All, texMgr.texture("underground"))
            );
     engine::Block surface =
        engine::Block(11, engine::Layers::Opaque, &geos.get("cube"))
            .isSolid(true)
            .material(engine::BlockMaterial().add(engine::FaceTag::Top, texMgr.texture("top"))
                                             .add(engine::FaceTag::Side, texMgr.texture("sides")));

    blocks.add(ground, "ground");
    blocks.add(underground, "underground");
    blocks.add(surface, "surface");
}

// NOTE: default geometries should be registered automatically by engine
inline void registerGeometries() {
    auto& geos = engine::RegistryManager::Geometries();

    geos.add(engine::Geometry::Cube(), "cube");
}