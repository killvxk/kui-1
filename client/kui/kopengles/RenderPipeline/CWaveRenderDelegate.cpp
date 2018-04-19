#include "RenderEngine.h"

namespace OpenHMI
{
	enum EVertexAttrib {
			 VERTEX_ARRAY, NORMAL_ARRAY, TEXCOORD_ARRAY, eNumAttribs };

	// Shader uniforms
	enum EUniform {
			 eMVPMatrix, efFreq, efAmp, efPhase, evDir, efTime, eEyePosition, eRIRSquare, eNumUniforms };

	enum ESampler {
			 sTexture, eNumSamplers};

	typedef struct {
			 float x;
			 float y;
	} Dir2;

	typedef struct WaveTag { 
		float    fFreq[7];   
		float    fAmp[7];  
		float    fPhase[7];   
		Dir2     vDir[7];
	} WaveStruct;

	int NumWaves=5;
	#define fRIR 2.0
	#define WAVE_SCALE 1

	WaveStruct wave;

	CWaveRenderDelegate::CWaveRenderDelegate()
		: m_uiTexID(0)
	{
		;
	}

	CWaveRenderDelegate::~CWaveRenderDelegate()
	{
	}

	void CWaveRenderDelegate::initialize()
	{
	     NumWaves = 5;
	 
		 wave.fFreq[0]    = 0.2f;
		 wave.fAmp[0]    = WAVE_SCALE*1.5f;
		 wave.fPhase[0] = 0.1f;
		 wave.vDir[0].x    = 1.0f;
		 wave.vDir[0].y    = 0.0f;

		 wave.fFreq[1]    = 0.8f;
		 wave.fAmp[1]    = WAVE_SCALE*0.2f;
		 wave.fPhase[1] = -0.4f;
		 wave.vDir[1].x    = 1.0f;
		 wave.vDir[1].y    = 0.0f;

		 wave.fFreq[2]    = 0.5f;
		 wave.fAmp[2]    = WAVE_SCALE*0.3f;
		 wave.fPhase[2] = 0.2f;
		 wave.vDir[2].x    = 1.0f;
		 wave.vDir[2].y    = 0.0f;

		 wave.fFreq[3]    = 0.6f;
		 wave.fAmp[3]    = WAVE_SCALE*0.5f;
		 wave.fPhase[3] = 0.2f;
		 wave.vDir[3].x    = -0.2f;
		 wave.vDir[3].y    = 0.8f;

		 wave.fFreq[4]    = 1.6f;
		 wave.fAmp[4]    = WAVE_SCALE*0.2f;
		 wave.fPhase[4] = -0.35f;
		 wave.vDir[4].x    = 0.1f;
		 wave.vDir[4].y    = 0.9f;
	}

	void CWaveRenderDelegate::setTexture(CCacheTexture* pTex)
	{
		if (pTex)
		{
			m_uiTexID = pTex->getTexID();
		}
	}

	void CWaveRenderDelegate::render(const Array2<INT>& aryUniformLoc, CCamera*  pCamera , const C3DMatrix& modelMatrix, Object* pObject, CRenderState* pRndState)
	{
		pRndState->enableDepthTest(TRUE);
		pRndState->enableBlend(TRUE);
		pRndState->blendFunc(CRenderState::EBF_SRC_ALPHA, CRenderState::EBF_ONE);
		pRndState->enableCullFace(FALSE);
		pRndState->cullFace(CRenderState::ECF_CCW);

		glActiveTexture(GL_TEXTURE0);
		// Binds the loaded texture
		glBindTexture(GL_TEXTURE_2D, m_uiTexID);

		const C3DMatrix& matView = pCamera->getViewMatrix();
		const C3DMatrix& matProject = pCamera->getProjectMatrix();
		C3DMatrix mvp = matProject;
		mvp *= matView;
		mvp *= modelMatrix;
		glUniformMatrix4fv(aryUniformLoc[eMVPMatrix], 1, GL_FALSE, mvp.ptr() );

		C3DVector eyePos = pCamera->getPosition();
		glUniform3fv(aryUniformLoc[eEyePosition], 1, eyePos.ptr());

		glUniform1f(aryUniformLoc[eRIRSquare], fRIR * fRIR);

		glUniform1fv(aryUniformLoc[efFreq], NumWaves, &wave.fFreq[0]);
		glUniform1fv(aryUniformLoc[efAmp], NumWaves, &wave.fAmp[0]);
		glUniform1fv(aryUniformLoc[efPhase], NumWaves, &wave.fPhase[0]);
		glUniform2fv(aryUniformLoc[evDir], NumWaves, &wave.vDir[0].x);

		FLOAT fTime = FLOAT(CTime::getTickCount())/1000;
		glUniform1fv(aryUniformLoc[efTime], 1, &fTime);
	}
}