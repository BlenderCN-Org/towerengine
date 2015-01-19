
#ifndef _MESHOBJECT_H
#define _MESHOBJECT_H


class tMeshObject : public tTransformObject
{
	private:
		tMesh *mesh;

		bool visible;

		bool animation_mode;
		bool loop;
		tAnimation *animation;
		float time;

		char *pose;

		tVector color;
		float alpha;

		float *transform_matrix;

		btRigidBody *rigid_body;
		btMotionState *motion_state;

	protected:
		void TransformChanged(void);

	public:
		tMeshObject(tMesh *mesh, float mass = 0.0);

		void SetAnimation(const char *animation);
		void Play(float time);
		bool GetAnimationFinished(void);
		void SetAnimationLoop(bool l)					{ loop = l; }
		void SetAnimationMode(bool a)					{ animation_mode = a; }
		void SetAnimationTime(float t)					{ time = t; }
		void SetPose(const char *pose);
		void SetColor(tVector c)						{ color = c; }
		void SetAlpha(float a)							{ alpha = a; }
		void SetColor(tVector c, float a)				{ SetColor(c); SetAlpha(a); }
		void SetVisible(bool visible)					{ this->visible = visible; }

		void UpdateRigidBodyTransformation(void);

		void GeometryPass(void);
		void ForwardPass(void);
		tBoundingBox GetBoundingBox(void);

		void AddedToWorld(tWorld *world);
		void RemovedFromWorld(tWorld *world);

		btRigidBody *GetRigidBody(void)		{ return rigid_body; }

		void SetTransformWithoutRigidBody(tTransform transform);
};

class tMeshObjectMotionState : public btMotionState
{
	private:
		tMeshObject *object;

	public:
		tMeshObjectMotionState(tMeshObject *object);

		virtual void getWorldTransform(btTransform &trans) const;
		virtual void setWorldTransform(const btTransform &trans);
};

#endif
