---
title: Pikachu's Beach Port Specification
target: pret/pokefirered
source_game: Pokémon Yellow
purpose: AI-agent implementation brief for porting Pikachu's Beach into Pokémon FireRed
---

# Pikachu's Beach — Port Specification for `pret/pokefirered`

## 1. Overview and Access

**Pikachu's Beach** is a side-scrolling surfing minigame exclusive to *Pokémon Yellow*. It is accessed by talking to the Surfin' Dude in the isolated house on **Route 19** while the player has a Surfing Pikachu in the party. The minigame is structurally similar to *Excitebike*: the player manages speed, timing, and risk to maximise a final score.

**For the port:** The trigger condition (Surfing Pikachu) can be simplified or retained as a flag check. The core deliverable is the minigame loop itself.

---

## 2. Core Gameplay Loop

Pikachu rides a surfboard automatically from left to right across a scrolling ocean. The player does not steer left or right freely. Instead, the **only input is pressing Left or Right on the D-pad** to perform flips when Pikachu is launched off a wave.

### Wave Interaction and Flips

- Waves approach from the right. When Pikachu hits a wave, it is launched into the air.
- During the airborne window, the player can press **Left** or **Right** to rotate Pikachu.
- Each press during the same launch constitutes a **flip**. The maximum number of flips depends on the size of the wave; large waves allow up to three.
- **Bailing / Crashing:** If the player attempts more flips than the wave can support, or mistimes the final press, Pikachu crashes into the water. A crash grants **zero points for that wave** and **slows the scroll speed**.

### Scroll Speed — Risk/Reward Pacing

- Successful flips **increase the scroll rate**.
- The faster the screen scrolls, the more waves appear per unit of time, and the sooner Pikachu reaches the shore.
- Bailing on a flip **decreases the scroll rate**, making progress slower and draining more of the HP/timer before the shore is reached.

---

## 3. Scoring System — “Radness”

Points are awarded per successful wave-launch sequence, measured in **“Radness”**. The value depends on the **number of flips** and whether all flips were in the **same direction** or **mixed directions**:

| Flips performed | Direction condition | Radness points |
|---|---|---|
| 1 flip | — | **50** |
| 2 flips | Both in the same direction | **150** |
| 2 flips | In different directions | **180** |
| 3 flips | All in the same direction | **350** |
| 3 flips | Not all in the same direction | **500** |

**Port note:** Implement the full directional matrix above. The “not all in the same direction” condition for three flips means at least one flip must reverse direction relative to the others.

---

## 4. HP / Timer and Progress

### HP as a Timer

A counter in the lower-right corner displays **HP**, starting at **6000**. It decreases by **1 unit every frame**, functioning as a countdown timer rather than a traditional health bar.

- If HP reaches **0** before Pikachu reaches the shore, the player **loses** the minigame.
- The HP value is **not** replenished by flips. It only drains.

### Progress Bar

A separate progress indicator, opposite the HP counter, shows how far Pikachu is from the shore. The game ends when the progress bar is filled and Pikachu reaches the shore.

### Final Score

When Pikachu reaches the shore, the final score is:

> **Final Score = Total Radness Points + Remaining HP**

The remaining HP therefore acts as a **speed bonus**: faster scrolling, achieved through successful flips, preserves more HP for the final score.

---

## 5. End States and Hi-Score

- **Win:** Pikachu reaches the shore; final score is calculated.
- **Loss:** HP reaches 0; game over, typically no final score bonus.
- **Hi-score:** The best final score is saved and can be viewed at the Summer Beach House.

**Port note:** For `pret/pokefirered`, store the hi-score in a save variable or dedicated save section. Printing is not required; displaying the stored best score is sufficient.

---

## 6. Presentation and Audio

- **Visuals:** A horizontally scrolling beach/ocean scene. Pikachu is rendered on a surfboard with flip animation frames. Waves are tile-based obstacles and launch points. The original uses Game Boy palettes; the port will need GBA-appropriate palettes and sprite dimensions.
- **Audio:** The minigame uses a dedicated music track stored on a soundbank not present in the other Gen I games. For the FireRed port, this can be replaced with a suitable existing GBA track or a newly sequenced arrangement using the `pret/pokefirered` sound engine.

---

## 7. Key Logic Summary for the Porting Agent

1. **Input:** D-pad Left/Right only; edge-detected during the airborne window after a wave launch.
2. **Flip counter per launch:** Increment on each valid directional press; track direction sequence.
3. **Wave capacity:** Each wave has a maximum flip count of 1–3. Exceeding it or pressing outside the window triggers a **bail**.
4. **Scoring:** Apply the five-row Radness table on a **successful landing**; award zero points on a bail.
5. **Scroll speed:** Increase on successful flips; decrease on bail.
6. **HP:** Decrement by 1 per frame; terminate as a loss at 0.
7. **Progress:** Advance based on scroll speed; terminate as a win when the shore is reached.
8. **Final score:** Radness + remaining HP.
9. **Save:** Store hi-score.

---

## 8. Integration Notes for `pret/pokefirered`

- Map the minigame to a dedicated C module, e.g. `src/pikachu_beach.c`.
- Use FireRed's existing input system for edge-detected D-pad presses.
- Use the sprite/tile/animation systems for Pikachu, surfboard, waves, and background scrolling.
- Use a dedicated save variable or save block field for the hi-score.
- Keep the minigame loop isolated from normal overworld logic.
- Provide a clean entry point from the Route 19 Surfin' Dude NPC or a debug trigger.
- Once original source/assembly is provided, map the constants, scoring table, wave capacities, and HP/scroll-speed values directly into the FireRed implementation.