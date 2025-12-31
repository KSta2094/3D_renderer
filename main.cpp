#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <locale>
#include <optional>
#include <type_traits>
#include <vector>
#define WIDTH 50
#define HEIGHT 50
#define FRAMES 1.0 / 30.0
#define PI 3.141592653589793

struct Vector2 {
    float x, y = 0;

    bool visalbe = true;
    Vector2(float x, float y, bool visalbe) : x(x), y(y), visalbe(visalbe) {};
};

struct Vector3 {
    float x, y, z;

    Vector3(float x, float y, float z) : x(x), y(y), z(z) {};

    Vector3 operator+(Vector3 vec) {
        return Vector3{x + vec.x, y + vec.y, z + vec.z};
    };
};

float dist(Vector2 a, Vector2 b) {
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}
bool is_point_on_screen(Vector2 a) {

    if (a.x > 0 && a.x < WIDTH && a.y > 0 && a.y < HEIGHT) {
        return true;
    }
    return false;
}

Vector2 project2D(Vector3 p) {
    float FOV = (PI / 2.0);
    float f = 1.0f / std::tan(FOV / 2.0);

    if (p.z <= 0) {

        return Vector2{((p.x / p.z)) * f, ((p.y / p.z)) * f, false};
    };
    return Vector2{((p.x / p.z)) * f, ((p.y / p.z)) * f, true};
}
void drawLine(Vector2 a, Vector2 b, std::vector<char> *display) {

    if (!(a.visalbe && b.visalbe)) {
        return;
    }
    float dx = b.x - a.x;
    float dy = b.y - a.y;

    int steps = (int)std::max(std::abs(dx), std::abs(dy));

    float xInc = dx / steps;
    float yInc = dy / steps;

    float x = a.x;
    float y = a.y;

    for (int i = 0; i < steps; i++) {
        int ix = (int)std::round(x);
        int iy = (int)std::round(y);

        if (is_point_on_screen(Vector2{static_cast<float>(ix),
                                       static_cast<float>(iy), true})) {
            (*display)[iy * WIDTH + ix] = '.';
        }
        if (is_point_on_screen(Vector2{static_cast<float>(ix),
                                       static_cast<float>(iy), true}) &&
            (i == 0 || i == steps)) {
            (*display)[iy * WIDTH + ix] = 'X';
        }
        x += xInc;
        y += yInc;
    }
}

Vector2 drawPoint(Vector3 p, std::vector<char> *buffer) {
    std::optional<Vector2> new_p = project2D(p);
    Vector2 screen{0, 0, new_p->visalbe};
    screen.x = static_cast<int>(((new_p->x + 1) / 2.0) * WIDTH - 1);
    screen.y = static_cast<int>((1 - ((new_p->y + 1) / 2.0)) * HEIGHT);

    return screen;
}

void connectPolygon(std::vector<Vector2> vertexes, std::vector<char> *buffer) {
    for (std::optional<Vector2> vertex : vertexes) {
        if (vertex->visalbe) {
            for (int i{0}; i < vertexes.size() - 1; i++) {
                drawLine(vertex.value(), vertexes[i], buffer);
            }
        }
    }
}

void clear(std::vector<char> *buffer) {

    std::fill((*buffer).begin(), buffer->end(), ' ');
    printf("\033[H\033[J");
}

void bufferDraw(std::vector<char> buffer) {
    for (int i{0}; i < WIDTH * HEIGHT; i++) {

        printf("%c", buffer[i]);
        if (i % WIDTH == 0) {
            printf("\n");
        }
    }
}

void rotatex(std::vector<std::optional<Vector3>> *vertexes, float angle) {

    float c_z = 3;
    float c_y = 0;

    for (int i{0}; i < (*vertexes).size(); i++) {

        Vector3 tmp(0, 0, 0);

        tmp.y = ((*vertexes)[i]->y - c_y) * cos(angle) -
                ((*vertexes)[i]->z - c_z) * sin(angle) + c_y;

        tmp.z = ((*vertexes)[i]->y - c_y) * sin(angle) +
                ((*vertexes)[i]->z - c_z) * cos(angle) + c_z;
        (*vertexes)[i]->y = tmp.y;
        (*vertexes)[i]->z = tmp.z;
    }
}

void rotatey(std::vector<std::optional<Vector3>> *vertexes, float angle) {

    float c_x = 0;
    float c_z = 3;

    for (int i{0}; i < (*vertexes).size(); i++) {
        Vector3 tmp(0, 0, 0);
        // x_rot = c_x + (x - c_x) cos(θ) - (y - c_z) sin(θ);
        // z_rot = c_z - (x - c_x) sin(θ) + (z - c_z) cos(θ);

        tmp.x = c_x + ((*vertexes)[i]->x - c_x) * cos(angle) +
                ((*vertexes)[i]->z - c_z) * sin(angle);

        tmp.z = c_z - ((*vertexes)[i]->x - c_x) * sin(angle) +
                ((*vertexes)[i]->z - c_z) * cos(angle);
        (*vertexes)[i]->x = tmp.x;
        (*vertexes)[i]->z = tmp.z;
    }
}

std::vector<Vector2> plot3D(std::vector<std::optional<Vector3>> vertexes3D,
                            std::vector<char> *buffer) {

    std::vector<Vector2> screenPoints;

    for (std::optional<Vector3> vertex : vertexes3D) {
        auto tmp = drawPoint(vertex.value(), buffer);
        // if (is_point_on_screen(tmp)) {
        screenPoints.push_back(tmp);
        //}
    }
    return screenPoints;
}

Vector3 get_input() {

    Vector3 out(0, 0, 0);

    system("stty raw");
    char input = getchar();

    system("stty cooked");
    switch (tolower(input, std::locale())) {
    case 'a':
        out.x += 0.3;
        break;
    case 'd':
        out.x -= 0.3;
        break;
    case 'w':
        out.z -= 0.3;
        break;
    case 's':
        out.z += 0.3;
        break;
    case 'q':
        system("stty cooked");
        exit(1);
    default:
        return Vector3(0, 0, 0);
    }
    return out;
}
void move_vertexes(std::vector<std::optional<Vector3>> *vertexes,
                   Vector3 offset) {
    for (int i{0}; i < vertexes->size(); i++) {
        (*vertexes)[i] = (*vertexes)[i].value() + offset;
    }
}
int main() {

    auto last_time = std::chrono::steady_clock::now();
    bool running = true;
    std::vector<char> buffer(WIDTH * HEIGHT, ' ');
    std::vector<Vector2> vertexes2D;

    std::vector<std::optional<Vector3>> vertexes3D = {

        // front face
        Vector3{-1, 1, 2},
        Vector3{1, 1, 2},
        Vector3{-1, -1, 2},
        Vector3{1, -1, 2},

        // back face
        Vector3{-1, 1, 4},
        Vector3{1, 1, 4},
        Vector3{-1, -1, 4},
        Vector3{1, -1, 4},

    };

    Vector3 input(0, 0, 0);

    while (running) {

        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<float> delta = now - last_time;
        float dt = delta.count();
        if (dt < FRAMES) {
            continue;
        }
        // clear screen
        move_vertexes(&vertexes3D, input);
        vertexes2D = plot3D(vertexes3D, &buffer);
        connectPolygon(vertexes2D, &buffer);

        bufferDraw(buffer);

        last_time = now;
        input = get_input();

        clear(&buffer);
    }

    return 1;
}
