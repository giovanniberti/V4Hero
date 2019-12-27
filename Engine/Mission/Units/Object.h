#ifndef OBJECT_H
#define OBJECT_H

#include <SFML/Graphics.hpp>

using namespace std;

class Object
{
    private:
    bool debug = false;

    public:
    std::string texture_path;

    sf::Texture tex_obj;
    sf::Sprite s_obj;

    float x,y,r=0,rs=0; ///local position
    float old_x,old_y,old_r; ///old position
    float or_x,or_y; ///origin (for rotation)
    float s_x=1,s_y=1; ///scale
    int layer = 0;
    int parent = 0;

    struct Frame
    {
        float time; ///timestamp
        float pos_x; ///x position
        float pos_y; ///y position
        float or_x; ///x origin
        float or_y; ///y origin
        float rotation; ///rotation
        float scale_x; ///x scale
        float scale_y; ///y scale
    };

    vector<Frame> frames; ///frames

    Object();
    void Load(string filename, int xpos, int ypos);
    void Load(sf::Texture& texture, int xpos, int ypos);
    void SetFrame(float time);
    void SetCustomFrame(float in_time, float in_pos_x, float in_pos_y, float in_or_x, float in_or_y, float in_rotation, float in_scale_x, float in_scale_y);
    void SetPos(float time);
    void Draw(sf::RenderWindow& window);
};

#endif // OBJECT_H