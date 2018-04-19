#include "RenderEngine.h"

namespace OpenHMI
{
		////////////////////////////////////// UI_DEFAUT //////////////////////////////////////////
	 char* s_pszVtx001 = "\
					attribute vec3			a_position; \
					attribute vec2			a_texCoord; \
					uniform mat4			u_uimatrix; \
					varying mediump vec2	v_texCoord; \
					void main(void)						\
					{									\
						gl_Position = u_uimatrix * vec4(a_position.x, a_position.y, 2.0, 1.0);\
						v_texCoord = a_texCoord;		\
					}";
	
	char* s_pvsAtt001[] = {"a_position", "a_texCoord" };
	char* s_pvsUni001[] = {"u_uimatrix"};

	 char* s_pszFrg001 = "\
				precision mediump float;							\
				varying mediump vec2	v_texCoord;					\
				uniform sampler2D s_texture;						\
				void main()											\
				{													\
				  gl_FragColor = texture2D( s_texture, v_texCoord );\
				} ";                                                  

	char* s_pfsSampler001[]	=	{"s_texture"};


	////////////////////////////////////// UI_APHA //////////////////////////////////////////
	//------------------------------
	 char* s_pszVtx002 = "\
					attribute vec3			a_position; \
					attribute vec2			a_texCoord; \
					uniform mat4			u_uimatrix; \
					varying mediump vec2	v_texCoord; \
					varying mediump float	v_alpha;	\
					void main(void)						\
					{									\
						gl_Position = u_uimatrix * vec4(a_position.x, a_position.y, 2.0, 1.0);\
						v_texCoord = a_texCoord;		\
						v_alpha = a_position.z;			\
					}";
	
	 char* s_pvsAtt002[] = {"a_position", "a_texCoord" };
	 char* s_pvsUni002[] = {"u_uimatrix"};

	//--------------------------------
	 char* s_pszFrg002 = "\
				precision mediump float;							\
				varying mediump vec2	v_texCoord;					\
				varying mediump float	v_alpha;	\
				uniform sampler2D s_texture;						\
				void main()											\
				{													\
				  gl_FragColor = texture2D( s_texture, v_texCoord );\
				  gl_FragColor = v_alpha * gl_FragColor;	\
				} ";                                                  

	 char* s_pfsSampler002[]	=	{"s_texture"};


	////////////////////////////////////// WAVE //////////////////////////////////////////
	//--------------------------------
	 char* s_pszVtx003 = "\
						attribute highp   vec4  inVertex;		\
						attribute highp   vec3  inNormal;		\
						attribute mediump vec2  inTexCoord;		\
						\
						uniform mediump mat4  MVPMatrix;		\
						uniform struct {						\
							mediump float    fFreq[7];			\
							mediump float    fAmp[7];			\
							mediump float    fPhase[7];			\
							mediump vec2     vDir[7];			\
						} Wave;									\
																\
						uniform highp float fTime;				\
						uniform int NumWaves;					\
						uniform highp vec3   EyePosition;		\
						uniform highp float  RIRSquare;			\
						\
						varying mediump vec2   ReflectCoord;	\
						varying mediump vec2   TexCoord;		\
						varying lowp    float  ReflectRatio;	\
						varying mediump float  z;				\
						\
						float EvaluateWave(int i, vec2 vPos, float fTime) {		\
							return Wave.fAmp[i] * sin( dot( Wave.vDir[i], vPos ) * Wave.fFreq[i] + fTime * Wave.fPhase[i] );	\
						} \
						\
						float EvaluateWaveDifferential(int i, vec2 vPos, float fTime ) {										\
							return Wave.fAmp[i] * Wave.fFreq[i] * cos( dot( Wave.vDir[i], vPos ) * Wave.fFreq[i] + fTime * Wave.fPhase[i] );	\
						} \
						\
						void main()	\
						{\
							highp vec4 inPos = inVertex;		\
							highp float ddx = 0.0;				\
							highp float ddy = 0.0;				\
							for(int i=0; i < 5; i++) {			\
								inPos.y += EvaluateWave(i, inPos.xz, fTime );		\
								float diff = EvaluateWaveDifferential(i, inPos.xz, fTime);		\
								ddx += diff * Wave.vDir[i].x;									\
								ddy += diff * Wave.vDir[i].y;									\
							}																	\
							vec3 normal = vec3( -ddx, 1, -ddy );								\
							\
							highp vec3 eyeDir = normalize(EyePosition - inVertex.xyz);			\
																				\
							\
							highp float c = abs(dot(eyeDir, normal));							\
							highp float g = sqrt(RIRSquare + c * c - 1.0);						\
							highp float f1 = (g - c) / (g + c);									\
							highp float f2 = (c * (g + c) - 1.0) / (c * (g - c) + 1.0);			\
							ReflectRatio = 0.5 * f1 * f1 * (1.0 + f2 * f2);						\
							\
							gl_Position = MVPMatrix * inPos;									\
							TexCoord = inTexCoord;												\
							z = inPos.z;			\
						}";
	 char* s_pvsAtt003[] = {"inVertex", "inNormal", "inTexCoord" };
	 char* s_pvsUni003[] = {"MVPMatrix", "Wave.fFreq[0]", "Wave.fAmp[0]", "Wave.fPhase[0]", "Wave.vDir[0]", "fTime", "EyePosition", "RIRSquare"};

