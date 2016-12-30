/**
 * Copyright (c) 2006-2016 LOVE Development Team
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 **/

#ifndef LOVE_GRAPHICS_GRAPHICS_H
#define LOVE_GRAPHICS_GRAPHICS_H

// LOVE
#include "common/config.h"
#include "common/Module.h"
#include "common/StringMap.h"
#include "StreamBuffer.h"
#include "vertex.h"
#include "Color.h"
#include "Texture.h"

// C++
#include <string>
#include <vector>

namespace love
{
namespace graphics
{

const int MAX_COLOR_RENDER_TARGETS = 16;

/**
 * Globally sets whether gamma correction is enabled. Ideally this should be set
 * prior to using any Graphics module function.
 **/
void setGammaCorrect(bool gammacorrect);

/**
 * Gets whether global gamma correction is enabled.
 **/
bool isGammaCorrect();

/**
 * Gamma-corrects a color (converts it from sRGB to linear RGB, if
 * gamma correction is enabled.)
 * The color's components are expected to be in the range of [0, 1].
 **/
void gammaCorrectColor(Colorf &c);

/**
 * Un-gamma-corrects a color (converts it from linear RGB to sRGB, if
 * gamma correction is enabled.)
 * The color's components are expected to be in the range of [0, 1].
 **/
void unGammaCorrectColor(Colorf &c);

Colorf gammaCorrectColor(const Colorf &c);
Colorf unGammaCorrectColor(const Colorf &c);

class Graphics : public Module
{
public:

	static love::Type type;

	enum DrawMode
	{
		DRAW_LINE,
		DRAW_FILL,
		DRAW_MAX_ENUM
	};

	enum ArcMode
	{
		ARC_OPEN,
		ARC_CLOSED,
		ARC_PIE,
		ARC_MAX_ENUM
	};

	enum BlendMode
	{
		BLEND_ALPHA,
		BLEND_ADD,
		BLEND_SUBTRACT,
		BLEND_MULTIPLY,
		BLEND_LIGHTEN,
		BLEND_DARKEN,
		BLEND_SCREEN,
		BLEND_REPLACE,
		BLEND_NONE,
		BLEND_MAX_ENUM
	};

	enum BlendAlpha
	{
		BLENDALPHA_MULTIPLY,
		BLENDALPHA_PREMULTIPLIED,
		BLENDALPHA_MAX_ENUM
	};

	enum LineStyle
	{
		LINE_ROUGH,
		LINE_SMOOTH,
		LINE_MAX_ENUM
	};

	enum LineJoin
	{
		LINE_JOIN_NONE,
		LINE_JOIN_MITER,
		LINE_JOIN_BEVEL,
		LINE_JOIN_MAX_ENUM
	};

	enum StencilAction
	{
		STENCIL_REPLACE,
		STENCIL_INCREMENT,
		STENCIL_DECREMENT,
		STENCIL_INCREMENT_WRAP,
		STENCIL_DECREMENT_WRAP,
		STENCIL_INVERT,
		STENCIL_MAX_ENUM
	};

	enum CompareMode
	{
		COMPARE_LESS,
		COMPARE_LEQUAL,
		COMPARE_EQUAL,
		COMPARE_GEQUAL,
		COMPARE_GREATER,
		COMPARE_NOTEQUAL,
		COMPARE_ALWAYS,
		COMPARE_MAX_ENUM
	};

	enum Feature
	{
		FEATURE_MULTI_CANVAS_FORMATS,
		FEATURE_CLAMP_ZERO,
		FEATURE_LIGHTEN,
		FEATURE_FULL_NPOT,
		FEATURE_PIXEL_SHADER_HIGHP,
		FEATURE_MAX_ENUM
	};

	enum Renderer
	{
		RENDERER_OPENGL = 0,
		RENDERER_OPENGLES,
		RENDERER_MAX_ENUM
	};

