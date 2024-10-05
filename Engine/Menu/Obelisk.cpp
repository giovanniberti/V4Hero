#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include "Obelisk.h"
#include "math.h"
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <fstream>
#include <spdlog/spdlog.h>
#include "../CoreManager.h"
#include "../StateManager.h"

using json = nlohmann::json;

ObeliskMenu::ObeliskMenu()
{
    SPDLOG_INFO("Initializing Obelisk...");

    quality = CoreManager::getInstance().getConfig()->GetInt("textureQuality");

    SpriteWrapper i_hunt, i_fortress;
    i_hunt.load("resources/graphics/ui/worldmap/hunting_icon.png");
    i_fortress.load("resources/graphics/ui/worldmap/fortress_icon.png");

    mission_icons.push_back(i_hunt);
    mission_icons.push_back(i_fortress);

    dullpon.load("resources/graphics/ui/worldmap/dullpon.png");

    ///boxes
    mainbox.load("resources/graphics/ui/worldmap/main_box.png");
    mainbox.setOrigin(mainbox.getLocalBounds().width / 2, mainbox.getLocalBounds().height / 2);
    mainbox.setPosition(640*3, 320*3);

    descbox.load("resources/graphics/ui/worldmap/description_box.png");
    descbox.setOrigin(descbox.getLocalBounds().width / 2, descbox.getLocalBounds().height / 2);
    descbox.setPosition(820*3, 542*3);

    iconbox.load("resources/graphics/ui/worldmap/icon_box.png");

    missionbox.load("resources/graphics/ui/worldmap/mission_box.png");
    missionbox.setOrigin(missionbox.getLocalBounds().width / 2, missionbox.getLocalBounds().height / 2);
    missionbox.setPosition(290*3, 542*3);

    auto strRepo = CoreManager::getInstance().getStrRepo();
    std::string font = strRepo->GetFontNameForLanguage(strRepo->GetCurrentLanguage());

    worldmap_title.setFont(font);
    worldmap_title.setCharacterSize(34);
    worldmap_title.setColor(sf::Color::Black);
    worldmap_title.setStringKey("worldmap_header_1");
    location_title.setFont(font);
    location_title.setCharacterSize(27);
    location_title.setColor(sf::Color::Black);
    worldmap_title.setStringKey("worldmap_location_1_title");
    string desc = Func::wrap_text("worldmap_location_1_description", 800*3, font, 18);

    location_desc.setFont(font);
    location_desc.setCharacterSize(19);
    location_desc.setColor(sf::Color::Black);
    location_desc.setString(desc);
    select_quest.setFont(font);
    select_quest.setCharacterSize(18);
    select_quest.setColor(sf::Color::Black);
    select_quest.setStringKey("worldmap_select");
    mission_title.setFont(font);
    mission_title.setCharacterSize(18);
    mission_title.setColor(sf::Color::Black);
    mission_title.setString("Hunting Kacheek");
    mission_desc.setFont(font);
    mission_desc.setCharacterSize(18);
    mission_desc.setColor(sf::Color::Black);
    mission_desc.setString("(no translation needed)");

    unavailable.load("resources/graphics/ui/worldmap/unavailable.png");
    location_highlight.load("resources/graphics/ui/worldmap/location_highlight.png");
    location_highlight.setOrigin(location_highlight.getLocalBounds().width / 2, location_highlight.getLocalBounds().height / 2);

    mission_select.load("resources/graphics/ui/worldmap/mission_select.png");

    int fields = 6; //make it be detected via a file or automatically, later
    for (int i = 1; i <= fields; i++)
    {
        SpriteWrapper bg;
        bg.load("resources/graphics/ui/worldmap/locationbg_" + to_string(i) + ".png");
        preloaded_location_bgs.push_back(bg);

        SpriteWrapper loc;
        loc.load("resources/graphics/ui/worldmap/location_" + to_string(i) + ".png");
        preloaded_worldmap_icons.push_back(loc);
    }

    ctrlTips.create(66*3, font, 20, sf::String("Left/Right: Select field      X: View missions      O: Exit to Patapolis"), quality);

    SPDLOG_INFO("Initializing Obelisk finished.");
}