	//--------------------------------
	 char* s_pszFrg003 = "\
				uniform sampler2D  sTexture;					\
				varying mediump vec2   ReflectCoord;			\
				varying mediump vec2   TexCoord;				\
				varying lowp    float  ReflectRatio;			\
				varying mediump float  z;						\
				\
				void main()										\
				{		\
					gl_FragColor = texture2D(sTexture, TexCoord) * ReflectRatio * 1.0;	\
				}";
	 char* s_pfsSampler003[]	=	{"sTexture"};


	////////////////////////////////////// UI transpant viewport //////////////////////////////////////////
	//------------------------------
	 char* s_pszVtx004 = "\
					attribute vec3			a_position; \
					attribute vec2			a_texCoord; \
					uniform mat4			u_uimatrix; \
					void main(void)						\
					{									\
						gl_Position = u_uimatrix * vec4(a_position.x, a_position.y, 2.0, 1.0);\
					}";
	
	 char* s_pvsAtt004[] = {"a_position", "a_texCoord" };
	 char* s_pvsUni004[] = {"u_uimatrix"};

	//--------------------------------
	 char* s_pszFrg004 = "\
				void main()										\
				{		\
					gl_FragColor = vec4(0.0,0.0,0.0,0.0);	\
				}";


	////////////////////////////////////// 2.5DUI rotate alpha //////////////////////////////////////////
	//------------------------------
	 char* s_pszVtx005 = "\
					attribute vec3			a_position; \
					attribute vec2			a_texCoord; \
					uniform mat4			u_uiMPmatrix; \
					varying mediump vec2	v_texCoord; \
					varying mediump float	v_alpha;	\
					void main(void)						\
					{									\
						gl_Position = u_uiMPmatrix * vec4(a_position.x, a_position.y, 0.0, 1.0);\
						v_texCoord = a_texCoord;		\
						v_alpha = a_position.z;			\
					}";
	
	 char* s_pvsAtt005[] = {"a_position", "a_texCoord" };
	 char* s_pvsUni005[] = {"u_uiMPmatrix"};

	//--------------------------------
	 char* s_pszFrg005 = s_pszFrg002;
	 char* s_pfsSampler005[]	=	{"s_texture"};


	////////////////////////////////////// openArtist line //////////////////////////////////////////
	//------------------------------
	 char* s_pszVtx006 = "\
					attribute vec2			a_position; \
					attribute vec4			a_color;	\
					uniform mat4			u_uiMPmatrix; \
					varying lowp vec4		v_color;	\
					void main(void)						\
					{									\
						gl_Position = u_uiMPmatrix * vec4(a_position.x, a_position.y, 2.0, 1.0);\
						v_color = a_color;		\
					}";
	
