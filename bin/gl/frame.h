
import carray.h

define M3DVector3f carray<float,3>
define M3DVector4f carray<float,4>
define M3DMatrix44f carray<float,16>

class GLFrame
{
	M3DVector3f vOrigin
	M3DVector3f vForward
	M3DVector3f vUp
	
	GLFrame()
	{
		vOrigin[0]=0.0f
		vOrigin[1]=0.0f
		vOrigin[2]=0.0f
		vUp[0]=0.0f
		vUp[1]=1.0f
		vUp[2]=0.0f
		vForward[0]=0.0f
		vForward[1]=0.0f
		vForward[2]=-1.0f
	}
	
	void MoveForward(float fDelta)
	{
		vOrigin[0]+=vForward[0]*fDelta
		vOrigin[1]+=vForward[1]*fDelta
		vOrigin[2]+=vForward[2]*fDelta
	}
	
	void MoveRight(float fDelta)
	{
		M3DVector3f vCross
		m3dCrossProduct(vCross,vUp,vForward)
	
		vOrigin[0]+=vCross[0]*fDelta
		vOrigin[1]+=vCross[1]*fDelta
		vOrigin[2]+=vCross[2]*fDelta
	}
	
	static void m3dGetPlaneEquation(M3DVector4f& planeEq,M3DVector3f& p1,
		M3DVector3f& p2,M3DVector3f& p3)
	{
		M3DVector3f v1
		M3DVector3f v2
		
		v1[0]=p3[0]-p1[0]
		v1[1]=p3[1]-p1[1]
		v1[2]=p3[2]-p1[2]
		
		v2[0]=p2[0]-p1[0]
		v2[1]=p2[1]-p1[1]
		v2[2]=p2[2]-p1[2]
		
		m3dCrossProduct(*(&planeEq).to<M3DVector3f*>,v1,v2)
		m3dNormalizeVector(*(&planeEq).to<M3DVector3f*>)
		planeEq[3]=-(planeEq[0]*p3[0]+planeEq[1]*p3[1]+planeEq[2]*p3[2])
	}
	
	static float m3dGetVectorLengthSquared(M3DVector3f& u)
	{ 
		return(u[0]*u[0])+(u[1]*u[1])+(u[2]*u[2])
	}
	
	static float m3dGetVectorLength(M3DVector3f& u)
	{ 
		return FL(math.sqrt(double(m3dGetVectorLengthSquared(u)))) 
	}
	
	static void m3dNormalizeVector(M3DVector3f& u)
	{ 
		m3dScaleVector3(u,1.0f/m3dGetVectorLength(u)) 
	}
	
	static void m3dScaleVector3(M3DVector3f& v,float scale) 
	{ 
		v[0]*=scale 
		v[1]*=scale 
		v[2]*=scale 
	}
	
	static void m3dCrossProduct(M3DVector3f& result,M3DVector3f& u,M3DVector3f& v)
	{
		result[0]=u[1]*v[2]-v[1]*u[2]
		result[1]=-u[0]*v[2]+v[0]*u[2]
		result[2]=u[0]*v[1]-v[0]*u[1]
	}
	
	void ApplyCameraTransform(bool bRotOnly=false) 
	{
		M3DMatrix44f m
		GetCameraOrientation(m)
		@glMultMatrixf(&m)
		if(!bRotOnly)
		{
			@glTranslatef(-vOrigin[0],-vOrigin[1],-vOrigin[2])
		}
	}
	
	static void m3dMakePlanarShadowMatrix(
		M3DMatrix44f& proj,
		M3DVector4f& planeEq, 
		M3DVector4f& vLightPos)
	{	
		float a=planeEq[0]
		float b=planeEq[1]
		float c=planeEq[2]
		float d=planeEq[3]
	
		float dx=-vLightPos[0]
		float dy=-vLightPos[1]
		float dz=-vLightPos[2]
	
		proj[0]=b*dy+c*dz
		proj[1]=-a*dy
		proj[2]=-a*dz
		proj[3]=0.0
	
		proj[4]=-b*dx
		proj[5]=a*dx+c*dz
		proj[6]=-b*dz
		proj[7]=0.0
	
		proj[8]=-c*dx
		proj[9]=-c*dy
		proj[10]=a*dx+b*dy
		proj[11]=0.0
	
		proj[12]=-d*dx
		proj[13]=-d*dy
		proj[14]=-d*dz
		proj[15]=a*dx+b*dy+c*dz
	}
	
	mac M(row,col) m[col*4+row]
	
	void GetCameraOrientation(M3DMatrix44f& m)
	 {
		M3DVector3f x
		M3DVector3f z

		z[0]=-vForward[0]
		z[1]=-vForward[1]
		z[2]=-vForward[2]

		m3dCrossProduct(x,vUp,z)
		
		M(0,0)=x[0]
		M(0,1)=x[1]
		M(0,2)=x[2]
		M(0,3)=0.0
		M(1,0)=vUp[0]
		M(1,1)=vUp[1]
		M(1,2)=vUp[2]
		M(1,3)=0.0
		M(2,0)=z[0]
		M(2,1)=z[1]
		M(2,2)=z[2]
		M(2,3)=0.0
		M(3,0)=0.0
		M(3,1)=0.0
		M(3,2)=0.0
		M(3,3)=1.0
	 }
}