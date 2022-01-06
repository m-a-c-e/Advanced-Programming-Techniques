/*
 * Author: Manan Patel
 * Class: ECE 6122
 * Last Date Modified: 10/27/2021
 * Description:
 * This project runs Buzzy's Revenge, a game developed to satisfy the
 * requirements mentioned in the Assignment 4 for this class.
*/


#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <chrono>
#include <random>

#define RAD_TO_DEG 180 / 3.1415
#define DEG_TO_RAD 3.1415 / 180 

using namespace sf;

class BuzzyGameText 
{
// Stores the text for the game and its properties
private:

public:
    Text txt_msg;
    Font font;
    FloatRect txt_rect;
    int charSize;
    float xPos;
    float yPos;

    BuzzyGameText(String msg, String font, int charSize, float xPos, float yPos, String red) 
    {
        // Separate Constructor for Red text
        this->charSize = charSize;
        this->font.loadFromFile(font);
        this->txt_msg.setFont(this->font);
        this->txt_msg.setString(msg);
        this->txt_msg.setCharacterSize(charSize);
        this->txt_msg.setFillColor(Color::Red);
        this->txt_rect = txt_msg.getLocalBounds();
        this->txt_msg.setOrigin(txt_rect.left + txt_rect.width / 2.0, txt_rect.top + txt_rect.height / 2.0f);
        this->xPos = xPos;
        this->yPos = yPos;
        this->txt_msg.setPosition(xPos, yPos);
    }

    BuzzyGameText(String msg, String font, int charSize, float xPos, float yPos) 
    {
        // Constructor for white text
        this->charSize = charSize;
        this->font.loadFromFile(font);
        this->txt_msg.setFont(this->font);
        this->txt_msg.setString(msg);
        this->txt_msg.setCharacterSize(charSize);
        this->txt_msg.setFillColor(Color::White);
        this->txt_rect = txt_msg.getLocalBounds();
        this->txt_msg.setOrigin(txt_rect.left + txt_rect.width / 2.0, txt_rect.top + txt_rect.height / 2.0f);
        this->xPos = xPos;
        this->yPos = yPos;
        this->txt_msg.setPosition(xPos, yPos);
    }


    void updateText(std::string msg) 
    {
        // function to update the text
        // Input: string msg to change the text to
        // Returns: void
        txt_msg.setString(msg);
    }
};

class BuzzyGameSprite 
{
// Class to store the sprite objects of the game and their properties
private:

public:
    int row = 0;
    int col = 0;
    Texture texture;
    Sprite sprite;
    std::string picture;
    float xPos;
    float yPos;
    float init_xPos = 0;
    float init_yPos = 0;
    float vel = 0;
    float x_vel = 0;
    float y_vel = 0;
    float initial_vel = 0;
    float initial_x_vel = 0;
    float initial_y_vel = 0;


    BuzzyGameSprite(std::string picture, float xPos, float yPos, bool background):xPos(xPos), yPos(yPos), picture(picture) 
    {
        // Constructor for background. Only gets called if background is set
        // while initializing an obj of this class
        texture.loadFromFile(picture);
        sprite.setPosition(xPos, yPos);
        sprite.setTexture(texture);
    }


    BuzzyGameSprite(std::string picture, float xPos, float yPos):xPos(xPos), yPos(yPos), picture(picture) 
    {
        // Constructor for all other game objects. To initialize background, see above.
        FloatRect rect = sprite.getLocalBounds();
        texture.loadFromFile(picture);
        sprite.setPosition(xPos, yPos);
        sprite.setTexture(texture);
    }


    void loadTexture() 
    {
        // function to load the texture
        // Input: none
        // Returns: void
        texture.loadFromFile(picture);
        sprite.setTexture(texture);
    }


    void changePos(float x, float y) 
    {
        // function to change position of sprite
        // Input: x and y (pixel co-ordinates)
        // output: void
        this->xPos = x;
        this->yPos = y;
        this->sprite.setPosition(xPos, yPos);
    }
};


float euclidean_dist(float x1, float y1, float x2, float y2) 
{
    // to calculate the distance between two pixels
    // input: x1, y1, x2, y2 - pixel co-oridnates
    // returns: eulidean distance between the two pixels
    float d_x = abs(x1 - x2);
    float d_y = abs(y1 - y2);
    return sqrt(d_x * d_x + d_y * d_y);
}


