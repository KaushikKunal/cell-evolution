#include <cmath>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <thread>

const int NUM_SPECIES = 8;
const int NUM_BLOBS = 2000;
const float MAX_FORCE = 0.05f;
const float MAX_DIST = 50.0f;
const float FRICTION = 0.8f;
const float BLOB_SIZE = 2.5f;
const float REPULSION_DIST = 10.0f;
const float REPULSION_FORCE = 0.1f;

int WINDOW_WIDTH = 1000;
int WINDOW_HEIGHT = 1000;
unsigned int num_threads = 1;

std::vector<std::vector<float> > rule_matrix(NUM_SPECIES, std::vector<float>(NUM_SPECIES));
std::vector<sf::Color> species_colors(NUM_SPECIES);

float random_float(float min, float max) {
    return min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max-min)));
}

int random_int(int min, int max) {
    return min + static_cast <int> (rand()) /( static_cast <int> (RAND_MAX/(max-min)));
}

void generate_rules() {
    for (int i = 0; i < NUM_SPECIES; ++i) {
        for (int j = 0; j < NUM_SPECIES; ++j) {
            rule_matrix[i][j] = random_float(-MAX_FORCE, MAX_FORCE);
        }
    }
}

void generate_colors() {
    for (int i = 0; i < NUM_SPECIES; ++i) {
        species_colors[i] = sf::Color(random_int(0, 255), random_int(0, 255), random_int(0, 255));
    }
}

class Blob {
public:
    Blob(sf::Vector2f position, int species_id)
        : position(position), species_id(species_id) {
            velocity = sf::Vector2f(0.f, 0.f);
            // random small velocity
            velocity.x = random_float(-1.0f, 1.0f);
            velocity.y = random_float(-1.0f, 1.0f);
            size = BLOB_SIZE;
        }

    void interact(std::vector<Blob>& blobs) {
        // loop through all other blobs
        for (auto& other_blob : blobs) {
            // don't interact with yourself
            if (&other_blob == this) {
                continue;
            }
            // calculate the distance between the two blobs
            sf::Vector2f dist = other_blob.getPosition() - position;
            float length = sqrt(dist.x * dist.x + dist.y * dist.y);
            float peak_force = rule_matrix[species_id][other_blob.species_id];
            float force;
            float min_dist = size + other_blob.size + REPULSION_DIST;
            // if the distance is less than the max distance, interact
            if (length < min_dist) {
                force = REPULSION_FORCE * (length / min_dist) - REPULSION_FORCE;
            }
            else if (length < (min_dist + MAX_DIST)/2) {
                force = peak_force * (length - min_dist) / ((min_dist + MAX_DIST) / 2 - min_dist);
            }
            else if (length < MAX_DIST) {
                force = peak_force * (MAX_DIST - length) / (MAX_DIST - (min_dist + MAX_DIST) / 2);
            }
            else {
                continue;
            }
            // apply the force to the velocity
            sf::Vector2f force_vector = dist / length * force;
            velocity += force_vector;
        }
    }

    void update() {
        // Update the blob's position based on its velocity
        position += velocity;

        // Apply friction to the velocity
        velocity *= FRICTION;

        // bounce off walls
        if (position.x < 0.0f) {
            position.x = 0.0f;
            velocity.x *= -1.0f;
        }
        if (position.x > WINDOW_WIDTH) {
            position.x = WINDOW_WIDTH;
            velocity.x *= -1.0f;
        }
        if (position.y < 0.0f) {
            position.y = 0.0f;
            velocity.y *= -1.0f;
        }
        if (position.y > WINDOW_HEIGHT) {
            position.y = WINDOW_HEIGHT;
            velocity.y *= -1.0f;
        }
    }

    void draw(sf::RenderWindow& window) const {
        sf::CircleShape shape;
        shape.setFillColor(species_colors[species_id]);
        // shape.setFillColor(color);
        shape.setRadius(size);
        shape.setPosition(position.x, position.y);
        window.draw(shape);
    }

    sf::Color getColor() const {
        return species_colors[species_id];
    }

    sf::Vector2f getPosition() const {
        return position;
    }

    float getSize() const {
        return size;
    }

private:
    
    sf::Vector2f velocity;
    int species_id;
    sf::Vector2f position;
    float size;
};

// interact a certain range of blobs with all other blobs
void interact_blobs(std::vector<Blob>& blobs, int start, int end) {
    for (int i = start; i < end; ++i) {
        blobs[i].interact(blobs);
    }
}

