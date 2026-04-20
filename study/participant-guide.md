# VoxelEngine — Studie použitelnosti
### Průvodce pro účastníka

---

> **Poznámka pro účastníka:** Tato studie hodnotí *API enginu a dokumentaci*, nikoliv vás osobně. Neexistují špatné odpovědi ani podmínky neúspěchu. Pokud uvíznete, je to cenná informace. Prosím popisujte nahlas, co vás mate, než požádáte o pomoc.
>
> Hodnotitel nebude odpovídat na otázky *jak API používat* — pouze na problémy s kompilací nebo prostředím. Všechny ostatní odpovědi hledejte v dokumentaci nebo v existujícím zdrojovém kódu.

---

## Část 1 — Základní informace

Vyplňte prosím před otevřením jakéhokoli kódu: 
- https://forms.gle/jvNXzcFg5f3W8ZFS6


---

## Část 2 — Orientace v projektu (přečtěte před zahájením)

Pracujete s **VoxelEngine**, knihovnou pro voxelové vykreslování v C++. Startovací projekt se již kompiluje a spouští — otevře okno s terénem generovaným Perlinovým šumem, dvěma typy bloků a hráčem, kterého ovládáte klávesami WASD + myší.

**Struktura projektu:**

```
src/
  Game.cpp / Game.h       — hlavní třída hry (dědí od engine::Engine)
  registration.h          — registrace geometrií a bloků
  level/
    TerrainGenerator.h    — generátor terénu pomocí Perlinova šumu
  player/
    Player.cpp / Player.h — hráč s WASD pohybem a kamerou
resources/
  ground.png              — textura pro povrchovou vrstvu
  underground.png         — textura pro hlubší vrstvy
voxel_engine/
  docs.md                 — index dokumentace (zatím nečíst)
  docs/                   — kompletní dokumentace (zatím nečíst)
    getting-started.md
    blocks.md
    world-terrain.md
    rendering.md
    events-input.md
```

**Co je již zaregistrováno** (viz `registration.h`):

| Název | Block ID | Textura |
|-------|----------|---------|
| `ground` | 8 | `ground.png` — všechny strany |
| `underground` | 9 | `underground.png` — všechny strany |
| `cube` | (auto) | — pouze geometrie |

**Dostupné textury** (viz složka `resources/`):

| Soubor | Vhodné použití |
|--------|---------------|
| `ground.png` | boky/spodek povrchového bloku |
| `underground.png` | hlubší vrstvy |
| `top.png` | horní strana povrchového bloku |
| `sides.png` | alternativní textura pro boky |

**Pojmenování textur:** Název textury v kódu je název souboru bez přípony (např. `resources/ground.png` → `tex.texture("ground")`).

**Pravidlo pro Block ID:** ID 0–10 jsou již obsazena. Nové bloky začínejte od ID **11** výše.

**Dokumentace: (zatím nečíst)** `voxel_engine/docs.md` je rozcestník. Online Doxygen: https://dalibortrampota.github.io/voxel_engine/

---

## Část 2b — Cold Start (10 minut, bez dokumentace)

Otevřete soubory `src/Game.cpp`, `src/registration.h` a `src/level/TerrainGenerator.h` a přečtěte si je. Dokumentaci **zatím neotevírejte**.

Poté odpovězte na otázky ve formuláři pouze na základě toho, co vidíte v kódu:
- https://forms.gle/Rtb3qneLNqGKUupy9


---

## Část 3 — Úkoly

U každého úkolu zapište čas **začátku** a **konce**. Pokud uvíznete na více než několik minut, zapište kde a co vás zmátlo, a rozhodněte se, zda pokračovat nebo přejít k dalšímu úkolu.

---

### Úkol 1 — Nový typ bloku
**Doporučený čas: 10 minut**

Čas začátku:    
Čas konce:    
Dokončeno: ano/ne   
Přerušeno v: 

**Co implementovat:**

Terén aktuálně používá dva vizuálně totožné typy bloků. Přidejte třetí blok, který se zobrazuje pouze na **přesné povrchové vrstvě** (kde `pos.y == height(x, z)`), s vizuálně odlišnou texturou na horní straně oproti bočním a spodní straně.

Postup:
1. Zaregistrujte nový blok v `registration.h` s jedinečným ID (≥ 11) a materiálem s různými texturami pro různé strany. Ideálně použít textury `top.png` a `sides.png`.
2. Upravte `TerrainGenerator::voxelAt()` tak, aby vracela nový blok, když `pos.y == y` (přesná povrchová vrstva).

**Kritérium úspěchu:** Terén v okně má vizuálně odlišnou vrchní vrstvu silnou jeden blok.

**Poznámky / co vás zmátlo:**

```


```

---

### Úkol 2 — Struktury v terénu
**Doporučený čas: 25 minut**

Čas začátku:    
Čas konce:    
Dokončeno: ano/ne   
Přerušeno v: 

**Co implementovat:**

Rozšiřte `TerrainGenerator::populate()` tak, aby na vyšším terénu generovala struktury.

Pro každý sloupec `(x, z)` v chunku najděte **povrchový blok** — nejvyšší Y v daném sloupci, který není vzduch. Pokud je tento povrchový blok **nad Y = 3** (vzhledem k malému rozsahu výšek ve startovacím projektu), umístěte plošinu `3×3×1` z bloků `underground` jeden blok přímo nad povrch.