	 char* s_pvsAtt006[] = {"a_position", "a_color" };
	 char* s_pvsUni006[] = {"u_uiMPmatrix"};

	//--------------------------------
	 char* s_pszFrg006 = "\
				varying lowp vec4 v_color;			\
				void main()										\
				{		\
				gl_FragColor = v_color;	\
				}";


	/////////////////////////////////////// GAUSSIAN_BUR_H /////////////////////////////////////////
	//---------------------------------

	// The horizontal blur shader.
	// Vertex Shader (GS) . This shader screen align a quad with width 1. Any method to render a screen aligned quad will work. 
	// So you¡¯re free to use other shaders.
	// remember that you should draw a screen aligned quad
// 	void main(void)
// 	{
// 		gl_Position = ftransform();
// 
// 		// Clean up inaccuracies
// 		vec2 Pos;
// 		Pos = sign(gl_Vertex.xy);
// 
// 		gl_Position = vec4(Pos, 0.0, 1.0);
// 		// Image-space
// 		vTexCoord = Pos * 0.5 + 0.5;
// 	}

	 char* s_pszVtx007 = s_pszVtx001;
	
	 char* s_pvsAtt007[] = {"a_position", "a_texCoord" };
	 char* s_pvsUni007[] = {"u_uimatrix"};

	// Fragment Shader (GS)
	// uniform mediump float u_fBlurSize;
	// uniform sampler2D u_RTScene -- the texture with the scene you want to blur
	// take nine samples, with the distance blurSize between them.
	// const float blurSize = 1.0/512.0; 
	// I've chosen this size because this will result in that every step will be one pixel wide if the RTScene texture is of size 512x512 -- TODO pass from outside.
	 char* s_pszFrg007 = "\
							const	  mediump float blurSize = 1.0/280.0;           \
							uniform   sampler2D		sTexture;				\
							varying   mediump vec2			v_texCoord;				\
							mediump   vec4       a_sum = vec4(0.0);					\
							void main(void)									\
							{												\
								lowp vec4 curColor = texture2D(sTexture, v_texCoord);\
								 if( curColor.a > 0.001)\
								 {\
									a_sum += texture2D(sTexture, vec2(v_texCoord.x - 4.0*blurSize, v_texCoord.y)) * 0.05;	\
									a_sum += texture2D(sTexture, vec2(v_texCoord.x - 3.0*blurSize, v_texCoord.y)) * 0.09;	\
									a_sum += texture2D(sTexture, vec2(v_texCoord.x - 2.0*blurSize, v_texCoord.y)) * 0.12;	\
									a_sum += texture2D(sTexture, vec2(v_texCoord.x - blurSize, v_texCoord.y)) * 0.15;		\
									a_sum += curColor * 0.16;					\
									a_sum += texture2D(sTexture, vec2(v_texCoord.x + blurSize, v_texCoord.y)) * 0.15;		\
									a_sum += texture2D(sTexture, vec2(v_texCoord.x + 2.0*blurSize, v_texCoord.y)) * 0.12;	\
									a_sum += texture2D(sTexture, vec2(v_texCoord.x + 3.0*blurSize, v_texCoord.y)) * 0.09;	\
									a_sum += texture2D(sTexture, vec2(v_texCoord.x + 4.0*blurSize, v_texCoord.y)) * 0.05;	\
									gl_FragColor = a_sum; \
									gl_FragColor.a = curColor.a; \
								 }\
								 else\
								 {\
									gl_FragColor = curColor;	\
								 }\
							}";
	 char* s_pfsSampler007[]	=	{"sTexture"};

	
	/////////////////////////////////////// GAUSSIAN_BUR_V /////////////////////////////////////////
	 char* s_pszVtx008 = s_pszVtx001;
	
	 char* s_pvsAtt008[] = {"a_position", "a_texCoord" };
	 char* s_pvsUni008[] = {"u_uimatrix"};

