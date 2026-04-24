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

    auto cube = [&](int id, const char* texture) {
        return engine::Block(id, engine::Layers::Opaque, &geos.get("cube"))
            .isSolid(true)
            .material(engine::BlockMaterial().add(engine::FaceTag::All, texMgr.texture(texture)));
    };
    // Biome blocks
    blocks.add(cube(11, "ash"), "ash");
    blocks.add(cube(12, "compressed_ash"), "compressed_ash");
    blocks.add(cube(13, "silicate_sand"), "silicate_sand");
    blocks.add(cube(14, "basalt"), "basalt");
    blocks.add(cube(15, "fractured_basalt"), "fractured_basalt");
    blocks.add(cube(16, "slate"), "slate");
    blocks.add(cube(17, "crystal_crust"), "crystal_crust");
    blocks.add(cube(18, "gas_ice"), "gas_ice");
    blocks.add(cube(19, "deep_slate"), "deep_slate");
    blocks.add(cube(20, "iron_basalt"), "iron_basalt");
    blocks.add(cube(21, "microbial_crust"), "microbial_matter");
    blocks.add(cube(22, "algal"), "algal_slime");
}

// NOTE: default geometries should be registered automatically by engine
inline void registerGeometries() {
    auto& geos = engine::RegistryManager::Geometries();

    geos.add(engine::Geometry::Cube(), "cube");
}