void ObeliskMenu::addMission(json missiondata)
{
    Mission tmp;
    try
    {
        tmp.mis_ID = missiondata["mission_id"];
    } catch (const std::exception& e)
    {
        tmp.mis_ID = 0;
    }
    try
    {
        tmp.loc_ID = missiondata["location_id"];
    } catch (const std::exception& e)
    {
        tmp.loc_ID = 1;
    }

    try
    {
        std::string title_key = missiondata["mission_title"];
        tmp.title = title_key;
    } catch (const std::exception& e)
    {
        std::string title = "No Data";
        tmp.title = string(title.begin(), title.end());
    }
    try
    {
        std::string desc_key = missiondata["mission_description"];
        tmp.desc = desc_key;
    } catch (const std::exception& e)
    {
        std::string desc = "No Data";
        tmp.desc = string(desc.begin(), desc.end());
    }

    try
    {
        tmp.mission_file = missiondata["mission_file"];
    } catch (const std::exception& e)
    {
        tmp.mission_file = "mis1_1.p4m";
    }

    string level = "";

    if (CoreManager::getInstance().getSaveReader()->mission_levels[tmp.mis_ID] != 0)
    {
        level = to_string(CoreManager::getInstance().getSaveReader()->mission_levels[tmp.mis_ID]);
    }

    auto strRepo = CoreManager::getInstance().getStrRepo();
    std::string font = strRepo->GetFontNameForLanguage(strRepo->GetCurrentLanguage());

    PText tm;
    tm.setFont(font);
    tm.setCharacterSize(18);
    tm.setColor(sf::Color::Black);
    tm.setString("");
    tm.setString(tmp.title + level);
    tm.setOrigin(tm.getLocalBounds().width / 2, tm.getLocalBounds().height / 2);
    tmp.p_mis = tm;

    SPDLOG_DEBUG("[WorldMap] Making text {}", tmp.title);

    missions.push_back(tmp);
}

void ObeliskMenu::Reload()
{
    location_bgs.clear();
    location_icons.clear();

    worldmap_fields.clear();
    worldmap_icons.clear();

    SPDLOG_TRACE("Location_bgs capacity: {}", location_bgs.capacity());

    ///Access the save data
    fields_unlocked = CoreManager::getInstance().getSaveReader()->locations_unlocked;
    missions_unlocked = CoreManager::getInstance().getSaveReader()->missions_unlocked;

    SpriteWrapper fld;
    fld.load("resources/graphics/ui/worldmap/location_field.png");

    ///worldmap contents
    // i is location id, not an idx in fields_unlocked
    for (int i = 1; i <= *std::max_element(fields_unlocked.begin(), fields_unlocked.end()); i++)
    {
        if (std::find(fields_unlocked.begin(), fields_unlocked.end(), i) != fields_unlocked.end())
        {
            int curIdx = std::find(fields_unlocked.begin(), fields_unlocked.end(), i) - fields_unlocked.begin();
            location_bgs.push_back(preloaded_location_bgs[fields_unlocked[curIdx]-1]);
            worldmap_icons.push_back(preloaded_worldmap_icons[fields_unlocked[curIdx]-1]);
        } else
        {
            worldmap_fields.push_back(fld);
            worldmap_icons.push_back(dullpon);
        }
    }

    for (int i = 1; i <= 20; i++)
    {
        worldmap_fields.push_back(fld);

        if (i <= location_icons.size())
        {
            worldmap_icons.push_back(location_icons[i - 1]);
        } else
        {
            worldmap_icons.push_back(dullpon);
        }
    }

    //if (sel_location > 0 && sel_location < location_bgs.size())
    //{
    //    location_bg_a = location_bgs[sel_location - 1];
    //    location_bg_b = location_bgs[sel_location - 1];
    //}

    unlocked.clear();

    for (const auto& field : fields_unlocked)
    {
        unlocked.push_back(field - 1);
    }

    //vector<int> unlocked = {0};
}

