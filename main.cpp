#include <SFML/Graphics.hpp>

#include <cmath>

#include "Function/function.hpp"

using namespace sf;


bool darkTheme = true;

Color colorBackground = Color::Black;
Color colorFunctionGraphic = Color::White;
Color colorAxis = Color(128, 128, 128);
Color colorGrid = Color(32, 32, 32);
Color colorArea = Color(165, 165, 165);

int GRAPHIC_WINDOW_WIDTH = 800;
int GRAPHIC_WINDOW_HEIGHT = 800;

const float SCALE_MAX = 200;
const float SCALE_MIN = 1;

const float DXY = 25;

size_t scale = 50;
size_t speed = 10;

const float I = 0;
size_t t = 0;

float A = I + cos(t);
float f = M_PI / 2;
float d = 0;

bool functionLimitation = false;
bool fillFunctionArea = false;

float ox = GRAPHIC_WINDOW_WIDTH / 2 / scale;
float oy = GRAPHIC_WINDOW_HEIGHT / 2 / scale;

VertexArray axisX(Lines, 2);
VertexArray axisY(Lines, 2);

Function function(A, f, d);

void EventHandler(Event& event, RenderWindow& window);

void SetAxisX();
void SetAxisY();

void CalculateSinWaveVertexesCountAndBias(int& sinWaveVertexes, int& bias);
void CalculateFunctionVertexes(VertexArray& function, const int& countVertexes, const int& bias);

void CalculateGridLinesCountX(int& gridLinesCountX);
void CalculateGridLinesCountY(int& gridLinesCountX);

void CalculateGridX(VertexArray& gridX, const int& gridLinesCountX);
void CalculateGridY(VertexArray& gridY, const int& gridLinesCountY);

void DrawFillingArea(VertexArray& function, const int& length, RenderWindow& window);

void InvertColor(Color& color);