	enum SystemLimit
	{
		LIMIT_POINT_SIZE,
		LIMIT_TEXTURE_SIZE,
		LIMIT_MULTI_CANVAS,
		LIMIT_CANVAS_MSAA,
		LIMIT_ANISOTROPY,
		LIMIT_MAX_ENUM
	};

	enum StackType
	{
		STACK_ALL,
		STACK_TRANSFORM,
		STACK_MAX_ENUM
	};

	struct RendererInfo
	{
		std::string name;
		std::string version;
		std::string vendor;
		std::string device;
	};

	struct Stats
	{
		int drawCalls;
		int canvasSwitches;
		int shaderSwitches;
		int canvases;
		int images;
		int fonts;
		size_t textureMemory;
	};

	struct ColorMask
	{
		bool r, g, b, a;

		ColorMask()
			: r(true), g(true), b(true), a(true)
		{}

		ColorMask(bool _r, bool _g, bool _b, bool _a)
			: r(_r), g(_g), b(_b), a(_a)
		{}

		bool operator == (const ColorMask &m) const
		{
			return r == m.r && g == m.g && b == m.b && a == m.a;
		}

		bool operator != (const ColorMask &m) const
		{
			return !(operator == (m));
		}
	};

	struct OptionalColorf
	{
		Colorf c;
		bool enabled;
	};

	struct StreamDrawRequest
	{
		vertex::PrimitiveMode primitiveMode = vertex::PrimitiveMode::TRIANGLES;
		vertex::CommonFormat formats[2];
		vertex::TriangleIndexMode indexMode = vertex::TriangleIndexMode::NONE;
		int vertexCount = 0;
		Texture *texture = nullptr;

		// FIXME: This is only needed for fonts. We should just change fonts to
		// use love.graphics Images instead of raw OpenGL textures.
		ptrdiff_t textureHandle = 0;

		StreamDrawRequest()
		{
			// VS2013 can't initialize arrays in the above manner...
			formats[1] = formats[0] = vertex::CommonFormat::NONE;
		}
	};

	struct StreamVertexData
	{
		void *stream[2];
	};

	class TempTransform
	{
	public:

		TempTransform(Graphics *gfx)
			: gfx(gfx)
		{
			gfx->pushTransform();
		}

		TempTransform(Graphics *gfx, const Matrix4 &t)
			: gfx(gfx)
		{
			gfx->pushTransform();
			gfx->transformStack.back() *= t;
		}

		~TempTransform()
		{
			gfx->popTransform();
		}

	private:
		Graphics *gfx;
	};

	Graphics();
	virtual ~Graphics();

	// Implements Module.
	virtual ModuleType getModuleType() const { return M_GRAPHICS; }

	/**
	 * Sets the current graphics display viewport dimensions.
	 **/
	virtual void setViewportSize(int width, int height, int pixelwidth, int pixelheight) = 0;

	/**
	 * Sets the current graphics display viewport and initializes the renderer.
	 * @param width The viewport width.
	 * @param height The viewport height.
	 **/
	virtual bool setMode(int width, int height, int pixelwidth, int pixelheight) = 0;

	/**
	 * Un-sets the current graphics display mode (uninitializing objects if
	 * necessary.)
	 **/
	virtual void unSetMode() = 0;

	/**
	 * Sets whether the module is active (internal use only.)
	 **/
	virtual void setActive(bool active) = 0;

	/**
	 * Gets whether the module is active. Graphics module methods are only
	 * guaranteed to work when it is active. Calling them otherwise may cause
	 * the program to crash (or worse.)
	 * Normally the module will always be active as long as a window exists, it
	 * may be different on some platforms (especially mobile ones.)
	 **/
	virtual bool isActive() const = 0;

	virtual int getWidth() const = 0;
	virtual int getHeight() const = 0;

	virtual bool isCanvasActive() const = 0;

	virtual void flushStreamDraws() = 0;
	StreamVertexData requestStreamDraw(const StreamDrawRequest &request);

	virtual Colorf getColor() const = 0;

	virtual float getLineWidth() const = 0;
	virtual LineStyle getLineStyle() const = 0;
	virtual LineJoin getLineJoin() const = 0;