void ObeliskMenu::Update()
{
    if (true)
    {
        InputController* inputCtrl = CoreManager::getInstance().getInputController();
        sf::RenderWindow* window = CoreManager::getInstance().getWindow();
        float fps = CoreManager::getInstance().getCore()->getFPS();

        window->setView(window->getDefaultView());

        float resRatioX = window->getSize().x / float(3840);
        float resRatioY = window->getSize().y / float(2160);

        if (!displayMissions)
        {
            mainbox_destX = 640*3;
            mainbox_destY = 320*3;
        } else
        {
            mainbox_destX = 640*3;
            mainbox_destY = 230*3;
        }

        if (round(mainbox.getPosition().x) != mainbox_destX)
        {
            float speed = abs(mainbox.getPosition().x - mainbox_destX) * 10;

            if (mainbox.getPosition().x > mainbox_destX)
                mainbox.setPosition(sf::Vector2f(mainbox.getPosition().x - (speed / fps), mainbox.getPosition().y));
            else
                mainbox.setPosition(sf::Vector2f(mainbox.getPosition().x + (speed / fps), mainbox.getPosition().y));
        }

        if (round(mainbox.getPosition().y) != mainbox_destY)
        {
            float speed = abs(mainbox.getPosition().y - mainbox_destY) * 10;

            if (mainbox.getPosition().y > mainbox_destY)
                mainbox.setPosition(sf::Vector2f(mainbox.getPosition().x, mainbox.getPosition().y - (speed / fps)));
            else
                mainbox.setPosition(sf::Vector2f(mainbox.getPosition().x, mainbox.getPosition().y + (speed / fps)));
        }

        if (round(alphaA) != location_bg_a_destAlpha)
        {
            float speed = abs(alphaA - location_bg_a_destAlpha) * 8;

            if (alphaA > location_bg_a_destAlpha)
                alphaA -= speed / fps;
            else
                alphaA += speed / fps;
        }

        alphaB = 255 - alphaA;

        int renderPrev = sel_prevlocation - 1;

        if (renderPrev > location_bgs.size() - 1)
            renderPrev = location_bgs.size() - 1;

        int renderCur = sel_location - 1;

        if (renderCur > location_bgs.size() - 1)
            renderCur = location_bgs.size() - 1;

        if (!location_bgs.empty())
        {
            location_bgs[renderPrev].setColor(sf::Color(255, 255, 255, 255));
            location_bgs[renderPrev].draw();

            location_bgs[renderCur].setColor(sf::Color(255, 255, 255, alphaB));
            location_bgs[renderCur].draw();
        }

        mainbox.draw();

        worldmap_title.setOrigin(worldmap_title.getLocalBounds().width / 2, worldmap_title.getLocalBounds().height / 2);
        worldmap_title.setPosition(mainbox.getPosition().x - 303*3, mainbox.getPosition().y - 174*3);
        worldmap_title.draw();

        location_title.setOrigin(location_title.getLocalBounds().width / 2, location_title.getLocalBounds().height / 2);
        location_title.setPosition(mainbox.getPosition().x, mainbox.getPosition().y + 72*3);
        location_title.draw();

        location_desc.setOrigin(location_desc.getLocalBounds().width / 2, location_desc.getLocalBounds().height / 2);
        location_desc.setPosition(mainbox.getPosition().x, mainbox.getPosition().y + 142*3);
        location_desc.draw();

        if (round(mapX) != mapXdest)
        {
            float speed = abs(mapX - mapXdest) * 10;

            if (mapX > mapXdest)
                mapX -= speed / fps;
            else
                mapX += speed / fps;
        }

        if (mapX >= 0)
            mapX = 0;

        int maxBound = (worldmap_fields.size() * 123*3 - 1012*3) * (-1);

        if (mapX <= maxBound)
            mapX = maxBound;

        v_render_map.setSize(1012*3 * resRatioX, 720*3 * resRatioY);
        v_render_map.setCenter((506*3) * resRatioX, -360*3 * resRatioY);
        v_render_map.setViewport(sf::FloatRect(134.0*3 / 3840.0, (mainbox.getPosition().y - 115*3) / 2160.0, 1012.0*3 / 3840.0, 1.f));

        window->setView(v_render_map);

        for (int i = 0; i < worldmap_fields.size(); i++)
        {
            worldmap_fields[i].setPosition(i * 123*3 + mapX, 0 - 720*3);
            worldmap_fields[i].draw();
        }

        location_highlight.setOrigin(location_highlight.getLocalBounds().width / 2, location_highlight.getLocalBounds().height / 2);
        location_highlight.setPosition(sel_location * 123*3 + mapX - 62*3, 78*3 - 720*3);
        location_highlight.draw();

        for (int i = 0; i < worldmap_fields.size(); i++)
        {
            worldmap_icons[i].setOrigin(worldmap_icons[i].getLocalBounds().width / 2, worldmap_icons[i].getLocalBounds().height);
            worldmap_icons[i].setPosition(i * 123*3 + mapX + 61*3, 155*3 - 720*3);
            worldmap_icons[i].draw();

            if (std::find(unlocked.begin(), unlocked.end(), i) == unlocked.end())
            {
                unavailable.setPosition(i * 123*3 + mapX - 1*3, 0 - 720*3);
                unavailable.draw();
            }
        }

        window->setView(window->getDefaultView());

        if (displayMissions)
        {
            missionbox.draw();
            descbox.draw();

            select_quest.setOrigin(0, select_quest.getLocalBounds().height / 2);
            select_quest.setPosition(missionbox.getPosition().x - 141*3, missionbox.getPosition().y - 74*3);
            select_quest.draw();

            mission_title.setOrigin(0, mission_title.getLocalBounds().height / 2);
            mission_title.setPosition(descbox.getPosition().x - 315*3, descbox.getPosition().y - 74*3);
            mission_title.draw();

            mission_desc.setOrigin(0, 0);
            mission_desc.setPosition(descbox.getPosition().x - 315*3, descbox.getPosition().y - 55*3);
            mission_desc.draw();

            mission_select.setPosition(135*3, missionbox.getPosition().y - 56*3 + (sel_mission * 24*3));
            mission_select.draw();

            v_render_missions_map.setSize(298*3 * resRatioX, 120*3 * resRatioY);
            v_render_missions_map.setCenter((1280*3 + 149*3) * resRatioX, (-720*3 + 60*3) * resRatioY);
            v_render_missions_map.setViewport(sf::FloatRect(143.0*3 / 3840, (missionbox.getPosition().y - 53*3) / 2160.0, 298.0*3 / 3840.0, 120.0*3 / 2160.0));

            window->setView(v_render_missions_map);

            for (int i = 0; i < missions.size(); i++)
            {
                missions[i].p_mis.setOrigin(0, 0);
                missions[i].p_mis.setPosition(1280*3, -720*3 + (i * 24*3));
                missions[i].p_mis.draw();
            }

            window->setView(window->getDefaultView());
        }

        auto strRepo = CoreManager::getInstance().getStrRepo();
        std::string font = strRepo->GetFontNameForLanguage(strRepo->GetCurrentLanguage());

        if (screenFade.checkFinished())
        {
            if (inputCtrl->isKeyPressed(Input::Keys::CIRCLE))
            {
                if (displayMissions)
                {
                    ctrlTips.create(66*3, font, 20, sf::String("Left/Right: Select field      X: View missions      O: Exit to Patapolis"), quality);

                    displayMissions = false;
                    SPDLOG_DEBUG("Exited mission selection.");
                } else
                {
                    //go back to patapolis
                    screenFade.Create(ScreenFade::FADEOUT, 1024);
                    goto_id = 0;

                    SPDLOG_INFO("Exited Obelisk.");
                }
            } else if (inputCtrl->isKeyPressed(Input::Keys::CROSS))
            {
                if (!displayMissions)
                {
                    SPDLOG_INFO("Displaying missions on Worldmap for location {}.", sel_location);

                    ///(re)load missions here
                    SPDLOG_DEBUG("Displaying missions");

                    missions.clear();

                    std::ifstream wmap("resources/missions/worldmap.dat", std::ios::in);
                    json wmap_data;

                    if (wmap.good())
                    {
                        wmap >> wmap_data;

                        for (const auto& missiondata : wmap_data)
                        {
                            if (missiondata["location_id"] == sel_location && CoreManager::getInstance().getSaveReader()->isMissionUnlocked(missiondata["mission_id"]))
                            {
                                addMission(missiondata);
                            }
                        }
                    }

                    wmap.close();

                    if (missions.size() > 0)
                    {
                        ctrlTips.create(66*3, font, 20, sf::String("Up/Down: Select mission      X: Enter mission      O: Return to field select"), quality);

                        displayMissions = true;

                        string level = "";

                        if (CoreManager::getInstance().getSaveReader()->mission_levels[missions[sel_mission].mis_ID] != 0)
                        {
                            level = to_string(CoreManager::getInstance().getSaveReader()->mission_levels[missions[sel_mission].mis_ID]);
                        }

                        mission_title.setString(missions[sel_mission].title + level);
                        string desc = Func::wrap_text(missions[sel_mission].desc, 633, font, 18);
                        mission_desc.setString(desc);
                    }
                } else
                {
                    //go to barracks
                    screenFade.Create(ScreenFade::FADEOUT, 1024);
                    goto_id = 1;

                    SPDLOG_INFO("Set barracks mission to ID {}, mission file: {}", missions[sel_mission].mis_ID, missions[sel_mission].mission_file);
                }
            } else if (inputCtrl->isKeyPressed(Input::Keys::LTRIGGER))
            {
                SPDLOG_DEBUG("Skipping maps to the left (Q key).");

                mapXdest += float(123) * 6;

                if (mapXdest >= 0)
                    mapXdest = 0;
            } else if (inputCtrl->isKeyPressed(Input::Keys::RTRIGGER))
            {
                SPDLOG_DEBUG("Skipping maps to the right (E key).");

                mapXdest -= float(123) * 6;

                int maxBound = (worldmap_fields.size() * 123*3 - 1012*3) * (-1);

                if (mapXdest <= maxBound)
                    mapXdest = maxBound;
            } else if (inputCtrl->isKeyPressed(Input::Keys::LEFT))
            {
                if (!displayMissions)
                {
                    //mapXdest += float(123);

                    //if(mapXdest >= 0)
                    //mapXdest = 0;

                    sel_prevlocation = sel_location;
                    sel_location--;
                    sel_mission = 0;

                    if (sel_location <= 1)
                        sel_location = 1;

                    SPDLOG_DEBUG("Selecting Obelisk location {}", sel_location);

                    //if((sel_location*123 + mapX - 62) < 0)
                    //{
                    mapXdest = (sel_location * 123*3 - 615*3) * (-1);

                    if (mapXdest >= 0)
                        mapXdest = 0;
                    //}

                    if (sel_location - 1 < location_bgs.size())
                    {
                        alphaA = 255;
                        alphaB = 0;

                        location_bg_a_destAlpha = 0;
                        location_bg_b_destAlpha = 255;
                    }

                    if (std::find(unlocked.begin(), unlocked.end(), sel_location - 1) != unlocked.end())
                    {
                        string L1 = "worldmap_location_" + to_string(sel_location) + "_title";
                        string L2 = "worldmap_location_" + to_string(sel_location) + "_description";
                        string wL1 = string(L1.begin(), L1.end());
                        string wL2 = string(L2.begin(), L2.end());

                        string desc = Func::wrap_text(wL2, 800, font, 18);

                        location_title.setString(wL1);
                        location_desc.setString(desc);
                    } else
                    {
                        string L1 = "worldmap_location_locked";
                        string wL1 = string(L1.begin(), L1.end());

                        location_title.setString(wL1);
                        location_desc.setString("");
                    }
                }
            } else if (inputCtrl->isKeyPressed(Input::Keys::RIGHT))
            {
                if (!displayMissions)
                {
                    //mapXdest -= float(123);

                    //int maxBound = (worldmap_fields.size()*123 - 1012) * (-1);

                    //if(mapXdest <= maxBound)
                    //mapXdest = maxBound;

                    sel_prevlocation = sel_location;
                    sel_location++;
                    sel_mission = 0;

                    if (sel_location >= worldmap_fields.size())
                        sel_location = worldmap_fields.size();

                    SPDLOG_DEBUG("Selecting Obelisk location {}", sel_location);

                    //if((sel_location*123 + mapX - 62 + 176 + 246) > 1012)
                    //{
                    //mapXdest -= float(123);
                    mapXdest = (sel_location * 123*3 - 615*3) * (-1);

                    int maxBound = (worldmap_fields.size() * 123*3 - 1012*3) * (-1);

                    if (mapXdest <= maxBound)
                        mapXdest = maxBound;
                    //}

                    if (sel_location - 1 < location_bgs.size())
                    {
                        alphaA = 255;
                        alphaB = 0;

                        location_bg_a_destAlpha = 0;
                        location_bg_b_destAlpha = 255;
                    }

                    if (std::find(unlocked.begin(), unlocked.end(), sel_location - 1) != unlocked.end())
                    {
                        string L1 = "worldmap_location_" + to_string(sel_location) + "_title";
                        string L2 = "worldmap_location_" + to_string(sel_location) + "_description";
                        string wL1 = string(L1.begin(), L1.end());
                        string wL2 = string(L2.begin(), L2.end());

                        string desc = Func::wrap_text(wL2, 800*3, font, 18);

                        location_title.setString(wL1);
                        location_desc.setString(desc);
                    } else
                    {
                        string L1 = "worldmap_location_locked";
                        string wL1 = string(L1.begin(), L1.end());

                        location_title.setString(wL1);
                        location_desc.setString("");
                    }
                }
            } else if (inputCtrl->isKeyPressed(Input::Keys::UP))
            {
                if (displayMissions)
                {
                    if (sel_mission > 0)
                        sel_mission--;

                    SPDLOG_DEBUG("Selecting Obelisk mission {}", sel_mission);

                    string level = "";

                    if (CoreManager::getInstance().getSaveReader()->mission_levels[missions[sel_mission].mis_ID] != 0)
                    {
                        level = to_string(CoreManager::getInstance().getSaveReader()->mission_levels[missions[sel_mission].mis_ID]);
                    }

                    mission_title.setString(missions[sel_mission].title + level);
                    string desc = Func::wrap_text(missions[sel_mission].desc, 633*3, font, 18);
                    mission_desc.setString(desc);
                }
            } else if (inputCtrl->isKeyPressed(Input::Keys::DOWN))
            {
                if (displayMissions)
                {
                    if (sel_mission < missions.size() - 1)
                        sel_mission++;

                    SPDLOG_DEBUG("Selecting Obelisk mission {}", sel_mission);

                    string level = "";

                    if (CoreManager::getInstance().getSaveReader()->mission_levels[missions[sel_mission].mis_ID] != 0)
                    {
                        level = to_string(CoreManager::getInstance().getSaveReader()->mission_levels[missions[sel_mission].mis_ID]);
                    }

                    mission_title.setString(missions[sel_mission].title + level);
                    string desc = Func::wrap_text(missions[sel_mission].desc, 633*3, font, 18);
                    mission_desc.setString(desc);
                }
            }
        }

        ctrlTips.x = 0;
        ctrlTips.y = (2160 - ctrlTips.ySize);
        ctrlTips.draw();

        screenFade.draw();

        if (screenFade.checkFinished())
        {
            switch (goto_id)
            {
                case 0: {
                    StateManager::getInstance().setState(StateManager::PATAPOLIS);
                    break;
                }

                case 1: {
                    StateManager::getInstance().setState(StateManager::BARRACKS);
                    break;
                }
            }

            goto_id = -1;
        }
    }
}

ObeliskMenu::~ObeliskMenu()
{
    //dtor
}