int main(int argc, char** argv) {
    srand(time(nullptr));

    Clock* clock = new Clock;
    double deltaTime = 0;

    if (argc == 3) {
        GRAPHIC_WINDOW_WIDTH = atoi(argv[1]);
        GRAPHIC_WINDOW_HEIGHT = atoi(argv[2]);
    }

    RenderWindow graphicWindow(VideoMode(GRAPHIC_WINDOW_WIDTH, GRAPHIC_WINDOW_HEIGHT, 32), "Prosto sinusoida");

    while (graphicWindow.isOpen()) {
        deltaTime = clock->restart().asSeconds();
        Event event;
        while (graphicWindow.pollEvent(event)) {
            EventHandler(event, graphicWindow);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        graphicWindow.clear(colorBackground);


        SetAxisX();
        SetAxisY();


        int gridLinesCountX = 0;
        int gridLinesCountY = 0;

        CalculateGridLinesCountX(gridLinesCountX);
        CalculateGridLinesCountY(gridLinesCountY);

        VertexArray gridX(Lines, gridLinesCountX * 2);
        VertexArray gridY(Lines, gridLinesCountY * 2);

        CalculateGridX(gridX, gridLinesCountX);
        CalculateGridY(gridY, gridLinesCountY);


        A = I + cos(0.01 * t++);

        function.Set(A, f, d);

        int sinWaveVertexes = 0;
        int bias = 0;

        CalculateSinWaveVertexesCountAndBias(sinWaveVertexes, bias);
        VertexArray sinWave(LineStrip, sinWaveVertexes);
        CalculateFunctionVertexes(sinWave, sinWaveVertexes, bias);      


        DrawFillingArea(sinWave, sinWaveVertexes, graphicWindow);


        graphicWindow.draw(gridX);
        graphicWindow.draw(gridY);

        graphicWindow.draw(axisX);
        graphicWindow.draw(axisY);

        graphicWindow.draw(sinWave);

        graphicWindow.display();
    }

    return 0;
}

void EventHandler(Event& event, RenderWindow& window) {
    if (event.type == Event::Closed) {
        window.close();
    }

    if (event.type == Event::KeyPressed) {
        if (event.key.code == Keyboard::Left) { // Left arrow
            ox += DXY / scale;
        } else if (event.key.code == Keyboard::Right) { // Right arrow
            ox -= DXY / scale;
        } else if (event.key.code == Keyboard::Up) { // Up arrow
            oy += DXY / scale;
        } else if (event.key.code == Keyboard::Down) { // Down arrow
            oy -= DXY / scale;


        } else if (event.key.code == Keyboard::Equal && scale < SCALE_MAX) { // '=' ('+')
            scale++;
            ox -= ox / scale;
            oy -= oy / scale;
        } else if (event.key.code == Keyboard::Hyphen && scale > SCALE_MIN) { // '-' ('_')
            scale--;
            ox += ox / scale;
            oy += oy / scale;


        } else if (event.key.code == Keyboard::L) {
            functionLimitation = !functionLimitation;
        } else if (event.key.code == Keyboard::F) {
            fillFunctionArea = !fillFunctionArea;


        } else if (event.key.code == Keyboard::D) {
            darkTheme = !darkTheme;
            InvertColor(colorBackground);
            InvertColor(colorFunctionGraphic);
            InvertColor(colorAxis);
            InvertColor(colorGrid);
            if (
                colorArea != Color::Red &&
                colorArea != Color::Blue &&
                colorArea != Color::Green
            ) {
                InvertColor(colorArea);
            }

        } else if (event.key.code == Keyboard::R) {
            colorArea = Color::Red;
        } else if (event.key.code == Keyboard::G) {
            colorArea = Color::Green;
        } else if (event.key.code == Keyboard::B) {
            colorArea = Color::Blue;
        } else if (event.key.code == Keyboard::W) {
            if (darkTheme) {
                colorArea = Color(165, 165, 165);
            } else {
                colorArea = Color(91, 91, 91);
            }
        }
    }
}

void SetAxisX() {
    axisX[0].position = Vector2f(0, oy * scale);
    axisX[1].position = Vector2f(GRAPHIC_WINDOW_WIDTH, oy * scale);

    axisX[0].color = colorAxis;
    axisX[1].color = colorAxis;
}

void SetAxisY() {
    axisY[0].position = Vector2f(ox * scale, 0);
    axisY[1].position = Vector2f(ox * scale, GRAPHIC_WINDOW_HEIGHT);

    axisY[0].color = colorAxis;
    axisY[1].color = colorAxis;
}

void CalculateSinWaveVertexesCountAndBias(int& sinWaveVertexes, int& bias) {
    if (functionLimitation) {
        int minX = -d/f;
        int maxX = (6*M_PI - d)/f;

        for (int x = 0; x < GRAPHIC_WINDOW_WIDTH; x++) {
            float scaledX = static_cast<float>(x) / scale - ox;
            if (scaledX < minX) {
                bias++;
            }
            if (scaledX > minX && scaledX < maxX) {
                sinWaveVertexes++;
            }
                
        }
    } else {
        sinWaveVertexes = GRAPHIC_WINDOW_WIDTH;
    }
}

void CalculateFunctionVertexes(VertexArray& functionGraphic, const int& countVertexes, const int& bias) {
    for (int x = 0; x < countVertexes; x++) {
        float scaledX = static_cast<float>(x + bias) / scale - ox;
        float scaledY = scale * (oy + function(scaledX));
        functionGraphic[x].position = Vector2f(x + bias, scaledY);
        functionGraphic[x].color = colorFunctionGraphic;
    }
}

void CalculateGridLinesCountX(int& gridLinesCountX) {
    for (int y = 0; y < GRAPHIC_WINDOW_HEIGHT; y += scale) {
        gridLinesCountX++;
    }
}

void CalculateGridLinesCountY(int& gridLinesCountY) {
    for (int x = 0; x < GRAPHIC_WINDOW_WIDTH; x += scale) {
        gridLinesCountY++;
    }
}

void CalculateGridX(VertexArray& gridX, const int& gridLinesCountX) {
    for (int i = 0; i < gridLinesCountX * 2; i += 2) {
        float y = static_cast<float>(i) / 2;

        gridX[i].position = Vector2f(0, (y + oy - floor(oy)) * scale);
        gridX[i + 1].position = Vector2f(GRAPHIC_WINDOW_WIDTH, (y + oy - floor(oy)) * scale);

        gridX[i].color = colorGrid;
        gridX[i + 1].color = colorGrid;
    }
}

void CalculateGridY(VertexArray& gridY, const int& gridLinesCountY) {
    for (int i = 0; i < gridLinesCountY * 2; i += 2) {
        float x = static_cast<float>(i) / 2;

        gridY[i].position = Vector2f((x + ox - floor(ox)) * scale, 0);
        gridY[i + 1].position = Vector2f((x + ox - floor(ox)) * scale, GRAPHIC_WINDOW_HEIGHT);

        gridY[i].color = colorGrid;
        gridY[i + 1].color = colorGrid;
    }
}

void DrawFillingArea(VertexArray& function, const int& length, RenderWindow& window) {
    if (fillFunctionArea) {
        int index = 0;
        VertexArray fillingArea(Lines, length * 2);
        for (int i = 0; i < length * 2; i += 2) {
            fillingArea[i].position = Vector2f(function[index].position.x, oy * scale);
            fillingArea[i + 1].position = Vector2f(function[index].position.x, function[index].position.y);

            fillingArea[i].color = colorArea;
            fillingArea[i + 1].color = colorArea;

            index++;
        }
        window.draw(fillingArea);
    }
}


void InvertColor(Color& color) {
    color.r = 255 - color.r;
    color.g = 255 - color.g;
    color.b = 255 - color.b;
}
