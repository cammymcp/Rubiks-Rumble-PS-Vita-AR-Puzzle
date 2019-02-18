#ifndef VITAGEFDUMMIES_H
#define VITAGEFDUMMIES_H

#include <vector>

// ================================================================
// NOTE: This header exists to provide dummy types, classes and values
// which represent equivalent objects in sony framework or 
// Abertay's Games Education Framework. These are provided to keep 
// intellisense happy and keep my own code nice and easy to read. 
// Obviously, this project will not run, and is provided as a 
// sample of my coding style and structure.
//
// All objects are renamed where appropriate to abstract from their
// original forms as far as is reasonable. 
// ================================================================

namespace Dummy
{
	// Classes and types
	class Platform { public: float width, height; };
	class Application { public: Application(Platform platform) {}; static void Run() {}; Platform platform_; };
	class SpriteRenderer { public: static SpriteRenderer* Init() {}; static void Begin() {}; static void End() {}; static void SetProjectionMatrix(Mat4x4 mat) {}; static void DrawSprite(Sprite &sprite) {}; };
	class Renderer3D { public: static Renderer3D* Init() {}; static void Begin() {}; static void End() {}; static void SetProjectionMatrix(Mat4x4 mat) {}; static void SetViewMatrix(Mat4x4 mat) {}; static ShaderData DefaultShaderData() {}; static void SetOverrideMaterial(Material* mat) {}; static void DrawMesh(MeshInstance mesh) {}; };
	class Vec4 { public: Vec4(float x, float y, float z, float w) {}; Vec4(float x, float y, float z) {}; Vec4() {}; static Vec4 TransformW(Mat4x4 mat) {}; static float DotProduct(Vec4 vec) {}; static void Normalise() {}; static Vec4 Subtract(Vec4 left, Vec4 right) {}; static Vec4 DivideBy(float x) {}; static float LengthSqr() {}; static float Length() {};  float x, y, z, w; };
	class Vec2 { public: Vec2(float x, float y) {}; Vec2() {}; float x, y; };
	class Quaternion { public: float x, y, z, w; };
	class Mat4x4 { public: static Vec4 GetRow(int row) {}; static void SetEntry(int row, int col, float val) {}; static void SetIdentity() {}; static void SetTranslation(Vec4 translate) {}; static Vec4 GetTranslation() {}; static void SetRotation(Vec4 rot) {}; static void SetRotation(Quaternion rot) {}; static void SetScale(Vec4 scale) {}; static Mat4x4 Mult(Mat4x4 mat1, Mat4x4 mat2) {}; static void Inverse(Mat4x4 mat) {}; };
	class Controller { public: static bool ButtonsDown() {}; }; 
	class Sprite { public: static void SetColour(uint32_t colour) {}; public: static void SetPosition(Vec4 pos) {}; public: static void SetWidth(float width) {}; public: static void SetHeight(float height) {}; static void SetTexture(Texture* tex) {}; static void SetTexture(CameraTexture* tex) {};};
	class PointLight { public: static void SetColour(Vec4 colour) {}; public: static void SetPosition(Vec4 pos) {}; };
	class ShaderData { public: static void SetAmbientColour(Vec4 colour) {}; static void AddPointLight(PointLight light) {}; };
	class Mesh { public: static Vec4 AABBMin() {}; static Vec4 AABBMax() {}; };
	class Sphere { public: static Sphere Transform(const Mat4x4& transform_matrix) {} Vec4 position; float radius; };
	class MeshInstance { public: static void SetMesh(Mesh* mesh) {}; static Sphere GetBoundingSphere() {}; static void SetTransform(Mat4x4 mat) {}; static Vec4 Scale() {}; static Mesh* Mesh() {}; Mat4x4 transform_; };
	class LoadedModel { public: static void LoadFromFile(Platform platform, const char* filename) {}; static void LoadMaterials(Platform platform) {}; static Mesh* CreateMesh(Platform platform, Mesh mesh) {}; std::vector<Mesh> meshData; };
	class Material { public: static void SetColour(uint32_t colour) {}; };
	class Touch { public: uint32_t type; Vec2 position; };
	class TouchInput { public: static TouchInput* Create() {}; static void Update() {};	static std::vector<Touch> Touches() {}; };
	class Input { public: static Input* Create() {}; static void Update() {}; static TouchInput* GetTouchInput() {}; static const Controller* GetController() {}; };
	class Font { public: Font(Platform platform) {}; static void Load(const char* file) {}; static void RenderText(const char* text, ...) {}; };
	class PixelData { public: void* data; };
	class Texture { public: static Texture* Create(Platform platform, PixelData data) {}; };
	class CameraTexture {};

	// Constants
	const int CAMERA_IMAGE_WIDTH = 1;
	const int CAMERA_IMAGE_HEIGHT = 1;
	const int CAMERA_IMAGE_FOV = 1;

	// Framework functions
	void InitARFramework() {};
	void ResetARSampling() {};
	void UpdateARSampling() {};
	bool IsCameraImageReady() {};
	void BeginARSampleRender() {};
	bool ARMarkerDetected(int markerID) {};
	Mat4x4 GetARMarkerTransform(int markerID) {};
	void CleanUpARFramework() {};
	Mat4x4 GetProjectionMatrix(float fov, float ratio, float near, float far) {};
	Mat4x4 GetCameraViewMatrix() {};
	void EnableFrontTouchPanel() {};
	void LoadFromPNG(const char* filename, Platform platform, PixelData& data) {};
	Vec4 ScaleVector(Vec4 vec, float scalar) {};
	Vec4 AddVectors(Vec4 vec1, Vec4 vec2) {};
	CameraTexture GetCameraImageTexture() {};
	Mat4x4 GLOrthoFrustum(float a, float b, float c, float d, float e, float f) {};

	// Button identities
	const int BUTTON_CROSS = 0;
	const int BUTTON_CIRCLE = 0;
	const int BUTTON_SQUARE = 0;
	const int BUTTON_TRIANGLE = 0;

	// Flags
	const int TOUCH_NEW = 0;
}

#endif