void draw_blobs(sf::RenderWindow& window, std::vector<Blob>& blobs, sf::VertexArray& objects_va, sf::Texture& texture) {
    // 0 for superfast vertex array blobs
    if (0) {
        for (auto& blob : blobs) {
            blob.draw(window);
        }
    }
    else {
        float texture_size = 1024.0f;
        for (uint32_t i = 0; i < blobs.size(); ++i) {
                const Blob& object = blobs[i];
                const uint32_t idx = i << 2;
                const float radius = object.getSize();
                sf::Color color = object.getColor();
                sf::Vector2f pos = object.getPosition();
                objects_va[idx + 0].position = pos + sf::Vector2f(-radius, -radius);
                objects_va[idx + 1].position = pos + sf::Vector2f(radius, -radius);
                objects_va[idx + 2].position = pos + sf::Vector2f(radius, radius);
                objects_va[idx + 3].position = pos + sf::Vector2f(-radius, radius);
                objects_va[idx + 0].texCoords = {0.0f        , 0.0f};
                objects_va[idx + 1].texCoords = {texture_size, 0.0f};
                objects_va[idx + 2].texCoords = {texture_size, texture_size};
                objects_va[idx + 3].texCoords = {0.0f        , texture_size};

                objects_va[idx + 0].color = color;
                objects_va[idx + 1].color = color;
                objects_va[idx + 2].color = color;
                objects_va[idx + 3].color = color;
            }
        window.draw(objects_va, &texture);
    }
    
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML test 2!");
    // sf::CircleShape shape(50.f);
    // shape.setFillColor(sf::Color::Yellow);

    sf::Font font;
    
    if ( !font.loadFromFile( "res/fonts/ComicSansMS3.ttf" ) )
    {
        std::cout << "Error loading file" << std::endl;
        
        //system( "pause" );
    }
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(15);
    text.setFillColor(sf::Color::White);
    text.setPosition(10.0f, 10.0f);

    #include <algorithm> // Add this line to include the <algorithm> header for std::max

    sf::VertexArray objects_va(sf::Quads, NUM_BLOBS * 4);
    sf::Texture texture;
    texture.loadFromFile("res/images/circle.png");

    // print number of threads available
    std::vector<std::thread> threads;
    num_threads = std::max(std::thread::hardware_concurrency(), num_threads);
    std::cout << "Number of threads: " << num_threads << std::endl;

    // For calculating FPS
    sf::Clock fps_clock;
    sf::Clock timer_clock;
    float timeSinceLastUpdate = 0.f;
    int fps = 150;
    float timePerFrame = 1.f / fps; // 60 fps
    // window.setFramerateLimit(fps); // comment this our to uncap


    // // The speed at which the ball moves
    // float speed = 5.0f;
    // sf::Vector2f position = sf::Vector2f(color_distribution(generator), color_distribution(generator));
    // sf::Color color = sf::Color(color_distribution(generator), color_distribution(generator), color_distribution(generator));
    // Blob blob = Blob(position, color);
    // create a vector of blobs, randomizing their positions and colors
    generate_colors();
    generate_rules();
    
    std::vector<Blob> blobs;
    for (int i = 0; i < NUM_BLOBS; ++i) {
        sf::Vector2f position = sf::Vector2f(random_float(0.0f, WINDOW_WIDTH), random_float(0.0f, WINDOW_HEIGHT));
        int species_id = random_int(0, NUM_SPECIES);
        Blob blob = Blob(position, species_id);
        blobs.push_back(blob);
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Handle window resize
            if (event.type == sf::Event::Resized)
            {
                // Create a new view with the size of the window
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
                WINDOW_WIDTH = event.size.width;
                WINDOW_HEIGHT = event.size.height;
            }
            if (event.type == sf::Event::KeyPressed) {
                // Check if the key pressed is the "R" key
                if (event.key.code == sf::Keyboard::R) {
                    generate_rules();
                }
                if (event.key.code == sf::Keyboard::C) {
                    generate_colors();
                }
            }
        }

         // Update the scene
        float elapsedTime = fps_clock.restart().asSeconds();
        timeSinceLastUpdate += elapsedTime;
        if (timeSinceLastUpdate > timePerFrame)
        {
            // Calculate FPS
            float fps = 1.f / elapsedTime;
            text.setString("FPS: " + std::to_string(static_cast<int>(fps)));

            // Reset the timeSinceLastUpdate
            timeSinceLastUpdate = 0.f;
        }


        // Get the current position of the mouse
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        // Convert it to world coordinates
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        // distribute blob amongs threads and interact
        timer_clock.restart();
        threads.clear();
        for (int i = 0; i < num_threads; ++i) {
            threads.push_back(std::thread(interact_blobs, std::ref(blobs), i * NUM_BLOBS / num_threads, (i + 1) * NUM_BLOBS / num_threads));
        }
        for (auto& thread : threads) {
            thread.join();
        }
        for (auto& blob : blobs) {
            blob.update();
        }
        float timer_time = timer_clock.getElapsedTime().asMicroseconds();
        // text.setString("interact time: " + std::to_string(static_cast<int>(timer_time)));
        
        // draw the scene
        window.clear();
        draw_blobs(window, blobs, objects_va, texture);
        window.draw(text);
        window.display();
    }

    return 0;
}
