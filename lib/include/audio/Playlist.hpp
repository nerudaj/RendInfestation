#pragma once

#include <string>
#include <vector>

struct [[nodiscard]] Playlist final
{
    std::vector<std::string> menuSongs;
    std::vector<std::string> gameSongs;
};

const Playlist PLAYLIST = Playlist {
    .menuSongs = { "01_Dance_of_Anarchy.ogg" },
    .gameSongs = { "01_Dance_of_Anarchy.ogg",
                   "02_Smartphone_Zombies.ogg",
                   "03_The_men_with_fake_names.ogg",
                   "04_Sofamusik.ogg",
                   "06_When_the_freaks_are_coming_home.ogg",
                   "07_Sounds_from_yesterday.ogg",
                   "08_The_old_Technomachine.ogg" },
};
