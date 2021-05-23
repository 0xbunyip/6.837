#include "surf.h"
#include "extra.h"
#include "util.h"
using namespace std;

namespace
{

    // We're only implenting swept surfaces where the profile curve is
    // flat on the xy-plane.  This is a check function.
    static bool checkFlat(const Curve &profile)
    {
        for (unsigned i=0; i<profile.size(); i++)
            if (profile[i].V[2] != 0.0 ||
                profile[i].T[2] != 0.0 ||
                profile[i].N[2] != 0.0)
                return false;

        return true;
    }
}

void extendSurface(Surface& surface, const Curve& profile,
                   const Vector4f& N, const Vector4f& B,
                   const Vector4f& T, const Vector4f& V) {
    Matrix4f R(N, B, T, V, true);
    for (int j = 0; j < profile.size(); ++j) {
        auto v = R * Vector4f(profile[j].V, 1);
        auto n = -(R * Vector4f(profile[j].N, 0));
        surface.VV.push_back(v.xyz());
        surface.VN.push_back(n.xyz().normalized());

        int D = surface.VV.size() - 1;
        int C = D - 1;
        int B = D - profile.size();
        int A = B - 1;
        if (A < 0) continue;

        surface.VF.push_back(Tup3u(A, D, B));
        surface.VF.push_back(Tup3u(A, C, D));
    }
}

Surface makeSurfRev(const Curve &profile, unsigned steps)
{
    Surface surface;

    if (!checkFlat(profile))
    {
        cerr << "surfRev profile curve must be flat on xy plane." << endl;
        for (unsigned i=0; i<profile.size(); i++)
            LOG(i, profile[i].V, profile[i].T, profile[i].N);
        exit(0);
    }

    LOG(steps);
    for (int i = 0; i <= steps; ++i) {
        float theta = -i * 2 * M_PI / steps;

        Vector4f N(cos(theta), 0, -sin(theta), 0);
        Vector4f B(0, 1, 0, 0);
        Vector4f T(sin(theta), 0, cos(theta), 0);
        Vector4f V(0, 0, 0, 1);
        extendSurface(surface, profile, N, B, T, V);
    }

    // cerr << "\t>>> makeSurfRev called (but not implemented).\n\t>>> Returning empty surface." << endl;

    return surface;
}

Surface makeGenCyl(const Curve &profile, const Curve &sweep )
{
    Surface surface;

    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    LOG(sweep.size());
    for (int i = 0; i < sweep.size(); i += 1) {
        extendSurface(surface, profile,
                      Vector4f(sweep[i].B, 0),
                      Vector4f(sweep[i].N, 0),
                      Vector4f(sweep[i].T, 0),
                      Vector4f(sweep[i].V, 1));

        // LOG(sweep[0].V, sweep[0].N);
        // LOG(surface.VV[0], surface.VN[0]);
    }

    // cerr << "\t>>> makeGenCyl called (but not implemented).\n\t>>> Returning empty surface." <<endl;

    return surface;
}

void drawSurface(const Surface &surface, bool shaded)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (shaded)
    {
        // This will use the current material color and light
        // positions.  Just set these in drawScene();
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // This tells openGL to *not* draw backwards-facing triangles.
        // This is more efficient, and in addition it will help you
        // make sure that your triangles are drawn in the right order.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else
    {
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glColor4f(0.4f,0.4f,0.4f,1.f);
        glLineWidth(1);
    }

    glBegin(GL_TRIANGLES);
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        glNormal(surface.VN[surface.VF[i][0]]);
        glVertex(surface.VV[surface.VF[i][0]]);
        glNormal(surface.VN[surface.VF[i][1]]);
        glVertex(surface.VV[surface.VF[i][1]]);
        glNormal(surface.VN[surface.VF[i][2]]);
        glVertex(surface.VV[surface.VF[i][2]]);
    }
    glEnd();

    glPopAttrib();
}

void drawNormals(const Surface &surface, float len)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_LIGHTING);
    glColor4f(0,1,1,1);
    glLineWidth(1);

    glBegin(GL_LINES);
    for (unsigned i=0; i<surface.VV.size(); i++)
    {
        glVertex(surface.VV[i]);
        glVertex(surface.VV[i] + surface.VN[i] * len);
    }
    glEnd();

    glPopAttrib();
}

void outputObjFile(ostream &out, const Surface &surface)
{

    for (unsigned i=0; i<surface.VV.size(); i++)
        out << "v  "
            << surface.VV[i][0] << " "
            << surface.VV[i][1] << " "
            << surface.VV[i][2] << endl;

    for (unsigned i=0; i<surface.VN.size(); i++)
        out << "vn "
            << surface.VN[i][0] << " "
            << surface.VN[i][1] << " "
            << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;

    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j=0; j<3; j++)
        {
            unsigned a = surface.VF[i][j]+1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }
}
