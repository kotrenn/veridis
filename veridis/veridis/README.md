#Project: Veridis
Timeline:  Fall 2011

A 2D overhead action RPG adventure game with elements from SHMUP games.

Plot
----

Back in the days of mysterious scientific advances, one group worked for a shadow government on something sinister. Project: Veridis was supposed to be the ultimate weapon for bringing peace. But one day, everything went wrong. The event caused mass suffering, global economic turndown, and the world's supplie of Twinkies to disappear. The creation then suddenly disappeared, never to be heard of again...

One day you were sailing, when a storm appeared. You awoke to find yourself on a mysterious island. There are no smoke monsters here, only shadows of the past.

Overview
--------

Defeat the enemies!  Create bullets that can potentially damage critters while moving around in a dull environment.

Killing enemies gives you a change for items to appear!  Collect items to upgrade your smiley.  Upgrades include recovering health, increasing number of bullets fired, and rate at which bullets are fired.  All three have an upper limit.

Find the ultimate evil!  It's lurking somewhere...

Detailed Overview
--------

Project: Veridis was a semester project for a graduate course on software development at Clemson University.  The professor was fond of using computer games as a context for learning software practices.

For my project, I decided I would combine two styles of games.  First, the 2D overhead action RPGs which inherit their gameplay from the classic Legend of Zelda gamse.  Second, 2D vertical and side-scrolling shooters (SHMUPs).  To combine the two, I observed that for SHMUPs, players will generally fire in a singular direction:  typically either up or to the right.  This works as a key component of how to play, and influences how players aim and dodge, along with how enemies are designed.

To put this concept of directional fire into a 2D overhead exploration game, I created a targeting system.  At any time, the player will have one enemy on screen as the current 'target.'  The player may switch between enemies at will.  When firing, bullets will be fired towards the enemy.  Yet rather than simply going at a perfect straight line from player to enemy, the game will determine one of eight directions that matches this direction best.  Particularly, players will fire in any one of the four cardinal directions or any one of the four diagonals.  This way, if players keep their distance, then long narrow corridors become a vertical or side-scrolling SHMUP.

Since this game was more of a proof of concept (and also a course project), only basic gameplay mechanics were explored.  Enemies give simple upgrades for firing rate, firing quantity, and simply healing the player.  Only four basic enemy types were implemented, with basic firing patterns that were easy to learn.  A final boss was included, though provided no originality in terms of gameplay.  Instead the boss provided players just a quick chance to see what the game was like.


Editor
------

For this project, I decided to include a simple level editor.  The editor can be accessed during gameplay at any time.  A tile selector will appear on one side, while the current level is on the other.  Here users can select which tile they want to place, then click where it will appear in the region.  Additionally, Factory methods provided a nice way to place different enemies in levels.  A scroll list of all available Actor types is provided, and functions similarly to the tile placement tool.

The "world" is simply a patch of separate rectangular regions.  When a player reaches the border of one, they will move to the neighboring region.  Each region has an associated coordinate based on Cartesian coordinates.  For example, players start at [0, 0] and from there can reach [0, 1], [0, -1], [1, 0], and [-1, 0].  Each region is stored in its own data file under the dat/ directory.  The purpose of this is to allow for holes in the world.  Older games would try to fit every part of the world into one large 32x32 rectangle due to space constraints.  By the time Veridis was being developed, memory was much cheaper and the patchwork method was used.  When the game encountered a region that did not exist yet, a simple template region would be drawn in.

Finally, a Quake-style console was implemented for some basic functionality.  First, users could update global setting variables, such as where the player started and how fast bullets move around.  This allowed for quick feedback when fine-tuning gameplay.  Next, there were simple commands to save and load levels, along with clear out all enemies from the current level.  Finally, there was a cheat to give the player maximum upgrades.  This was useful for both development purposes and for showing off the game without worrying about succeeding.


Gameplay
--------

Controls:

- Z   - shoot
- X   - switch target
- P   - pause
- ESC - quit game
- ~   - open/close console
- BACKSPACE - switch between editing mode and playing

Debug Controls:

- SPC - add enemies

Edit Controls:

- WSAD - switch current map
- Left mouse click - add actor / select tile / select actor
- Right mouse click - remove actor
- 1 - switch to actor editing
- 2 - switch to tile editing
- F - fill map with current tile

Console Commands:

    set x y z      - set variable y of type x to value z in the settings
    save settings  - save the global variables to settings.dat
    save level     - save the current level
    load level x y - load level x y
    clear spawn    - clear all monster spawns from the current level
    malloy mode    - make the game stupidly easy




--Credits--

Art assets from:
- First Star Online
- Seiken Densetsu
- Seiken Densetsu III
- Ramona "Rob" Seay

Music:
- Flora Fauna by Anamanaguchi

