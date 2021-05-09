#include <GL/glut.h>
#include <chrono>
#include <thread>
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include <vecmath/vecmath.h>
using namespace std;

#define DEBUG 1

#ifdef DEBUG
#  define LOG(x) cout << "[L" << __LINE__ << "] " << #x << ": " << x << endl
#  define LOGS(x) cout << "[L" << __LINE__ << "] " << x << endl
#else
#  define LOG(x)
#  define LOGS(x)
#endif


// Globals
constexpr float PI = M_PI;

// #################### Objects ####################
class DrawableObject {
public:
    DrawableObject(vector<Vector3f>&& _vecv, vector<Vector3f>&& _vecn, vector<vector<unsigned> >&& _vecf):
        vecv(_vecv), vecn(_vecn), vecf(_vecf) { }

    // This is the list of points (3D vectors)
    vector<Vector3f> vecv;

    // This is the list of normals (also 3D vectors)
    vector<Vector3f> vecn;

    // This is the list of faces (indices into vecv and vecn)
    vector<vector<unsigned> > vecf;

    GLuint display_list_id_;
};

constexpr int MAX_BUFFER_SIZE = 255;
vector<DrawableObject> draw_objects;
int drawing_obj_idx = 0;

// #################### Colors ####################
// Here are some colors you might use - feel free to add more
GLfloat diffColors[][4] = {
	{1.0, 0.0, 0.0, 1.0},
	{1.0, 0.0, 1.0, 1.0},
	{0.7, 0.0, 1.0, 1.0},
	{0.2, 0.0, 1.0, 1.0},
	{0.0, 1.0, 1.0, 1.0},
	{0.0, 1.0, 0.0, 1.0},
	{1.0, 1.0, 0.0, 1.0},
};
GLfloat* current_color = diffColors[0];
constexpr int color_count = sizeof(diffColors) / sizeof(*diffColors);
bool change_color = false;
int color_idx = 0;
int color_transition_step = 0;
constexpr int kMaxColorTransitionStep = 20;

// #################### Lights ####################
GLfloat Lt0pos[] = {1.0f, 1.0f, 5.0f, 1.0f};

// #################### Camera ####################
struct CameraAngle {
    float theta; // Angle between Oz and vector projected to Oxz.
    float alpha; // Angle between Oy and vector projected to Oyz.

    friend ostream& operator<<(ostream& os, const CameraAngle& angle);
};

ostream& operator<<(ostream& os, const CameraAngle& angle) {
    os << "<" << angle.theta << ", " << angle.alpha << ">";
    return os;
}

constexpr float kCameraAngleDelta = PI * 0.02;
constexpr float kCameraDistance = 5.0;
constexpr float kCameraDistanceSqr = kCameraDistance * kCameraDistance;
Vector3f camera_position(0.0, 0.0, kCameraDistance);
Vector3f camera_up(0.0, 1.0, 0.0);
float theta = 0.0; // Rotating around y-axis.
float alpha = PI / 2; // Rotating around x-axis.
bool rotating_camera = false;

// #################### Mouse ####################
constexpr float kMouseThetaDiff = 0.03;
constexpr float kMouseAlphaDiff = 0.01;
int mouse_x = -1;
int mouse_y = -1;

// #################### Time ####################
int world_clock = 0;
constexpr int kTargetFPS = 60;
constexpr int kTargetFrameTime = static_cast<int>(1000.0 / kTargetFPS);

// You will need more global variables to implement color and position changes


// These are convenience functions which allow us to call OpenGL
// methods on Vec3d objects
inline void glVertex(const Vector3f &a)
{ glVertex3fv(a); }

inline void glNormal(const Vector3f &a)
{ glNormal3fv(a); }


void updateCamera(int);
void updateColor(int);
void drawScene(void);
void RotateCamera(float, float);
CameraAngle ComputeAngle(const Vector3f&);