	/**
	 * Draws a series of lines connecting the given vertices.
	 * @param coords Vertex components (x1, y1, ..., xn, yn). If x1,y1 == xn,yn the line will be drawn closed.
	 * @param count Number of items in the array, i.e. count = 2 * n
	 **/
	void polyline(const float *coords, size_t count);

	/**
	 * Draws a rectangle.
	 * @param x Position along x-axis for top-left corner.
	 * @param y Position along y-axis for top-left corner.
	 * @param w The width of the rectangle.
	 * @param h The height of the rectangle.
	 **/
	void rectangle(DrawMode mode, float x, float y, float w, float h);

	/**
	 * Variant of rectangle that draws a rounded rectangle.
	 * @param mode The mode of drawing (line/filled).
	 * @param x X-coordinate of top-left corner
	 * @param y Y-coordinate of top-left corner
	 * @param w The width of the rectangle.
	 * @param h The height of the rectangle.
	 * @param rx The radius of the corners on the x axis
	 * @param ry The radius of the corners on the y axis
	 * @param points The number of points to use per corner
	 **/
	void rectangle(DrawMode mode, float x, float y, float w, float h, float rx, float ry, int points);
	void rectangle(DrawMode mode, float x, float y, float w, float h, float rx, float ry);

	/**
	 * Draws a circle using the specified arguments.
	 * @param mode The mode of drawing (line/filled).
	 * @param x X-coordinate.
	 * @param y Y-coordinate.
	 * @param radius Radius of the circle.
	 * @param points Number of points to use to draw the circle.
	 **/
	void circle(DrawMode mode, float x, float y, float radius, int points);
	void circle(DrawMode mode, float x, float y, float radius);

	/**
	 * Draws an ellipse using the specified arguments.
	 * @param mode The mode of drawing (line/filled).
	 * @param x X-coordinate of center
	 * @param y Y-coordinate of center
	 * @param a Radius in x-direction
	 * @param b Radius in y-direction
	 * @param points Number of points to use to draw the circle.
	 **/
	void ellipse(DrawMode mode, float x, float y, float a, float b, int points);
	void ellipse(DrawMode mode, float x, float y, float a, float b);

	/**
	 * Draws an arc using the specified arguments.
	 * @param drawmode The mode of drawing (line/filled).
	 * @param arcmode The type of arc.
	 * @param x X-coordinate.
	 * @param y Y-coordinate.
	 * @param radius Radius of the arc.
	 * @param angle1 The angle at which the arc begins.
	 * @param angle2 The angle at which the arc terminates.
	 * @param points Number of points to use to draw the arc.
	 **/
	void arc(DrawMode drawmode, ArcMode arcmode, float x, float y, float radius, float angle1, float angle2, int points);
	void arc(DrawMode drawmode, ArcMode arcmode, float x, float y, float radius, float angle1, float angle2);

	/**
	 * Draws a polygon with an arbitrary number of vertices.
	 * @param mode The type of drawing (line/filled).
	 * @param coords Vertex components (x1, y1, x2, y2, etc.)
	 * @param count Coord array size
	 **/
	void polygon(DrawMode mode, const float *coords, size_t count);

	/**
	 * Gets the system-dependent numeric limit for the specified parameter.
	 **/
	virtual double getSystemLimit(SystemLimit limittype) const = 0;

	const Matrix4 &getTransform() const;
	const Matrix4 &getProjection() const;

	template <typename T>
	T *getScratchBuffer(size_t count)
	{
		size_t bytes = sizeof(T) * count;

		if (scratchBuffer.size() < bytes)
			scratchBuffer.resize(bytes);

		return (T *) scratchBuffer.data();
	}

	static bool getConstant(const char *in, DrawMode &out);
	static bool getConstant(DrawMode in, const char *&out);

	static bool getConstant(const char *in, ArcMode &out);
	static bool getConstant(ArcMode in, const char *&out);

