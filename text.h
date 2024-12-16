#ifndef TEXT_H
#define TEXT_H

#include <string.h>

static const char* DUNGEON_DESCRIPTIONS[15] = {
    "", // Beginning of the Road
    // Dead Valley
    "The lands of Dead Valley are far from their former glory. "
    "\nThe air is filled with the foul stench of decay, and the valley, once home to a small/charming village, "
    "\nnow lies in silence. The shadows flicker as if the valley's former inhabitants still wander in their lifeless bodies. "
    "\nLife has ended here, but this frightining stillness serves as proof that death is not truly the end.",
    
    // Route of King's Palace
    "The path to the King's Palace is a treacherous road lined with ancient statues of long-forgotten warriors. "
    "\nThe cracked cobblestones bear witness to countless battles, and whispers of long-lost souls fill the air. "
    "\nOnly the brave or the desperate tread this path, seeking what lies beyond.",

    // The King's Palace
    "An imposing palace looms in the distance. The King's Palace is majestic yet foreboading, its golden gates "
    "\n darkened with time. The gate is locked, and there is no one inside! The King has left the palace to mobilize for war. "
    "\nWhat treasures - or horrors - await those who gain entry?",

    // Crooked Forest: Spider Nest
    "The Crooked Forest is a labyrinth of twisted trees and perpetual darkness. Once home to countless species of plants, "
    "\nit is now a dark forest covered in spider webs. At its heart, beyond the spider webs, lives an ancient and wise wizard who "
    "\nseeks to help those brave enough to approach - but first, you must survive the swarm.",

    // Crooked Forest: Skeleton King
    "Deep in the Crooked Forest, an unholy graveyard marks the domain of the Skeleton King. His skeletal minions guard him fiercely, "
    "\nand their clattering bones echo like the drumbeats of an undead army. Defeating him is the only way to claim "
    "\nthe Key of the King's Palace, but his gaze promises certain doom for the unprepared.",

    // Exit of the Crooked Forest
    "The path out of the Crooked Forest is a narrow, winding trail that leads back to the safety of the world above. "
    "\nThe forest's oppressive darkness gives way to the light of the sun, and the air is fresh and free. But beware - the journey back is not without its dangers.",

    // Road to the Dragon's Lair
    "The path to the Dragon's Lair is a perilous journey through a land of towering cliffs and raging rivers. "
    "\nThe air is filled with the scent of danger, and the path is marked by the skeletal remains of those who dared to venture here. "
    "\nOnly the most determined will find the courage to continue.",

    // Path of the Ronin
    "The Path of the Ronin is a narrow, winding trail that leads back to the safety of the world above. "
    "\nThe forest's oppressive darkness gives way to the light of the sun, and the air is fresh and free. But beware - the journey back is not without its dangers.",

    // Hall of Blades
    "The Hall of Blades is a grand hall filled with the spirits of warriors who once fought for glory. "
    "\nAt the top is fixed a Katana, an ancient and powerful Japanese warrior sword that can only be wielded by the worthy.",

    // Crimson Abyss
    "The Crimson Abyss is a deep, dark abyss that stretches endlessly below. People have to be careful in this abyss "
    "\nfilled with lava, otherwise the slightest mistake can mean death.",

    // Entrance to Mount of Abyss
    "The ominous Entrance to Mount of Abyss is marked by jagged rocks and a single towering spire. Within, "
    "\nthe legendary Dragon guards its domain, its roars echoing through the cavern. Slaying the beast is the only way to claim "
    "\nits fabled Dragon's Scale.",

    // Mount of Abyss: Zombie Cave
    "A chilling wind greets those who step into the Zombie Cave, where the stench of rot is overwhelming. Hordes of zombies "
    "\nshuffle aimlessly, their lifeless eyes glowing faintly in the dark. Only those with unshakable courage can hope to survive "
    "\nthe onslaught.",

    // Mount of Abyss: Dead End
    "The Dead End is a cruel trick of nature - a hollow cave that echoes your every move. Skeletons of adventurers who sought "
    "\ntreasure here lie scattered, their weapons and hopes broken. The sense of despair is almost tangible, yet something about "
    "\nthis place feels... off.",

    // Top of the Mount of Abyss
    "The summit of the mountain is shrouded in dark clouds. At the Top of the Mount of Abyss, the air itself feels malevolent, "
    "\nthick with the presence of the Demon of the Abyss. Its glowing eyes pierce the darkness, daring you to challenge it. "
    "\nThis is the final trial - the end of your journey.",
};

#endif // TEXT_H