
#include "towerengine.h"


CMeshObject::CMeshObject(CMesh *mesh)
{
	this->mesh = mesh;
	animation = 0;
	pose = cstr("Idle");
	animation_mode = 0;
	loop = false;
	pos = Vec(0.0, 0.0, 0.0);
	rot = Vec(0.0, 0.0, 0.0);
	x = Vec(1.0, 0.0, 0.0);
	y = Vec(0.0, 1.0, 0.0);
	z = Vec(0.0, 0.0, 1.0);
	scale = Vec(1.0, 1.0, 1.0);
	transformation = new CTransformationMatrix();
	color = Vec(1.0, 1.0, 1.0);
	alpha = 1.0;
	visible = true;
	time = 0.0;
	motion_state = new btDefaultMotionState(btTransform(btQuaternion(0.0, 0.0, 0.0, 1), btVector3(0.0, 0.0, 0.0)));
	rigid_body = new btRigidBody(0.0, motion_state, mesh->GetPhysicsMeshShape(), btVector3(0.0, 0.0, 0.0));
	rigid_body->setRestitution(0.0);
}

void CMeshObject::SetAnimation(const char *animation)
{
	CAnimation *a = mesh->GetAnimationByName(animation);
	if(a != this->animation)
		time = 0.0;
	this->animation = a;
}

void CMeshObject::Fade(float fade_end, float time)
{
	this->fade_end = fade_end;
	fade_speed = (fade_end - alpha) / time;
}

void CMeshObject::Play(float time)
{
	if(fade_speed != 0.0)
	{
		alpha += fade_speed * time;

		if((fade_speed > 0.0 && alpha >= fade_end)
				|| (fade_speed < 0.0 && alpha <= fade_end))
		{
			fade_speed = 0.0;
			alpha = fade_end;
		}
	}

	if(!animation_mode || !animation)
		return;

	if(!loop)
		this->time = min(animation->GetLength(), this->time + time);
	else
		this->time = fmod(this->time + time, animation->GetLength());
}

bool CMeshObject::GetAnimationFinished(void)
{
	if(!animation_mode || !animation)
		return true;

	return !loop && this->time >= animation->GetLength();
}

void CMeshObject::SetPose(const char *pose)
{
	if(this->pose)
		delete [] this->pose;
	this->pose = cstr(pose);
}

CBoundingBox CMeshObject::GetBoundingBox(void)
{
	CBoundingBox b;
	CVector *p = mesh->GetBoundingBox().GetCornerPoints();
	float *mat;

	transformation->LoadIdentity();
	transformation->Translate(pos);
	transformation->Rotate(rot);
	transformation->SetXYZ(x, y, z);
	transformation->Scale(scale);

	mat = transformation->GetMatrix();

	btTransform trans(btMatrix3x3(mat[0], mat[1], mat[2], mat[4], mat[5], mat[6], mat[8], mat[9], mat[10]), btVector3(mat[3], mat[7], mat[11]));
	rigid_body->setWorldTransform(trans);

	for(int i=0; i<8; i++)
		b.AddPoint(ApplyMatrix4(mat, p[i]));

	return b;
}

void CMeshObject::PutToGL(void)
{
	if(!visible || alpha <= 0.0)
		return;

	transformation->LoadIdentity();
	transformation->Translate(pos);
	transformation->Rotate(rot);
	transformation->SetXYZ(x, y, z);
	transformation->Scale(scale);

	CEngine::GetCurrentFaceShader()->SetTransformation(transformation->GetMatrix());

	CMesh::Color(color, alpha);

	if(animation_mode && animation)
	{
		mesh->ChangeAnimation(animation);
		mesh->SetAnimationLoop(0);
		animation->SetTime(time);
	}
	else
	{
		if(pose)
			mesh->ChangePose(pose);
		else
			mesh->ChangePose("Idle");
	}
	mesh->PutToGL();
}