int main()
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);

    // create a video mode object
    VideoMode vm(1920, 1080);
    
    // create and open a window for the game
    RenderWindow window(vm, "Buzzy", Style::Fullscreen);

    // BuzzyGame background
    BuzzyGameSprite background("background.png", 0, 0, true);

    // Make 5 buzzy lives
    std::vector <BuzzyGameSprite> buzzy_lives;
    for (int i = 0; i < 5; i++) 
    {
        buzzy_lives.push_back(BuzzyGameSprite("buzzy life.png", 210 + i * 100, 40));
    }

    // Make animal BuzzyGameSprites
    BuzzyGameSprite tiger = BuzzyGameSprite("tiger.png", 1700, 700);
    BuzzyGameSprite Georgia_Bulldogs_logo_dog_g = BuzzyGameSprite("Georgia_Bulldogs_logo_dog_g.png", 1500, 250);
    BuzzyGameSprite dog =  BuzzyGameSprite("dog.png", 1500, 850);
    BuzzyGameSprite sheep = BuzzyGameSprite("sheep.png", 1700, 850);
    BuzzyGameSprite frog = BuzzyGameSprite("frog.png", 1500, 700);
    BuzzyGameSprite angry_unicorn = BuzzyGameSprite("angry_unicorn.png", 1500, 550);
    BuzzyGameSprite bunny = BuzzyGameSprite("bunny.png", 1700, 550);
    BuzzyGameSprite chicken = BuzzyGameSprite("chicken.png", 1500, 400);
    BuzzyGameSprite mouse = BuzzyGameSprite("mouse.png", 1700, 400);
    BuzzyGameSprite pig = BuzzyGameSprite("pig.png", 1700, 250);

    // Store them in vector
    std::vector <BuzzyGameSprite*> woodland_creatures;
    woodland_creatures.push_back(&dog);
    woodland_creatures.push_back(&sheep);
    woodland_creatures.push_back(&frog);
    woodland_creatures.push_back(&angry_unicorn);
    woodland_creatures.push_back(&bunny);
    woodland_creatures.push_back(&chicken);
    woodland_creatures.push_back(&mouse);
    woodland_creatures.push_back(&pig);

    // Drawing small buzzy    
    BuzzyGameSprite smallbuzzy = BuzzyGameSprite("smallbuzzy.png", 0, 500);
    smallbuzzy.init_xPos = 0;
    smallbuzzy.init_yPos = 500;

    // Drawing bee
    BuzzyGameSprite bee = BuzzyGameSprite("bee.png", 10, 400);

    // BuzzyGameText text
    BuzzyGameText txt_Lives("Lives", "KOMIKAP_.ttf", 50, 130, 70);
    BuzzyGameText txt_Score("Score: ", "KOMIKAP_.ttf", 50, 1700, 70);
    int score = 0;
    BuzzyGameText txt_99(std::to_string(score), "KOMIKAP_.ttf", 50, 1830, 70);
    BuzzyGameText txt_BR("Buzzy's Revenge", "KOMIKAP_.ttf", 70, 1000, 170, "red");
    BuzzyGameText txt_l1("Press Enter to restart game", "KOMIKAP_.ttf", 52, 1060, 360);
    BuzzyGameText txt_l2("Press Esc to exit", "KOMIKAP_.ttf", 52, 1060, 460);
    BuzzyGameText txt_l3("Press Space to Powerup", "KOMIKAP_.ttf", 52, 1060, 560);
    BuzzyGameText txt_l4("Press Up/Down arrow to aim", "KOMIKAP_.ttf", 52, 1060, 660);
    BuzzyGameText txt_l5("Created by: Manan Patel", "KOMIKAP_.ttf", 45, 1000, 870);
    BuzzyGameText txt_power("Power", "KOMIKAP_.ttf", 45, 160, 980);
    
    // creating a rectangle to contain the power rectangle
    RectangleShape rectangle;
    rectangle.setSize(sf::Vector2f(500, 50));
    rectangle.setFillColor(sf::Color::Transparent);
    rectangle.setOutlineColor(sf::Color::White);
    rectangle.setOutlineThickness(5);
    rectangle.setPosition(260, 950);

    // creating rectangle to denote power
    RectangleShape power_rectangle;
    power_rectangle.setSize(sf::Vector2f(0, 50));
    power_rectangle.setFillColor(sf::Color::Red);
    power_rectangle.setOutlineColor(sf::Color::White);
    power_rectangle.setOutlineThickness(5);
    power_rectangle.setPosition(260, 950);

    // detect key press only once
    window.setKeyRepeatEnabled(true);

    Clock clock;
    float t;                        // used to store time
    bool game_started = false;      // condition for starting game
    bool set_position_once = true;  // condition to set the position of text and sprites
    int lives = 5;                  // counter for lives left
    float a = 20;                   // acceleration
    bool launch_buzzy = false;      // condition to set buzzy to move
    bool reset_buzzy = true;        // making buzzy go back to initial position
    int hit_count = 0;              // counter for no of mascots hit

    while (window.isOpen()) 
    {
        if (launch_buzzy) 
        {
            t = clock.getElapsedTime().asSeconds();

            // Update x and y positions.
            float new_x = smallbuzzy.init_xPos + smallbuzzy.initial_x_vel + smallbuzzy.x_vel * t;
            float new_y = smallbuzzy.init_yPos + smallbuzzy.initial_y_vel + smallbuzzy.y_vel * t + 0.5 * a * t * t;

            // Update x and y velocity.
            smallbuzzy.y_vel = smallbuzzy.initial_y_vel + a * t;
            smallbuzzy.x_vel = smallbuzzy.initial_x_vel;

            // Update Buzzy's angle
            float angle = atan2(smallbuzzy.y_vel, smallbuzzy.x_vel) * RAD_TO_DEG;
            float curr_angle = smallbuzzy.sprite.getRotation();
            smallbuzzy.sprite.rotate(angle - curr_angle);

            // Update Buzzy's Position
            smallbuzzy.changePos(new_x, new_y);
        }
        
        // when buzzy goes outside the screen
        if (smallbuzzy.xPos > 1920 or smallbuzzy.yPos > 1080 or smallbuzzy.yPos < 0) 
        {
            lives--;
            reset_buzzy = true;
            launch_buzzy = false;
        }

        // check for buzzy hitting woodland creature or mascots
        FloatRect sb_rect = smallbuzzy.sprite.getLocalBounds();
        for (int i = 0; i < 8; i++) 
        {
            if (woodland_creatures[i]->col != hit_count) {
                // do not check for creatures which were removed if mascot was hit before this 
                continue;
            }
            
            // getting the centre of creature, tiger and bulldog
            FloatRect creature = woodland_creatures[i]->sprite.getLocalBounds();
            FloatRect tiger_rect = tiger.sprite.getLocalBounds();
            FloatRect bulldog_rect = Georgia_Bulldogs_logo_dog_g.sprite.getLocalBounds();

            float x_sb = smallbuzzy.xPos + sb_rect.width / 1.3;
            float y_sb = smallbuzzy.yPos + sb_rect.height / 2.0;

            float theta = atan2(y_sb - smallbuzzy.yPos, x_sb - smallbuzzy.xPos);
            float gamma = smallbuzzy.sprite.getRotation() * DEG_TO_RAD;
            float final_angle = theta + gamma;

            float x_pig = woodland_creatures[i]->xPos + creature.width / 2.0;
            float y_pig = woodland_creatures[i]->yPos + creature.height / 2.0;

            float x_tiger = tiger.xPos + tiger_rect.width / 2.0;
            float y_tiger = tiger.yPos + tiger_rect.height / 2.0;

            float x_bulldog = Georgia_Bulldogs_logo_dog_g.xPos + bulldog_rect.width / 2.0;
            float y_bulldog = Georgia_Bulldogs_logo_dog_g.yPos + bulldog_rect.height / 2.0;

            float d_to_pig = euclidean_dist(x_sb, y_sb, x_pig, y_pig);
            float d_to_tiger = euclidean_dist(x_sb, y_sb, x_tiger, y_tiger);
            float d_to_bulldog = euclidean_dist(x_sb, y_sb, x_bulldog, y_bulldog);

            // check if head of buzzy within 90 pixel dist to the centre
            if (d_to_tiger <= 90 && hit_count == tiger.col) 
            {
                score += 25;
                txt_99.updateText(std::to_string(score));
                launch_buzzy = false;
                reset_buzzy = true;
                hit_count++;
                break;
            }
            else if (d_to_bulldog <= 90 && hit_count == Georgia_Bulldogs_logo_dog_g.col) 
            {
                score += 25;
                txt_99.updateText(std::to_string(score));
                launch_buzzy = false;
                reset_buzzy = true;
                hit_count++;
                break;
            }
            else 
            {
                if (d_to_pig <= 90) 
                {
                    std::cout << "colliding with pig" << std::endl;
                    lives--;
                    launch_buzzy = false;
                    reset_buzzy = true;
                    break;
                }
            }
        }

        // restart game when lives = 0
        if (lives == 0) 
        {
            game_started = false;
            set_position_once = true;
            reset_buzzy = true;
            launch_buzzy = false;
        }

        // resetting level. Should not be confused with resetting game.
        if (reset_buzzy) 
        {
            smallbuzzy.initial_vel = 0;
            smallbuzzy.initial_x_vel = 0;
            smallbuzzy.initial_y_vel = 0;
            smallbuzzy.x_vel = 0;
            smallbuzzy.y_vel = 0;
            smallbuzzy.xPos = smallbuzzy.init_xPos;
            smallbuzzy.yPos = smallbuzzy.init_yPos;
            smallbuzzy.changePos(smallbuzzy.xPos, smallbuzzy.yPos);
            smallbuzzy.sprite.setRotation(0);
            reset_buzzy = false;
        }

        if (game_started) 
        {
            // remove all the instructions
            txt_BR.txt_msg.setFillColor(Color::Transparent);
            txt_l1.txt_msg.setFillColor(Color::Transparent);
            txt_l2.txt_msg.setFillColor(Color::Transparent);
            txt_l3.txt_msg.setFillColor(Color::Transparent);
            txt_l4.txt_msg.setFillColor(Color::Transparent);
            txt_l5.txt_msg.setFillColor(Color::Transparent);
        }
        else 
        {
            // add all the instructions
            txt_BR.txt_msg.setFillColor(Color::Red);
            txt_l1.txt_msg.setFillColor(Color::White);
            txt_l2.txt_msg.setFillColor(Color::White);
            txt_l3.txt_msg.setFillColor(Color::White);
            txt_l4.txt_msg.setFillColor(Color::White);
            txt_l5.txt_msg.setFillColor(Color::White);
            lives = 5;
            hit_count = 0;
            score = 0;

            // set the positions for sprites
            if (set_position_once) 
            {
                std::uniform_int_distribution<int> distribution_1(0, 1);
				std::uniform_int_distribution<int> distribution_2(0, 4);

				int col = distribution_1(generator);            // select 0 or 1 randomly
				int row_tiger = 0;
				int col_tiger = 0;
				int row_bulldog = 0;
				int col_bulldog = 0;
			    if (col == 0) {
					// place tiger in first column and bulldog in second
					float x_loc_tiger = 1500;
					int row = distribution_2(generator);        // randomly select the row for tiger
					float y_loc_tiger = 250 + row * 150;
					tiger.changePos(x_loc_tiger, y_loc_tiger);
					row_tiger = row;
					col_tiger = 0;
					tiger.row = row_tiger;
					tiger.col = col_tiger;

					float x_loc_bulldog = 1700;
					row = distribution_2(generator);             // randomly select the row for bulldog
					float y_loc_bulldog = 250 + row * 150;
					Georgia_Bulldogs_logo_dog_g.changePos(x_loc_bulldog, y_loc_bulldog);
					row_bulldog = row;
					col_bulldog = 1;
					Georgia_Bulldogs_logo_dog_g.row = row_bulldog;
					Georgia_Bulldogs_logo_dog_g.col = col_bulldog;
				} else 
                {
					// place tiger in second column and bulldog in first
					float x_loc_tiger = 1700;
					int row = distribution_2(generator);         // randomly select the row for tiger   
					float y_loc_tiger = 250 + row * 150;
					tiger.changePos(x_loc_tiger, y_loc_tiger);
					row_tiger = row;
					col_tiger = 1;
					tiger.row = row_tiger;
					tiger.col = col_tiger;

					float x_loc_bulldog = 1500;
					row = distribution_2(generator);             // randomly select row for bulldog 
					float y_loc_bulldog = 250 + row * 150;
					Georgia_Bulldogs_logo_dog_g.changePos(x_loc_bulldog, y_loc_bulldog);
					row_bulldog = row;
					col_bulldog = 0;
					Georgia_Bulldogs_logo_dog_g.row = row_bulldog;
					Georgia_Bulldogs_logo_dog_g.col = col_bulldog;
				}
				int nums[8] = {0, 1, 2, 3, 4, 5, 6, 7};
				std::random_shuffle(nums, nums + 8);

				int counter = 0;
				for (int col = 0; col <= 1; col++) 
                {
					for (int row = 0; row <= 4; row++) 
                    {
						int index = nums[counter];              // since nums is shuffled randomly, animal is
																// selected randomly
						if (row == row_tiger && col == col_tiger) 
                        {
							// skip if tiger is in this location
							continue;
						}
						if (row == row_bulldog && col == col_bulldog) 
                        {
							// skip if bulldog in this location
							continue;
						}
						woodland_creatures[index]->changePos(col * 200 + 1500, 250 + row * 150);
						woodland_creatures[index]->row = row;
						woodland_creatures[index]->col = col;
						counter++;
					}
				}
			}
            set_position_once = false;
		}

        Event event;
        while (window.pollEvent(event)) 
        {
            if (event.type == Event::KeyReleased && smallbuzzy.initial_vel != 0 && launch_buzzy == false) 
            {
                // Spacebar was released. Launch buzzy. Reset clock.
                launch_buzzy = true;
                power_rectangle.setSize(Vector2f(0, 50));
                clock.restart();
            }

            if (event.type == Event::KeyPressed) 
            {
                if (Keyboard::isKeyPressed(Keyboard::Return)) 
                {
                    // start game when enter is pressed
                    game_started = true;
                }

                // To set the initial velocity ////////////////////////////////////
                if (Keyboard::isKeyPressed(Keyboard::Space) && game_started && launch_buzzy == false) 
                {
                    Vector2f s = power_rectangle.getSize();
                    if (s.x != 500) 
                    {
                        s.x += 5;
                        // TO DO: 
                        // set the small buzzy velocity over here
                        smallbuzzy.initial_vel = 2 * s.x;
                        smallbuzzy.initial_x_vel = smallbuzzy.initial_vel * cos(smallbuzzy.sprite.getRotation() * DEG_TO_RAD);
					    smallbuzzy.initial_y_vel = smallbuzzy.initial_vel * sin(smallbuzzy.sprite.getRotation() * DEG_TO_RAD);
                        smallbuzzy.init_xPos = smallbuzzy.xPos;
                        smallbuzzy.init_yPos = smallbuzzy.yPos;
                    }
                    power_rectangle.setSize(s);
                }

                // To rotate the small buzzy angle ///////////////////////////////
                if (Keyboard::isKeyPressed(Keyboard::Down) && game_started) 
                {
                    smallbuzzy.sprite.rotate(1);
                }

                if (Keyboard::isKeyPressed(Keyboard::Up) && game_started) 
                {
			        smallbuzzy.sprite.rotate(-1);
                }

                // To Exit the game ///////////////////////////////////////////// 
                if (Keyboard::isKeyPressed(Keyboard::Escape)) 
                {
                    window.close();
                }
            }
        }

        // when both the mascots are hit, restart level
        if (hit_count == 2) 
        {
            hit_count = 0;
            game_started = false;
            set_position_once = true;
            lives = 5;
            launch_buzzy = false;
            reset_buzzy = true;
        }

        // clear everything from last frame    
        window.clear();

        // draw the background
        window.draw(background.sprite);

        // draw tiger and bulldog depending on hit_count
        if (hit_count == 0) 
        {
            window.draw(Georgia_Bulldogs_logo_dog_g.sprite);
            window.draw(tiger.sprite);
        }
        else if (hit_count == 1) 
        {
            if (tiger.col == 1) 
            {
                window.draw(tiger.sprite);
            }
            if (Georgia_Bulldogs_logo_dog_g.col == 1) 
            {
                window.draw(Georgia_Bulldogs_logo_dog_g.sprite);
            }
        }

        // draw wood_land creatures depending on hit_count
        for (int i = 0; i < 8; i++) 
        {
            woodland_creatures[i]->loadTexture();
            if (hit_count == 0) 
            {
                window.draw(woodland_creatures[i]->sprite);
            }
            else if (hit_count == 1) 
            {
                if (woodland_creatures[i]->col == 1) 
                {
                    window.draw(woodland_creatures[i]->sprite);
                }
            }
        }

        // draw bee
        window.draw(bee.sprite);

        // draw buzzy lives
        for (int i = 0; i < lives; i++) 
        {
            buzzy_lives[i].loadTexture();
            window.draw(buzzy_lives[i].sprite);
        }

        // draw text
        window.draw(txt_Lives.txt_msg);
        window.draw(txt_Score.txt_msg);
        window.draw(txt_99.txt_msg);
        window.draw(txt_BR.txt_msg);
        window.draw(txt_l1.txt_msg);
        window.draw(txt_l2.txt_msg);
        window.draw(txt_l3.txt_msg);
        window.draw(txt_l4.txt_msg);
        window.draw(txt_l5.txt_msg);
        txt_99.updateText(std::to_string(score));
        window.draw(txt_99.txt_msg);
        window.draw(txt_power.txt_msg);

        // rectangle
        window.draw(rectangle);
        window.draw(power_rectangle);

        // draw buzzy life
        window.draw(smallbuzzy.sprite);

        // show everything we just drew
        window.display();
    }
    return 0;
}