void setUpdateCameraTimer() {
	glutTimerFunc(16 /* ms */, [](int value) {
        updateCamera(value);
        setUpdateCameraTimer();
    }, 0);
}

void updateCamera(int value) {
    if (!rotating_camera) return;
    RotateCamera(kCameraAngleDelta, kCameraAngleDelta);
    // drawScene();
}

void setUpdateColorTimer() {
	glutTimerFunc(100 /* ms */, [](int value) {
        updateColor(value);
        setUpdateColorTimer();
    }, 0);
}

void updateColor(int value) {
	if (!change_color) return;
	auto next_color_idx = (color_idx + 1) % color_count;
	color_transition_step++;
	if (color_transition_step >= kMaxColorTransitionStep) {
		color_idx = next_color_idx;
		color_transition_step = 0;
		next_color_idx = (next_color_idx + 1) % color_count;
	}
	const auto& c1 = diffColors[color_idx];
	const auto& c2 = diffColors[next_color_idx];
	current_color[0] = c1[0] + (c2[0] - c1[0]) * color_transition_step / kMaxColorTransitionStep;
	current_color[1] = c1[1] + (c2[1] - c1[1]) * color_transition_step / kMaxColorTransitionStep;
	current_color[2] = c1[2] + (c2[2] - c1[2]) * color_transition_step / kMaxColorTransitionStep;
	// drawScene();
}

CameraAngle ComputeAngle(const Vector3f& pos) {
    // Project to xz plane.
    auto theta = PI / 2;
    if (abs(pos.x()) > 1e-3 || abs(pos.z()) > 1e-3) {
        theta = acos(pos.z() / pos.xz().abs());
        const auto cross_xz = Vector3f::cross(pos, -Vector3f::FORWARD);
        if (Vector3f::dot(Vector3f::UP, cross_xz) > 0) {
            theta = -theta;
        }
    }

    // Project to yz plane.
    auto alpha = PI / 2;
    if (abs(pos.y()) > 1e-3 || abs(pos.z()) > 1e-3) {
        alpha = acos(pos.y() / pos.yz().abs());
        const auto cross_yz = Vector3f::cross(pos, Vector3f::UP);
        if (Vector3f::dot(Vector3f::RIGHT, cross_yz) > 0) {
            alpha = -alpha;
        }
    }

    return CameraAngle{theta, alpha};
}

void UpdateCameraUp() {
    auto last_camera_up = camera_up;
    camera_up = Vector3f::cross(camera_position, Vector3f::RIGHT);
    if (Vector3f::dot(camera_up, last_camera_up) < 0) {
        camera_up = -camera_up;
    }
    camera_up.normalize();
    LOGS("====");
    LOG(camera_up);
}

void RotateCamera(float theta_diff, float alpha_diff) {
    // Rotate around y-axis.
    auto camera_angle = ComputeAngle(camera_position);
    camera_angle.theta += theta_diff;
    const auto y = camera_position.y();
    const auto radius_at_y = sqrt(kCameraDistanceSqr - y * y);
    camera_position.z() = radius_at_y * cos(camera_angle.theta);
    camera_position.x() = radius_at_y * sin(camera_angle.theta);

    // Rotate around x-axis.
    if (camera_position.yz().absSquared() > 1e-6) {
        camera_angle = ComputeAngle(camera_position); // Recompute camera angle.
        camera_angle.alpha -= alpha_diff;
        const auto x = camera_position.x();
        const auto radius_at_x = sqrt(kCameraDistanceSqr - x * x);
        camera_position.y() = radius_at_x * cos(camera_angle.alpha);
        camera_position.z() = radius_at_x * sin(camera_angle.alpha);
    }

    UpdateCameraUp();
}

void mouseMotionFunc(int x, int y) {
    int dx = x - mouse_x;
    int dy = y - mouse_y;
    // LOG(dx);
    mouse_x = x;
    mouse_y = y;
    float theta_diff = -dx * kMouseThetaDiff / PI;
    float alpha_diff = dy * kMouseAlphaDiff / PI;
    RotateCamera(theta_diff, alpha_diff);
    // drawScene();
}