	// blur in y (vertical)
	 char* s_pszFrg008 = "\
						 const	   mediump float blurSize = 1.0/160.0;           \
						 uniform   sampler2D	sTexture;				\
						 varying   mediump vec2			v_texCoord;				\
						 mediump   vec4       a_sum = vec4(0.0);					\
						 void main(void)							    \
						 {				\
							 lowp vec4 curColor = texture2D(sTexture, v_texCoord);\
							 if( curColor.a > 0.001)\
							 {\
								 a_sum += texture2D(sTexture, vec2(v_texCoord.x, v_texCoord.y - 4.0*blurSize)) * 0.05;	\
								 a_sum += texture2D(sTexture, vec2(v_texCoord.x, v_texCoord.y - 3.0*blurSize)) * 0.09;	\
								 a_sum += texture2D(sTexture, vec2(v_texCoord.x, v_texCoord.y - 2.0*blurSize)) * 0.12;	\
								 a_sum += texture2D(sTexture, vec2(v_texCoord.x, v_texCoord.y - blurSize)) * 0.15;		\
								 a_sum += curColor * 0.16;					\
								 a_sum += texture2D(sTexture, vec2(v_texCoord.x, v_texCoord.y + blurSize)) * 0.15;		\
								 a_sum += texture2D(sTexture, vec2(v_texCoord.x, v_texCoord.y + 2.0*blurSize)) * 0.12;	\
								 a_sum += texture2D(sTexture, vec2(v_texCoord.x, v_texCoord.y + 3.0*blurSize)) * 0.09;	\
								 a_sum += texture2D(sTexture, vec2(v_texCoord.x, v_texCoord.y + 4.0*blurSize)) * 0.05;	\
								 gl_FragColor = a_sum; \
								 gl_FragColor.a = curColor.a; \
							 }\
							 else\
							 {\
								gl_FragColor = curColor;	\
							 }\
						 }";
	 char* s_pfsSampler008[]	=	{"sTexture"};

	 /////////////////////////////////////// UI_MERGE /////////////////////////////////////////
	 char* s_pszVtx009 = "\
					attribute vec3			a_position; \
					attribute vec2			a_texCoord; \
					uniform mat4			u_uimatrix; \
					varying mediump vec2	v_texCoord; \
					varying lowp float		v_texidx;	\
					void main(void)						\
					{									\
						gl_Position = u_uimatrix * vec4(a_position.x, a_position.y, 2.0, 1.0);\
						v_texCoord = a_texCoord;		\
						v_texidx = a_position.z;		\
					}";
	
	 char* s_pvsAtt009[] = {"a_position", "a_texCoord" };
	 char* s_pvsUni009[] = {"u_uimatrix"};	

	 char* s_pszFrg009 = "\
										\
				varying mediump vec2	v_texCoord;					\
				varying lowp float		v_texidx;	\
				uniform sampler2D		s_texture[8];						\
				void main()											\
				{													\
				  int idx = int(v_texidx);								\
				  if( idx == 0 )	\
					gl_FragColor = texture2D( s_texture[0], v_texCoord );\
				  else if( idx == 1 ) \
				    gl_FragColor = texture2D( s_texture[1], v_texCoord );\
				  else if( idx == 2 ) \
				    gl_FragColor = texture2D( s_texture[2], v_texCoord );\
				  else if( idx == 3 ) \
				    gl_FragColor = texture2D( s_texture[3], v_texCoord );\
				  else if( idx == 4 ) \
				    gl_FragColor = texture2D( s_texture[4], v_texCoord );\
				  else if( idx == 5 ) \
				    gl_FragColor = texture2D( s_texture[5], v_texCoord );\
				  else if( idx == 6 ) \
				    gl_FragColor = texture2D( s_texture[6], v_texCoord );\
				  else if( idx == 7 ) \
				    gl_FragColor = texture2D( s_texture[7], v_texCoord );\
				} ";                                                  

