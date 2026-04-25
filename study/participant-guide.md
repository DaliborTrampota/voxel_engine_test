# VoxelEngine — Studie použitelnosti
### Průvodce pro účastníka

---

> **Poznámka pro účastníka:** Tato studie hodnotí *API enginu a dokumentaci*, nikoliv vás osobně. Neexistují špatné odpovědi ani podmínky neúspěchu. Pokud uvíznete, je to cenná informace. Prosím popisujte nahlas, co vás mate, než požádáte o pomoc.
>
> Hodnotitel nebude odpovídat na otázky *jak API používat* — pouze na problémy s kompilací nebo prostředím. Všechny ostatní odpovědi hledejte v dokumentaci nebo v existujícím zdrojovém kódu.

---

## Část 1 — Základní informace

Vyplňte prosím před otevřením jakéhokoli kódu: 

- [https://forms.gle/jvNXzcFg5f3W8ZFS6](https://forms.gle/jvNXzcFg5f3W8ZFS6)

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


| Název         | Block ID | Textura                            |
| ------------- | -------- | ---------------------------------- |
| `ground`      | 9        | `ground.png` — všechny strany      |
| `underground` | 10       | `underground.png` — všechny strany |
| `cube`        | (auto)   | — pouze geometrie                  |


**Dostupné textury** (viz složka `resources/`):


| Soubor            | Vhodné použití                 |
| ----------------- | ------------------------------ |
| `ground.png`      | boky/spodek povrchového bloku  |
| `underground.png` | hlubší vrstvy                  |
| `top.png`         | horní strana povrchového bloku |
| `sides.png`       | alternativní textura pro boky  |


**Pojmenování textur:** Název textury v kódu je název souboru bez přípony (např. `resources/ground.png` → `tex.texture("ground")`).

**Pravidlo pro Block ID:** ID 0–10 jsou již obsazena. Nové bloky začínejte od ID **11** výše.

**Dokumentace: (zatím nečíst)** `voxel_engine/docs.md` je rozcestník. Online Doxygen: [https://dalibortrampota.github.io/voxel_engine/](https://dalibortrampota.github.io/voxel_engine/)

---

## Část 2b — Cold Start (10 minut, bez dokumentace)

Otevřete soubory `src/Game.cpp`, `src/registration.h` a `src/level/TerrainGenerator.h` a přečtěte si je. Dokumentaci **zatím neotevírejte**.

Poté odpovězte na otázky ve formuláři pouze na základě toho, co vidíte v kódu:

- [https://forms.gle/Rtb3qneLNqGKUupy9](https://forms.gle/Rtb3qneLNqGKUupy9)

---

## Část 3 — Úkoly

U každého úkolu zapište čas **začátku** a **konce**. Pokud uvíznete na více než několik minut, zapište kde a co vás zmátlo, a rozhodněte se, zda pokračovat nebo přejít k dalšímu úkolu.

---

### Úkol 1 — Nový typ bloku
**Doporučený čas: 10 minut**

Čas začátku: 15:10 \
Čas konce:    15:31\
Dokončeno:   ano\
Přerušeno v: 

**Co implementovat:**

Terén aktuálně používá dva vizuálně totožné typy bloků. Přidejte třetí blok, který se zobrazuje pouze na **přesné povrchové vrstvě** (kde `pos.y == height(x, z)`), s vizuálně odlišnou texturou na horní straně oproti bočním a spodní straně.

Postup:
1. Zaregistrujte nový blok v `registration.h` s jedinečným ID (≥ 11) a materiálem s různými texturami pro různé strany. Ideálně použít textury `top.png` a `sides.png`.
2. Upravte `TerrainGenerator::voxelAt()` tak, aby vracela nový blok, když `pos.y == y` (přesná povrchová vrstva).

**Kritérium úspěchu:** Terén v okně má vizuálně odlišnou vrchní vrstvu silnou jeden blok.

**Poznámky / co vás zmátlo:**

```
Nevěděl jsem jak se přidávají víc než jedna textura k bloku

```

---

### Úkol 2 — Povrchové struktury
**Doporučený čas: 25 minut**

Čas začátku:    15:32\
Čas konce:    16:02 \
Dokončeno: ano   
Přerušeno v: 

**Co implementovat:**

Rozšiřte `TerrainGenerator::populate()` tak, aby na povrchu generovala dva typy dekorací.

Pro každý sloupec `(x, z)` v chunku najděte **povrchový blok** — nejvyšší Y v daném sloupci, který není vzduch. Na základě deterministického hashe světových souřadnic sloupce pak rozhodněte:

- **~5 % sloupců:** umístěte **kmen stromu** — 3 bloky `ground` nad sebou přímo nad povrchem
- **~10 % sloupců:** umístěte **kámen** — 1 blok `underground` přímo nad povrchem
- **zbývající sloupce:** nic

Pravidla:
- Kmen musí celý ležet uvnitř chunku — **nepište mimo chunk** (lokální Y musí zůstat v `[0, dims.y − 1]`)
- Pro deterministickou náhodu použijte vhodnou funkci z `<random/Random.h>`

> **Nápověda:** Uvnitř `populate()` pracujte s lokálními souřadnicemi chunku (0 až `chunk.dims() − 1`). Světová pozice lokálního `(x, y, z)` je `chunk.id() * chunk.dims() + glm::ivec3(x, y, z)`. Tento vzor již vidíte v existujícím `populate()`.

**Kritérium úspěchu:** Po spuštění hry jsou na terénu viditelné roztroušené kameny a trojblokové kmeny stromů.

**Poznámky / co vás zmátlo:**

```
Aby se generovala pro stejný seed stejná random hodnota, musím použít RandomRange

```

---

### Úkol 3 — Sledovač načítání chunků
**Doporučený čas: 25 minut**

Čas začátku:  16:10\
Čas konce:    16:45\
Dokončeno: ano  
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
Nevěděl jsem, že v id chunku jsou data o pozici toho chunku, ze začátku jsem předpokládal, že jsou v chunk->data  
Trochu v dokumentaci o events-input chyběly potřebné includes na eventy

```

---

### Úkol 4 — Bourání bloků za běhu

**Doporučený čas: 25 minut**

Čas začátku: 16:50\
Čas konce:  17:14\
Dokončeno: ano  
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
Nevěděl jsem, co mám includnout pro funkci traceLine(možná jsem to přehlédl) a to že musím používat pozici kamery a ne hráče

```

---

## Část 4 — Dotazník po studii

Vyplňte prosím po skončení testování: 
[https://forms.gle/kMT63j6ak2xBThkV8](https://forms.gle/kMT63j6ak2xBThkV8)

---

