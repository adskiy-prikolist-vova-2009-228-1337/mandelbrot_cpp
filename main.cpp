#define GLEW_STATIC
#define SFML_STATIC
#define UNICODE
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>

float zoom = 1;
float zoom_in_speed = 1.4;
float zoom_out_speed = 1.8;

float move_speed = 2.3;

double offsetx = -0.5;
double offsety = 0; 

float deltatime;
float prevtime;

//clip (or clamp) function
float clip(float n, float lower, float upper) {
  return std::max(lower, std::min(n, upper));
}

//screen sizes
int w_wdth = 800;
int w_hght = 450;


//some value passed form command line
bool vsync = true;
bool fps = false;

//func to quickly get the text to display
std::string get_info(){
    char str[1024];
    sprintf(str, "fps: %f\nzoom: %f\noffset: (%f, %f)", round(1/deltatime), zoom, offsetx, offsety);
    return str;
}



int main(int argc, char *argv[])
{
    //reading exe run parameters
    if (argc > 1){
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "-r") {
                //its only changes start window size
                //use as main.exe -r 1600 900 (for example)
                w_wdth = atoi(argv[i+1]);
                w_hght = atoi(argv[i+2]);
            }
            else if (arg == "-f"){
                fps = false;
                printf("fps label disabled\n");
            }
            else if (arg == "+f"){
                fps = true;
                printf("fps label enabled\n"); //it enables more than just a fps label
            }
            else if (arg == "+v" || arg == "+vsync"){
                vsync = true;
                printf("vsync on\n");
            }
            else if (arg == "-v" || arg == "-vsync"){
                vsync = false;
                printf("vsync off\n");
            }
        }
    }

    sf::ContextSettings settings;
    settings.majorVersion = 3;
    settings.minorVersion = 3;

    //creating a window
    sf::RenderWindow window(sf::VideoMode(w_wdth, w_hght), "Mandelbrot set", sf::Style::Default, settings);

    if (vsync)
        window.setVerticalSyncEnabled(true);
    
    // activate the window
    window.setActive(true);

    //setting up a clock
    sf::Clock clock;

    //adding main rectangle shape for which shader will be applied later
    sf::RectangleShape shape;
    shape.setSize(sf::Vector2f(w_wdth, w_hght));

    //adding shader
    sf::Shader shader;

    if (!shader.loadFromFile("shaders/vert.glsl", "shaders/frag.glsl"))
    {
        printf("some error with shader loading\n");
    }


    //setting up texts and fonts
    sf::Font font;

    if (!font.loadFromFile("fonts/Roboto-Regular.ttf")){
        printf("failed to load font\n");
    }
    
    sf::Text infotext;

    infotext.setCharacterSize(16);
    if (fps)
        infotext.setFont(font); //if there will be no font it just wont show


    //main cycle
    while (window.isOpen())
    {
        //getting the amount of time elapsed since the start of the program's start
        sf::Time timelapsed = clock.getElapsedTime();
        //and time elapsed between frame updates
        deltatime = timelapsed.asSeconds() - prevtime;
        prevtime = timelapsed.asSeconds();

        //processing events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::F8))
                window.close();
            if (event.type == sf::Event::Resized){
                shape.setSize(sf::Vector2f(event.size.width, event.size.height));
            }
        }

        //updating the window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //getting mousse coord
        sf::Vector2i mousecord = sf::Mouse::getPosition(window);

        //zoom in/out and scary moving realization
        if ((mousecord.x >= 0 && mousecord.y >= 0) && (mousecord.x <= window.getSize().x && mousecord.y <= window.getSize().y)){
            if (window.hasFocus() && window.isOpen()){
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)){
                    zoom -= zoom_in_speed * zoom * deltatime;
                    offsetx += (double) (mousecord.x - (int) window.getSize().x / 2) / (double) ((int)window.getSize().x / 2) * zoom * zoom_in_speed * deltatime * move_speed;
                    offsety -= (double) (mousecord.y - (int) window.getSize().y / 2) / (double) ((int)window.getSize().y / 2) * zoom * zoom_in_speed * deltatime * move_speed;
                }else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)){
                    zoom += zoom_out_speed * zoom * deltatime;
                    if (zoom > 1.5) zoom = 1.5;
                    offsetx += (double) (mousecord.x - (int) window.getSize().x / 2) / (double) ((int)window.getSize().x / 2) * zoom * zoom_out_speed * deltatime * move_speed;
                    offsety -= (double) (mousecord.y - (int) window.getSize().y / 2) / (double) ((int)window.getSize().y / 2) * zoom * zoom_out_speed * deltatime * move_speed;
                }
            }
        }

        //providing all uniform data to shader
        shader.setUniform("screen_size", sf::Vector2i(window.getSize().x, window.getSize().y)); //screen size
        shader.setUniform("iTime", timelapsed.asSeconds()); //time slapsed
        shader.setUniform("zoom", zoom); //zoom level
        shader.setUniform("iRatio", (float) ((int) window.getSize().x / (int) window.getSize().y));

        //clipping offset so that the user cant get too far away from start
        offsetx = clip(offsetx, -2, 1);
        offsety = clip(offsety, -1, 1);
        shader.setUniform("offset", sf::Vector2f(offsetx, offsety));

        //updating text
        infotext.setString(get_info());

        //drawing and display all needed
        window.draw(shape, &shader);
        window.draw(infotext);
        window.display();
    }

    return 0;
}