	static bool getConstant(const char *in, BlendMode &out);
	static bool getConstant(BlendMode in, const char *&out);

	static bool getConstant(const char *in, BlendAlpha &out);
	static bool getConstant(BlendAlpha in, const char *&out);

	static bool getConstant(const char *in, LineStyle &out);
	static bool getConstant(LineStyle in, const char *&out);

	static bool getConstant(const char *in, LineJoin &out);
	static bool getConstant(LineJoin in, const char *&out);

	static bool getConstant(const char *in, StencilAction &out);
	static bool getConstant(StencilAction in, const char *&out);

	static bool getConstant(const char *in, CompareMode &out);
	static bool getConstant(CompareMode in, const char *&out);

	static bool getConstant(const char *in, Feature &out);
	static bool getConstant(Feature in, const char *&out);

	static bool getConstant(const char *in, SystemLimit &out);
	static bool getConstant(SystemLimit in, const char *&out);

	static bool getConstant(const char *in, StackType &out);
	static bool getConstant(StackType in, const char *&out);

protected:

	struct StreamBufferState
	{
		StreamBuffer *vb[2];
		StreamBuffer *indexBuffer;
		vertex::PrimitiveMode primitiveMode;
		vertex::CommonFormat formats[2];
		StrongRef<Texture> texture;
		ptrdiff_t textureHandle = 0;
		int vertexCount;
		int indexCount;
	};

	void pushTransform();
	void pushIdentityTransform();
	void popTransform();

	StreamBufferState streamBufferState;

	std::vector<Matrix4> transformStack;
	Matrix4 projectionMatrix;

	std::vector<double> pixelScaleStack;

private:

	int calculateEllipsePoints(float rx, float ry) const;

	std::vector<uint8> scratchBuffer;

	static StringMap<DrawMode, DRAW_MAX_ENUM>::Entry drawModeEntries[];
	static StringMap<DrawMode, DRAW_MAX_ENUM> drawModes;

	static StringMap<ArcMode, ARC_MAX_ENUM>::Entry arcModeEntries[];
	static StringMap<ArcMode, ARC_MAX_ENUM> arcModes;

	static StringMap<BlendMode, BLEND_MAX_ENUM>::Entry blendModeEntries[];
	static StringMap<BlendMode, BLEND_MAX_ENUM> blendModes;

	static StringMap<BlendAlpha, BLENDALPHA_MAX_ENUM>::Entry blendAlphaEntries[];
	static StringMap<BlendAlpha, BLENDALPHA_MAX_ENUM> blendAlphaModes;

	static StringMap<LineStyle, LINE_MAX_ENUM>::Entry lineStyleEntries[];
	static StringMap<LineStyle, LINE_MAX_ENUM> lineStyles;

	static StringMap<LineJoin, LINE_JOIN_MAX_ENUM>::Entry lineJoinEntries[];
	static StringMap<LineJoin, LINE_JOIN_MAX_ENUM> lineJoins;

	static StringMap<StencilAction, STENCIL_MAX_ENUM>::Entry stencilActionEntries[];
	static StringMap<StencilAction, STENCIL_MAX_ENUM> stencilActions;

	static StringMap<CompareMode, COMPARE_MAX_ENUM>::Entry compareModeEntries[];
	static StringMap<CompareMode, COMPARE_MAX_ENUM> compareModes;

	static StringMap<Feature, FEATURE_MAX_ENUM>::Entry featureEntries[];
	static StringMap<Feature, FEATURE_MAX_ENUM> features;

	static StringMap<SystemLimit, LIMIT_MAX_ENUM>::Entry systemLimitEntries[];
	static StringMap<SystemLimit, LIMIT_MAX_ENUM> systemLimits;

	static StringMap<StackType, STACK_MAX_ENUM>::Entry stackTypeEntries[];
	static StringMap<StackType, STACK_MAX_ENUM> stackTypes;

}; // Graphics

} // graphics
} // love

#endif // LOVE_GRAPHICS_GRAPHICS_H