	char* s_pfsSampler009[]	=	{ "s_texture[0]","s_texture[1]", "s_texture[2]", "s_texture[3]", 
								  "s_texture[4]", "s_texture[5]", "s_texture[6]", "s_texture[7]"};

	/////////////////////////////////////// UI_MASK /////////////////////////////////////////
	 char* s_pszVtx0010 = s_pszVtx001;
	
	 char* s_pvsAtt0010[] = {"a_position", "a_texCoord" };
	 char* s_pvsUni0010[] = {"u_uimatrix"};	

	 char* s_pszFrg0010 =  "\
				precision mediump float;							\
				varying mediump vec2	v_texCoord;					\
				uniform sampler2D s_texture;						\
				void main()											\
				{													\
				  gl_FragColor = texture2D( s_texture, v_texCoord );\
				} ";                                                  
                          

	char* s_pfsSampler0010[] = { "s_texture"};

	/////////////////////////////////////// UI_MASK2 /////////////////////////////////////////
	 char* s_pszVtx0011 = s_pszVtx001;
	
	 char* s_pvsAtt0011[] = {"a_position", "a_texCoord" };
	 char* s_pvsUni0011[] = {"u_uimatrix"};	

	 char* s_pszFrg0011 = "\
				precision mediump float;							\
				varying mediump vec2	v_texCoord;					\
				uniform sampler2D		s_texture[2];				\
				void main()											\
				{													\
					vec2 maskTexCoord = vec2(v_texCoord.x,1.0-v_texCoord.y);	\
					gl_FragColor = texture2D(s_texture[0],v_texCoord);	\
					float maskalpha = texture2D(s_texture[1],maskTexCoord).a;	\
					if( maskalpha > 0.01 )	\
					{\
						gl_FragColor.a = maskalpha;	\
					}\
					else	\
					{\
						gl_FragColor.a = 0.0;\
					}\
				} ";                                                  

	char* s_pfsSampler0011[] = { "s_texture[0]","s_texture[1]"};


#define VTX(i)			s_pszVtx##00##i
#define VSATT(i)		s_pvsAtt##00##i
#define VSATTNUM(i)		sizeof(s_pvsAtt##00##i)/sizeof(char*)
#define VSUNI(i)		s_pvsUni##00##i
#define VSUNINUM(i)		sizeof(s_pvsUni##00##i)/sizeof(char*)

#define FRG(i)			s_pszFrg##00##i
#define FSUNI(i)		s_pfsUni##00##i
#define FSUNINUM(i)		sizeof(s_pfsUni##00##i)/sizeof(char*)
#define FSSAMPLER(i)	s_pfsSampler##00##i
#define FSSAMPLERNUM(i)	sizeof(s_pfsSampler##00##i)/sizeof(char*)

