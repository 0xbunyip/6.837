#include "SkeletalModel.h"
#include "util.h"

#include <FL/Fl.H>

using namespace std;

void SkeletalModel::load(const char *skeletonFile, const char *meshFile, const char *attachmentsFile)
{
	loadSkeleton(skeletonFile);

	m_mesh.load(meshFile);
	m_mesh.loadAttachments(attachmentsFile, m_joints.size());

	computeBindWorldToJointTransforms();
	updateCurrentJointToWorldTransforms();
}

void SkeletalModel::draw(Matrix4f cameraMatrix, bool skeletonVisible)
{
	// draw() gets called whenever a redraw is required
	// (after an update() occurs, when the camera moves, the window is resized, etc)

	m_matrixStack.clear();
	m_matrixStack.push(cameraMatrix);

	if( skeletonVisible )
	{
		drawJoints();

		drawSkeleton();
	}
	else
	{
		// Clear out any weird matrix we may have been using for drawing the bones and revert to the camera matrix.
		glLoadMatrixf(m_matrixStack.top());

		// Tell the mesh to draw itself.
		m_mesh.draw();
	}
}

void SkeletalModel::loadSkeleton(const char *filename) {
  // Load the skeleton from file here.
  ifstream in(filename);
  if (!in) {
    cerr << filename << " not found\a" << endl;
    exit(0);
  }

  // Joint* j1 = new Joint;
  // j1->transform = Matrix4f::translation(Vector3f(0, 0, 0));
  // m_joints.push_back(j1);
  // m_rootJoint = j1;

  // Joint *j2 = new Joint;
  // j2->transform = Matrix4f::translation(Vector3f(1, 0, 0));
  // m_joints.push_back(j2);
  // m_joints[0]->children.push_back(j2);

  // Joint *j3 = new Joint;
  // j3->transform = Matrix4f::translation(Vector3f(0, 1, 0));
  // m_joints.push_back(j3);
  // m_joints[0]->children.push_back(j3);
  // return;

  float x, y, z;
  int par = 0;
  while (in >> x >> y >> z >> par) {
    Joint* joint = new Joint;
    m_joints.push_back(joint);

    joint->transform = Matrix4f::identity();
    joint->transform.setCol(3, Vector4f(x, y, z, 1));

    if (par == -1) {
      m_rootJoint = joint;
    } else {
      m_joints[par]->children.push_back(joint);
    }
  }
}

void SkeletalModel::visitAndDrawJoint(const Joint* joint) {
  m_matrixStack.push(joint->transform);
  glutSolidSphere(0.025f, 12, 12);

  for (const auto& child : joint->children) {
    visitAndDrawJoint(child);
  }
  m_matrixStack.pop();
}

void SkeletalModel::drawJoints() {
  // Draw a sphere at each joint. You will need to add a recursive helper
  // function to traverse the joint hierarchy.
  //
  // We recommend using glutSolidSphere( 0.025f, 12, 12 )
  // to draw a sphere of reasonable size.
  //
  // You are *not* permitted to use the OpenGL matrix stack commands
  // (glPushMatrix, glPopMatrix, glMultMatrix).
  // You should use your MatrixStack class
  // and use glLoadMatrix() before your drawing call.
  visitAndDrawJoint(m_rootJoint);
}

void SkeletalModel::visitAndDrawBone(const Joint *joint, const Joint* par) {
  if (par != nullptr) {
    const auto translate = Matrix4f::translation(0, 0, 0.5);

    const auto eye = joint->transform.getCol(3).xyz();
    const float l = eye.abs();
    const auto scale = Matrix4f::scaling(0.010 / 0.5, 0.010 / 0.5, l);

    const auto origin = par->transform.getCol(3).xyz();
    const auto b3 = eye.normalized(); // z
    const auto b2 = Vector3f::cross(b3, Vector3f(0, 0, 1)).normalized(); // y
    const auto b1 = Vector3f::cross(b2, b3).normalized(); // x

    const Matrix4f transform = Matrix4f(
        Vector3f::dot(b1, Vector3f::RIGHT), Vector3f::dot(b2, Vector3f::RIGHT),
        Vector3f::dot(b3, Vector3f::RIGHT), 0, Vector3f::dot(b1, Vector3f::UP),
        Vector3f::dot(b2, Vector3f::UP), Vector3f::dot(b3, Vector3f::UP), 0,
        Vector3f::dot(b1, -Vector3f::FORWARD),
        Vector3f::dot(b2, -Vector3f::FORWARD),
        Vector3f::dot(b3, -Vector3f::FORWARD), 0, origin.x(), origin.y(),
        origin.z(), 1);

    const auto mat = transform * scale * translate;
    // LOG(eye, l, z, up);
    LOG(b1, b2, b3);
    LOG(transform);
    // LOG(mat);

    m_matrixStack.push(mat);
    glutSolidCube(1.0);
    m_matrixStack.pop();
  }
  m_matrixStack.push(joint->transform);
  for (const auto& child : joint->children) {
    visitAndDrawBone(child, joint);
  }
  m_matrixStack.pop();
}

void SkeletalModel::drawSkeleton() {
  // Draw boxes between the joints. You will need to add a recursive helper
  // function to traverse the joint hierarchy.
  visitAndDrawBone(m_rootJoint);
}

void SkeletalModel::setJointTransform(int jointIndex, float rX, float rY, float rZ)
{
	// Set the rotation part of the joint's transformation matrix based on the passed in Euler angles.
}


void SkeletalModel::computeBindWorldToJointTransforms()
{
	// 2.3.1. Implement this method to compute a per-joint transform from
	// world-space to joint space in the BIND POSE.
	//
	// Note that this needs to be computed only once since there is only
	// a single bind pose.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
}

void SkeletalModel::updateCurrentJointToWorldTransforms()
{
	// 2.3.2. Implement this method to compute a per-joint transform from
	// joint space to world space in the CURRENT POSE.
	//
	// The current pose is defined by the rotations you've applied to the
	// joints and hence needs to be *updated* every time the joint angles change.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
}

void SkeletalModel::updateMesh()
{
	// 2.3.2. This is the core of SSD.
	// Implement this method to update the vertices of the mesh
	// given the current state of the skeleton.
	// You will need both the bind pose world --> joint transforms.
	// and the current joint --> world transforms.
}