void mouseFunc(int button, int state, int x, int y) {
    switch (button) {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN) {
            mouse_x = x;
            mouse_y = y;
        }
        break;

    case GLUT_MIDDLE_BUTTON:
        break;

    case GLUT_RIGHT_BUTTON:
        break;
    }
}

// This function is called whenever a "Normal" key press is received.
void keyboardFunc( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case 27: // Escape key
        exit(0);
        break;
    case 'c':
        // add code to change color here
		change_color = !change_color;
        break;
    case 'd':
        // Next drawable object.
        drawing_obj_idx = (drawing_obj_idx + 1) % draw_objects.size();
        break;
    case 'r':
        // Rotate.
        rotating_camera = !rotating_camera;
        break;
    default:
        cout << "Unhandled key press " << key << "." << endl;
    }

	// this will refresh the screen so that the user sees the color change
    glutPostRedisplay();
}

// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc( int key, int x, int y )
{
    switch ( key )
    {
    case GLUT_KEY_UP:
        // add code to change light position
        Lt0pos[1] += 0.5f;
		break;
    case GLUT_KEY_DOWN:
        // add code to change light position
        Lt0pos[1] -= 0.5f;
		break;
    case GLUT_KEY_LEFT:
        // add code to change light position
        Lt0pos[0] -= 0.5f;
		break;
    case GLUT_KEY_RIGHT:
        // add code to change light position
        Lt0pos[0] += 0.5f;
		break;
    }

	// this will refresh the screen so that the user sees the light position
    glutPostRedisplay();
}

void drawObject() {
    // draw the display list
    glCallList(draw_objects[drawing_obj_idx].display_list_id_);
}

// This function is responsible for displaying the object.
void drawScene(void)
{
    // Clear the rendering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rotate the image
    glMatrixMode( GL_MODELVIEW );  // Current matrix affects objects positions
    glLoadIdentity();              // Initialize to the identity

    // Position the camera at [0,0,5], looking at [0,0,0],
    // with [0,1,0] as the up direction.
    gluLookAt(camera_position.x(), camera_position.y(), camera_position.z(),
              0.0, 0.0, 0.0,
              camera_up.x(), camera_up.y(), camera_up.z());

    // Set material properties of object

	// Here we use the first color entry as the diffuse color
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, current_color);

	// Define specular color and shininess
    GLfloat specColor[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shininess[] = {100.0};

	// Note that the specular color and shininess can stay constant
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    // Set light properties

    // Light color (RGBA)
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
    // Light position

    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

	// This GLUT method draws a teapot.  You should replace
	// it with code which draws the object you loaded.
	// glutSolidTeapot(1.0);
    drawObject();

    // Dump the image to the screen.
    glutSwapBuffers();

}

// Initialize OpenGL's rendering modes
void initRendering()
{
    glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
    glEnable(GL_LIGHTING);     // Enable lighting calculations
    glEnable(GL_LIGHT0);       // Turn on light #0.
    world_clock = glutGet(GLUT_ELAPSED_TIME);
}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h)
{
    // Always use the largest square viewport possible
    if (w > h) {
        glViewport((w - h) / 2, 0, h, h);
    } else {
        glViewport(0, (h - w) / 2, w, w);
    }

    // Set up a perspective view, with square aspect ratio
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 50 degree fov, uniform aspect ratio, near = 1, far = 100
    gluPerspective(50.0, 1.0, 1.0, 100.0);
}