	///////////////////////////////////////////////////////////////////////////////////////////////////
	void g_contructVertexShader(CShaderPool::EVertexShaderName eVSName, CCacheVertexShader* p)
	{
		if( p )
		{
			switch( eVSName )
			{
			case CShaderPool::EVS_UI_DEFAULT:
				p->setShader(VTX(1));
				p->setProperty(VSATT(1), VSATTNUM(1), VSUNI(1), VSUNINUM(1));
				break;
			case CShaderPool::EVS_UI_ALPHA:
				p->setShader(VTX(2));
				p->setProperty(VSATT(2), VSATTNUM(2), VSUNI(2), VSUNINUM(2));
				break;
			case CShaderPool::EVS_WAVE:
				p->setShader(VTX(3));
				p->setProperty(VSATT(3), VSATTNUM(3), VSUNI(3), VSUNINUM(3));
				break;
			case CShaderPool::EVS_UI_VIEWPORT:
				p->setShader(VTX(4));
				p->setProperty(VSATT(4), VSATTNUM(4), VSUNI(4), VSUNINUM(4));
				break;
			case CShaderPool::EVS_25DUI_ALPHA:
				p->setShader(VTX(5));
				p->setProperty(VSATT(5), VSATTNUM(5), VSUNI(5), VSUNINUM(5));
				break;
			case CShaderPool::EVS_OPENARTIST_LINESOLID:
				p->setShader(VTX(6));
				p->setProperty(VSATT(6), VSATTNUM(6), VSUNI(6), VSUNINUM(6));
				break;
			case CShaderPool::EVS_GAUSSIAN_BLUR_H:
				p->setShader(VTX(7));
				p->setProperty(VSATT(7), VSATTNUM(7), VSUNI(7), VSUNINUM(7));
				break;
			case CShaderPool::EVS_GAUSSIAN_BLUR_V:
				p->setShader(VTX(8));
				p->setProperty(VSATT(8), VSATTNUM(8), VSUNI(8), VSUNINUM(8));
				break;
			case CShaderPool::EVS_UI_DEFAULT_MERGE:
				p->setShader(VTX(9));
				p->setProperty(VSATT(9), VSATTNUM(9), VSUNI(9), VSUNINUM(9));
				break;
			case CShaderPool::EVS_UI_MASK:
				p->setShader(VTX(10));
				p->setProperty(VSATT(10), VSATTNUM(10), VSUNI(10), VSUNINUM(10));
				break;
			case CShaderPool::EVS_UI_MASK2:
				p->setShader(VTX(11));
				p->setProperty(VSATT(11), VSATTNUM(11), VSUNI(11), VSUNINUM(11));
				break;
			}
			
			p->compile(GL_VERTEX_SHADER);
		}
	}

	void g_contructFragmentShader(CShaderPool::EFragmentShaderName eFSName, CCacheFragmentShader* p)
	{
		if ( p)
		{
			switch( eFSName )
			{
			case CShaderPool::EFS_UI_DEFAULT:
				p->setShader(FRG(1));
				p->setProperty(FSSAMPLER(1), FSSAMPLERNUM(1), NULL, 0);
				break;
			case CShaderPool::EFS_UI_ALPHA:
				p->setShader(FRG(2));
				p->setProperty(FSSAMPLER(2), FSSAMPLERNUM(2), NULL, 0);
				break;
			case CShaderPool::EFS_WAVE:
				p->setShader(FRG(3));
				p->setProperty(FSSAMPLER(3), FSSAMPLERNUM(3), NULL, 0);
				break;
			case CShaderPool::EFS_UI_VIEWPORT:
				p->setShader(FRG(4));
				p->setProperty(NULL, 0, NULL, 0);
				break;
			case CShaderPool::EFS_25DUI_ALPHA:
				p->setShader(FRG(5));
				p->setProperty(FSSAMPLER(5), FSSAMPLERNUM(5), NULL, 0);
				break;
			case CShaderPool::EFS_OPENARTIST_LINESOLID:
				p->setShader(FRG(6));
				p->setProperty(NULL, 0, NULL, 0);
				break;
			case CShaderPool::EFS_GAUSSIAN_BLUR_H:
				p->setShader(FRG(7));
				p->setProperty(FSSAMPLER(7), FSSAMPLERNUM(7), NULL, 0);
				break;
			case CShaderPool::EFS_GAUSSIAN_BLUR_V:
				p->setShader(FRG(8));
				p->setProperty(FSSAMPLER(8), FSSAMPLERNUM(8), NULL, 0);
				break;
			case CShaderPool::EFS_UI_DEFAULT_MERGE:
				p->setShader(FRG(9));
				p->setProperty(FSSAMPLER(9), FSSAMPLERNUM(9), NULL, 0);
				break;
			case CShaderPool::EFS_UI_MASK:
				p->setShader(FRG(10));
				p->setProperty(FSSAMPLER(10), FSSAMPLERNUM(10), NULL, 0);
				break;
			case CShaderPool::EFS_UI_MASK2:
				p->setShader(FRG(11));
				p->setProperty(FSSAMPLER(11), FSSAMPLERNUM(11), NULL, 0);
				break;

			}
			p->compile(GL_FRAGMENT_SHADER);
		}
	}


}