
/**
 * Copyright 2009 Sun Microsystems, Inc. All rights reserved.
 * This file is dual licensed under LGPL v2.1 and the Scintilla license (http://www.scintilla.org/License.txt).
 * @file PlatCocoa.h
 */

#ifndef PLATCOCOA_H
#define PLATCOCOA_H

#include <cstdlib>
#include <cassert>
#include <cstring>
#include <cstdio>

#include <sys/time.h>

#include <Cocoa/Cocoa.h>

#include "Debugging.h"
#include "Geometry.h"
#include "Platform.h"
#include "Scintilla.h"

#include "QuartzTextLayout.h"

NSRect PRectangleToNSRect(const Scintilla::PRectangle &rc);
Scintilla::PRectangle NSRectToPRectangle(NSRect &rc);
CFStringEncoding EncodingFromCharacterSet(bool unicode, int characterSet);

@interface ScintillaContextMenu : NSMenu {
	Scintilla::ScintillaCocoa *owner;
}
- (void) handleCommand: (NSMenuItem *) sender;
- (void) setOwner: (Scintilla::ScintillaCocoa *) newOwner;

@end

namespace Scintilla {

// A class to do the actual text rendering for us using Quartz 2D.
class SurfaceImpl : public Surface {
private:
	bool unicodeMode;
	float x;
	float y;

	CGContextRef gc;

	/** The text layout instance */
	std::unique_ptr<QuartzTextLayout> textLayout;
	int codePage;
	int verticalDeviceResolution;

	/** If the surface is a bitmap context, contains a reference to the bitmap data. */
	std::unique_ptr<uint8_t[]> bitmapData;
	/** If the surface is a bitmap context, stores the dimensions of the bitmap. */
	int bitmapWidth;
	int bitmapHeight;

	/** Set the CGContext's fill colour to the specified desired colour. */
	void FillColour(const ColourDesired &back);
	void FillColour(ColourAlpha fill);

	void PenColourAlpha(ColourAlpha fore);

	void SetFillStroke(FillStroke fillStroke);

	// 24-bit RGB+A bitmap data constants
	static const int BITS_PER_COMPONENT = 8;
	static const int BITS_PER_PIXEL = BITS_PER_COMPONENT * 4;
	static const int BYTES_PER_PIXEL = BITS_PER_PIXEL / 8;

	void Clear();

public:
	SurfaceImpl();
	~SurfaceImpl() override;

	void Init(WindowID wid) override;
	void Init(SurfaceID sid, WindowID wid) override;
	void InitPixMap(int width, int height, Surface *surface_, WindowID wid) override;
	CGContextRef GetContext() { return gc; }

	void Release() noexcept override;
	int Supports(int feature) noexcept override;
	bool Initialised() override;
	void PenColour(ColourDesired fore) override;

	/** Returns a CGImageRef that represents the surface. Returns NULL if this is not possible. */
	CGImageRef CreateImage();
	void CopyImageRectangle(Surface &surfaceSource, PRectangle srcRect, PRectangle dstRect);

	int LogPixelsY() override;
	int PixelDivisions() override;
	int DeviceHeightFont(int points) override;
	void MoveTo(int x_, int y_) override;
	void LineTo(int x_, int y_) override;
	void Polygon(Scintilla::Point *pts, size_t npts, ColourDesired fore, ColourDesired back) override;
	void Polygon(const Scintilla::Point *pts, size_t npts, FillStroke fillStroke) override;
	void RectangleDraw(PRectangle rc, ColourDesired fore, ColourDesired back) override;
	void RectangleDraw(PRectangle rc, FillStroke fillStroke) override;
	void RectangleFrame(PRectangle rc, Stroke stroke) override;
	void FillRectangle(PRectangle rc, ColourDesired back) override;
	void FillRectangle(PRectangle rc, Fill fill) override;
	void FillRectangle(PRectangle rc, Surface &surfacePattern) override;
	void RoundedRectangle(PRectangle rc, ColourDesired fore, ColourDesired back) override;
	void RoundedRectangle(PRectangle rc, FillStroke fillStroke) override;
	void AlphaRectangle(PRectangle rc, int cornerSize, ColourDesired fill, int alphaFill,
			    ColourDesired outline, int alphaOutline, int flags) override;
	void AlphaRectangle(PRectangle rc, XYPOSITION cornerSize, FillStroke fillStroke) override;
	void GradientRectangle(PRectangle rc, const std::vector<ColourStop> &stops, GradientOptions options) override;
	void DrawRGBAImage(PRectangle rc, int width, int height, const unsigned char *pixelsImage) override;
	void Ellipse(PRectangle rc, ColourDesired fore, ColourDesired back) override;
	void Ellipse(PRectangle rc, FillStroke fillStroke) override;
	void Stadium(PRectangle rc, FillStroke fillStroke, Ends ends) override;
	void Copy(PRectangle rc, Scintilla::Point from, Surface &surfaceSource) override;
	std::unique_ptr<IScreenLineLayout> Layout(const IScreenLine *screenLine) override;

	void DrawTextNoClip(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourDesired fore,
			    ColourDesired back) override;
	void DrawTextClipped(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourDesired fore,
			     ColourDesired back) override;
	void DrawTextTransparent(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourDesired fore) override;
	void MeasureWidths(const Font *font_, std::string_view text, XYPOSITION *positions) override;
	XYPOSITION WidthText(const Font *font_, std::string_view text) override;

	void DrawTextNoClipUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourDesired fore,
			    ColourDesired back) override;
	void DrawTextClippedUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourDesired fore,
			     ColourDesired back) override;
	void DrawTextTransparentUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourDesired fore) override;
	void MeasureWidthsUTF8(const Font *font_, std::string_view text, XYPOSITION *positions) override;
	XYPOSITION WidthTextUTF8(const Font *font_, std::string_view text) override;

	XYPOSITION Ascent(const Font *font_) override;
	XYPOSITION Descent(const Font *font_) override;
	XYPOSITION InternalLeading(const Font *font_) override;
	XYPOSITION Height(const Font *font_) override;
	XYPOSITION AverageCharWidth(const Font *font_) override;

	void SetClip(PRectangle rc) override;
	void PopClip() override;
	void FlushCachedState() override;
	void FlushDrawing() override;

	void SetUnicodeMode(bool unicodeMode_) override;
	void SetDBCSMode(int codePage_) override;
	void SetBidiR2L(bool bidiR2L_) override;
}; // SurfaceImpl class

} // Scintilla namespace

#endif
