# 1.0.41
- Updated about.md and moved down the link to my youtube

# 1.0.4
- Added a setting and keybind to pause tracking of winrate
- Made it more clear when tracking is paused or if you died during the safe zone
- Removed potential softlock when linking levels by changing the order of popups.
- Fixed potential collision between editor ids and level ids.
- Patched bugged resets during the completion animation using the reset keybind.
- Opted against using numToAbbreviatedString since it doesn't handle large enough numbers, but I also reduced the maximum large number to a NONILLION.

# 1.0.3
- Fixed visual bug when dying after using the "Reset and record run"-keybind.

# 1.0.2
- Added a new keybind, "Reset and record run"
- Fixed formatting bug in Overall Winrate
- Fixed unintended behaviour for "Winrate Now" and "Run Rarity" when player dies
- Changed run rarity name in settings
- Removed log::info debug logs from release

# 1.0.1
- Fixed formating of hours("Use Days And Years Disabled")
- Re-ordered Settings
- Added settings for moving and resizing text labels

# 1.0.0
- Initial Release!
Features include: 
- Winrate tracking
- Estimated time for completing the level, based on winrate and level length
- In total, 4 possible text labels in the bottom left corner as you play
- Linking winrate for levels together
- Custom Menu Buttons in the level info screen and pause menu.
- 3 stats pages that can all be copied to clipboard.