Pravidla:
- Plošina musí být vystředěna na daném sloupci (přesahovat 1 blok v každém horizontálním směru od `x` a `z`)
- Je přesně jeden blok vysoká
- Umístěte ji pouze tehdy, když celá plocha 3×3 se vejde do hranic chunku — **nepište mimo chunk** (lokální souřadnice musí zůstat v `[0, dims − 1]`)

> **Nápověda:** Uvnitř `populate()` pracujte s lokálními souřadnicemi chunku (0 až `chunk.dims() − 1`). Světová pozice lokálního `(x, y, z)` je `chunk.id() * chunk.dims() + glm::ivec3(x, y, z)`. Tento vzor již vidíte v existujícím `populate()`.

**Kritérium úspěchu:** Po spuštění hry jsou na vrcholcích vyšších kopců viditelné ploché plošiny.

**Poznámky / co vás zmátlo:**

```


```

---

### Úkol 3 — Sledovač načítání chunků
**Doporučený čas: 25 minut**

Čas začátku:    
Čas konce:    
Dokončeno: ano/ne   
Přerušeno v: 

**Co implementovat:**

Vytvořte třídu `ChunkTracker`, která naslouchá světu a počítá načtené chunky.

Požadavky:
1. `ChunkTracker` se přihlásí k odběru událostí `World`.
2. Pokaždé, když se chunk **chystá načíst**, vypište na `stdout`:
   ```
   Chunk loaded: (x, y, z)  |  Total loaded so far: N
   ```
   kde `(x, y, z)` jsou souřadnice chunku a `N` je průběžný součet.
3. Přihlaste instanci `ChunkTracker` ke světu **před** voláním `loadChunks()` v `Game::start()`.

> **Nápověda:** Viz `voxel_engine/docs/events-input.md` — konkrétně `LevelEventSite` a `ChunkBeforeLoadEvent`. ID chunku lze přečíst z `event->chunk->id()`.

**Kritérium úspěchu:** Konzole zobrazuje jeden řádek na chunk se správně rostoucím počítadlem pokaždé, když hra načítá chunky.

**Poznámky / co vás zmátlo:**

```


```

---

### Úkol 4 — Variantní blok *(volitelný, pokud zbývá čas)*
**Doporučený čas: 30 minut**

Čas začátku:    
Čas konce:    
Dokončeno: ano/ne   
Přerušeno v: 

**Co implementovat:**

Implementujte blok **potrubí**, který se vizuálně napojuje na sousední potrubí stejného typu.

Budete potřebovat tři geometrie. Vytvořte je pomocí `Geometry::Box(start, end)`:
- `pipe_center` — např. `Box({0.35f, 0.35f, 0.35f}, {0.65f, 0.65f, 0.65f})` — střed potrubí
- `pipe_east_west` — segment podél osy X (např. `Box({0.0f, 0.35f, 0.35f}, {1.0f, 0.65f, 0.65f})`)
- `pipe_north_south` — segment podél osy Z (např. `Box({0.35f, 0.35f, 0.0f}, {0.65f, 0.65f, 1.0f})`)

Požadavky:
1. Zaregistrujte všechny tři geometrie a `VariantBlock`.
2. Blok musí **vždy** zobrazovat středový díl.
3. Zobrazuje `pipe_east_west`, když je potrubí přítomno na **východě nebo západě**.
4. Zobrazuje `pipe_north_south`, když je potrubí přítomno na **severu nebo jihu**.
5. Více rozšíření lze zobrazit současně (`allowMultiple(true)`).
6. Umístěte několik bloků potrubí vedle sebe ve světě a ověřte propojení.

> **Nápověda:** Viz `voxel_engine/docs/blocks.md` — sekce „Variant Blocks". Jedno volání `addVariant` odpovídá jednomu směru připojení; potřebujete samostatné varianty pro východ a západ (nebo zkombinujte do jedné podmínky s oběma ID bloků v daném směru).

**Kritérium úspěchu:** Izolované potrubí zobrazuje pouze středový díl; sousední potrubí zobrazuje spojovací segmenty.

**Poznámky / co vás zmátlo:**

```


```

---

### Úkol 5 — Bourání bloků za běhu *(volitelný, pokud zbývá čas)*
**Doporučený čas: 25 minut**

Čas začátku:    
Čas konce:    
Dokončeno: ano/ne   
Přerušeno v: 

**Co implementovat:**

Umožněte hráči **bourat bloky levým tlačítkem myši**. Kliknutí odstraní první pevný blok v přímce pohledu (dosah 5 jednotek).

Požadavky:
1. Reakce na kliknutí musí být **event-based** (ne polling v `update()`).
2. Paprsek vychází z pozice kamery ve směru pohledu; odstraňte první nezduchový blok, který protíná.
3. Pokud paprsek nic nezasáhne, nic se nestane.

> **Nápověda:** Viz `voxel_engine/docs/events-input.md` — sekce „Event System" a „Subscribing to Events" — pro zachytávání vstupních událostí. Viz `voxel_engine/docs/world-terrain.md` — sekce „Block Access" a „Coordinate Utilities" — pro čtení a nastavování bloků; funkce `traceLine` vrátí seznam bloků, přes které paprsek prochází.

**Kritérium úspěchu:** Levé kliknutí myší odstraní blok, na který hráč míří.

**Poznámky / co vás zmátlo:**

```


```

---

## Část 4 — Dotazník po studii

Vyplňte prosím po skončení testování: 
https://forms.gle/kMT63j6ak2xBThkV8

---