void AddObjectsToDisplayList() {
    // create display list
    GLuint index = glGenLists(draw_objects.size());
    if (!index) {
        cout << "ERROR: failed to init display list!" << endl;
        return;
    }

    int i = 0;
    for (auto& obj : draw_objects) {
        const auto& vecf = obj.vecf;
        const auto& vecv = obj.vecv;
        const auto& vecn = obj.vecn;

        // compile the display list
        glNewList(index + i, GL_COMPILE);
        obj.display_list_id_= index + i;
        i++;

        glBegin(GL_TRIANGLES);
        for (const auto& face : vecf) {
            auto a = face[0];
            auto b = face[1];
            auto c = face[2];
            auto d = face[3];
            auto e = face[4];
            auto f = face[5];
            auto g = face[6];
            auto h = face[7];
            auto i = face[8];
            glNormal3d(vecn[c-1][0], vecn[c-1][1], vecn[c-1][2]);
            glVertex3d(vecv[a-1][0], vecv[a-1][1], vecv[a-1][2]);
            glNormal3d(vecn[f-1][0], vecn[f-1][1], vecn[f-1][2]);
            glVertex3d(vecv[d-1][0], vecv[d-1][1], vecv[d-1][2]);
            glNormal3d(vecn[i-1][0], vecn[i-1][1], vecn[i-1][2]);
            glVertex3d(vecv[g-1][0], vecv[g-1][1], vecv[g-1][2]);
        }
        glEnd();

        // End list.
        glEndList();
    }
}

void loadInput()
{
	// load the OBJ file here
    char buffer[MAX_BUFFER_SIZE];

    // Store latest obj.
    vector<Vector3f> vecv;
    vector<Vector3f> vecn;
    vector<vector<unsigned> > vecf;

    while (cin.getline(buffer, MAX_BUFFER_SIZE)) {
        stringstream ss(buffer);
        string token = "";
        ss >> token;

        if (token == "v") {
            float v0, v1, v2;
            ss >> v0 >> v1 >> v2;
            vecv.emplace_back(v0, v1, v2);
        } else if (token == "vn") {
            float v0, v1, v2;
            ss >> v0 >> v1 >> v2;
            vecn.emplace_back(v0, v1, v2);
        } else if (token == "f") {
            string line = "";
            std::getline(ss, line);
            unsigned int a, b, c, d, e, f, g, h, i;
            sscanf(line.c_str(),
                    " %u/%u/%u %u/%u/%u %u/%u/%u",
                    &a, &b, &c, &d, &e, &f, &g, &h, &i);
            // cout << line << " " << a << " " << i << endl;
            vecf.push_back({a, b, c, d, e, f, g, h, i});
        } else if (token == "#") {
            // Start of file
            if (!vecf.empty()) {
                draw_objects.emplace_back(std::move(vecv), std::move(vecn), std::move(vecf));
                vecv.clear();
                vecf.clear();
                vecn.clear();
            }
        }
    }
    if (!vecf.empty()) {
        draw_objects.emplace_back(std::move(vecv), std::move(vecn), std::move(vecf));
        vecv.clear();
        vecf.clear();
        vecn.clear();
    }
}

void idleFunc() {
    drawScene();
    auto current_time = glutGet(GLUT_ELAPSED_TIME);
    auto expected_time = world_clock + kTargetFrameTime;
    if (current_time < expected_time) {
        std::this_thread::sleep_for(std::chrono::milliseconds(expected_time - current_time));
    }
    world_clock = current_time;
}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char** argv )
{
    loadInput();

    glutInit(&argc,argv);

    // We're going to animate it, so double buffer
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 600, 260 );
    glutInitWindowSize( 720, 720 );
    glutCreateWindow("Assignment 0");

    // Initialize OpenGL parameters.
    initRendering();

    // Add drawable objects to display list once.
    AddObjectsToDisplayList();

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

    // Set up callback functions for mouse presses.
    glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseMotionFunc);

     // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

	// Callback for color transitioning.
	setUpdateColorTimer();

	// Callback for camera rotation.
	setUpdateCameraTimer();

    glutIdleFunc(idleFunc);

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop( );

    return 0;	// This line is never reached.
}
