# Winrate Tracker

This mod <cg>tracks winrate</cg> using your recent runs and your uses it to calculate your <cy>time to get 100%</cy> from 0. 

The <cg>winrate</cg> gets presented with text labels in the bottom left corner. 

Three main labels are implemented in the mod:
- <cp>Winrate.</cp> Your winrate from 0.
- <cp>Winrate now.</cp> Your winrate from your current percentage.
- <cp>Time for 100%</cp>. The <cy>estimated time</cy> to beat the beat the level from 0.

In the settings you can choose which labels to display. These labels lets you track your consistency as you play. More labels may be added in the future!

The <cp>Winrate</cp> and <cp>Time for 100%</cp> can also be found in the menu located in the pause menu and level page.

Extra labels include: 
- <cp>Run Rarity</cp>. Says how <cy>rare</cy> your current run is. The inverse of <cr>Winrate now.</cr>

Keybinds include:
- Pausing/unpausing tracking of runs
- A keybind that allows resets midrun while also counting the run for winrate

There is support for linking levels together using the menu with a percentage sign on the level page.

The <cg>winrate</cg> gets updated with a Exponential Moving Average(EMA) algorithm and will lag behind your "true" <cg>winrate</cg> as you improve on the level. (You can change how fast your winrate changes by adjusting Alpha in the settings).

When playing from a startposition or checkpoint, I've added a safezone where winrate doesn't get updated for 1.2 seconds(you can change this in the settings).

<cy>Disclaimer! The creation of this mod has been streamed/recorded on my youtube channel: [Here is a link to the playlist! Come check it out if you'd like.](https://www.youtube.com/playlist?list=PLPUCtOoQdh8BEpDWf24AqSjPZfKtrFoCi)</cg>

Special thanks to: 
- Death Tracker
- Playtime Tracker
for inspiration and cross referencing how to do stuff(menus scared me a lot).

Also
- @Davinchik3
- Alva 
for joining me on the stream!