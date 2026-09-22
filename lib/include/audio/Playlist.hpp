#pragma once

#include <string>
#include <vector>

struct [[nodiscard]] Playlist final
{
    std::vector<std::string> menuSongs;
    std::vector<std::string> gameSongs;
};

const Playlist PLAYLIST = Playlist {
    .menuSongs = { "01 Dance of Anarchy.ogg" },
    .gameSongs = { "01 Dance of Anarchy.ogg",
                   "02 Smartphone Zombies.ogg",
                   "03 The men with fake names.ogg",
                   "04 Sofamusik.ogg",
                   "06 When the freaks are coming home.ogg",
                   "07 Sounds from yesterday.ogg",
                   "08 The old Technomachine.ogg" },
};
