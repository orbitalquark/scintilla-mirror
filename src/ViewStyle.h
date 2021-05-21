// Scintilla source code edit control
/** @file ViewStyle.h
 ** Store information on how the document is to be viewed.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef VIEWSTYLE_H
#define VIEWSTYLE_H

namespace Scintilla {

/**
 */
class MarginStyle {
public:
	int style;
	ColourRGBA back;
	int width;
	int mask;
	bool sensitive;
	int cursor;
	MarginStyle(int style_= SC_MARGIN_SYMBOL, int width_=0, int mask_=0) noexcept;
	bool ShowsFolding() const noexcept;
};

/**
 */


class FontRealised : public FontMeasurements {
public:
	std::shared_ptr<Font> font;
	FontRealised() noexcept;
	// FontRealised objects can not be copied.
	FontRealised(const FontRealised &) = delete;
	FontRealised(FontRealised &&) = delete;
	FontRealised &operator=(const FontRealised &) = delete;
	FontRealised &operator=(FontRealised &&) = delete;
	virtual ~FontRealised();
	void Realise(Surface &surface, int zoomLevel, int technology, const FontSpecification &fs, const char *localeName);
};

enum class IndentView {none, real, lookForward, lookBoth};

enum class WhiteSpace {invisible=0, visibleAlways=1, visibleAfterIndent=2, visibleOnlyInIndent=3};

enum class TabDrawMode {longArrow=0, strikeOut=1};

typedef std::map<FontSpecification, std::unique_ptr<FontRealised>> FontMap;

enum class WrapMode { none, word, character, whitespace };

inline std::optional<ColourRGBA> OptionalColour(uptr_t wParam, sptr_t lParam) {
	if (wParam) {
		return ColourRGBA::FromRGB(static_cast<int>(lParam));
	} else {
		return {};
	}
}

struct SelectionAppearance {
	// Whether to draw on base layer or over text
	Layer layer;
	// Draw selection past line end characters up to right border
	bool eolFilled;
};

struct CaretLineAppearance {
	// Whether to draw on base layer or over text
	Layer layer;
	// Also show when non-focused
	bool alwaysShow;
	// Non-0: draw a rectangle around line instead of filling line. Value is pixel width of frame
	int frame;
};

struct CaretAppearance {
	// Line, block, over-strike bar ... 
	int style;
	// Width in pixels
	int width;
};

struct WrapAppearance {
	// No wrapping, word, character, whitespace appearance
	WrapMode state;
	// Show indication of wrap at line end, line start, or in margin
	int visualFlags;
	// Show indication near margin or near text
	int visualFlagsLocation;
	// How much indentation to show wrapping
	int visualStartIndent;
	// SC_WRAPINDENT_FIXED, _SAME, _INDENT, _DEEPINDENT
	int indentMode;
};

struct EdgeProperties {
	int column = 0;
	ColourRGBA colour;
	EdgeProperties(int column_ = 0, ColourRGBA colour_ = ColourRGBA::FromRGB(0)) noexcept :
		column(column_), colour(colour_) {
	}
	EdgeProperties(uptr_t wParam, sptr_t lParam) noexcept :
		column(static_cast<int>(wParam)), colour(ColourRGBA::FromRGB(static_cast<int>(lParam))) {
	}
};

/**
 */
class ViewStyle {
	UniqueStringSet fontNames;
	FontMap fonts;
public:
	std::vector<Style> styles;
	int nextExtendedStyle;
	std::vector<LineMarker> markers;
	int largestMarkerHeight;
	std::vector<Indicator> indicators;
	bool indicatorsDynamic;
	bool indicatorsSetFore;
	int technology;
	int lineHeight;
	int lineOverlap;
	unsigned int maxAscent;
	unsigned int maxDescent;
	XYPOSITION aveCharWidth;
	XYPOSITION spaceWidth;
	XYPOSITION tabWidth;

	SelectionAppearance selection;

	int controlCharSymbol;
	XYPOSITION controlCharWidth;
	ColourRGBA selbar;
	ColourRGBA selbarlight;
	std::optional<ColourRGBA> foldmarginColour;
	std::optional<ColourRGBA> foldmarginHighlightColour;
	bool hotspotUnderline;
	/// Margins are ordered: Line Numbers, Selection Margin, Spacing Margin
	int leftMarginWidth;	///< Spacing margin on left of text
	int rightMarginWidth;	///< Spacing margin on right of text
	int maskInLine;	///< Mask for markers to be put into text because there is nowhere for them to go in margin
	int maskDrawInText;	///< Mask for markers that always draw in text
	std::vector<MarginStyle> ms;
	int fixedColumnWidth;	///< Total width of margins
	bool marginInside;	///< true: margin included in text view, false: separate views
	int textStart;	///< Starting x position of text within the view
	int zoomLevel;
	WhiteSpace viewWhitespace;
	TabDrawMode tabDrawMode;
	int whitespaceSize;
	IndentView viewIndentationGuides;
	bool viewEOL;

	CaretAppearance caret;

	CaretLineAppearance caretLine;

	bool someStylesProtected;
	bool someStylesForceCase;
	int extraFontFlag;
	int extraAscent;
	int extraDescent;
	int marginStyleOffset;
	int annotationVisible;
	int annotationStyleOffset;
	int eolAnnotationVisible;
	int eolAnnotationStyleOffset;
	bool braceHighlightIndicatorSet;
	int braceHighlightIndicator;
	bool braceBadLightIndicatorSet;
	int braceBadLightIndicator;
	int edgeState;
	EdgeProperties theEdge;
	std::vector<EdgeProperties> theMultiEdge;
	int marginNumberPadding; // the right-side padding of the number margin
	int ctrlCharPadding; // the padding around control character text blobs
	int lastSegItalicsOffset; // the offset so as not to clip italic characters at EOLs

	using ElementMap = std::map<int, std::optional<ColourRGBA>>;
	ElementMap elementColours;
	ElementMap elementBaseColours;
	std::set<int> elementAllowsTranslucent;

	WrapAppearance wrap;

	std::string localeName;

	ViewStyle();
	ViewStyle(const ViewStyle &source);
	ViewStyle(ViewStyle &&) = delete;
	// Can only be copied through copy constructor which ensures font names initialised correctly
	ViewStyle &operator=(const ViewStyle &) = delete;
	ViewStyle &operator=(ViewStyle &&) = delete;
	~ViewStyle();
	void CalculateMarginWidthAndMask() noexcept;
	void Init(size_t stylesSize_=256);
	void Refresh(Surface &surface, int tabInChars);
	void ReleaseAllExtendedStyles() noexcept;
	int AllocateExtendedStyles(int numberStyles);
	void EnsureStyle(size_t index);
	void ResetDefaultStyle();
	void ClearStyles();
	void SetStyleFontName(int styleIndex, const char *name);
	void SetFontLocaleName(const char *name);
	bool ProtectionActive() const noexcept;
	int ExternalMarginWidth() const noexcept;
	int MarginFromLocation(Point pt) const noexcept;
	bool ValidStyle(size_t styleIndex) const noexcept;
	void CalcLargestMarkerHeight() noexcept;
	int GetFrameWidth() const noexcept;
	bool IsLineFrameOpaque(bool caretActive, bool lineContainsCaret) const;
	std::optional<ColourRGBA> Background(int marksOfLine, bool caretActive, bool lineContainsCaret) const;
	bool SelectionBackgroundDrawn() const noexcept;
	bool SelectionTextDrawn() const;
	bool WhitespaceBackgroundDrawn() const;
	ColourRGBA WrapColour() const;

	void AddMultiEdge(uptr_t wParam, sptr_t lParam);

	std::optional<ColourRGBA> ElementColour(int element) const;
	bool ElementAllowsTranslucent(int element) const;
	bool ResetElement(int element);
	bool SetElementColour(int element, ColourRGBA colour);
	bool SetElementColourOptional(int element, uptr_t wParam, sptr_t lParam);
	void SetElementRGB(int element, int rgb);
	void SetElementAlpha(int element, int alpha);
	bool ElementIsSet(int element) const;
	bool SetElementBase(int element, ColourRGBA colour);

	bool SetWrapState(int wrapState_) noexcept;
	bool SetWrapVisualFlags(int wrapVisualFlags_) noexcept;
	bool SetWrapVisualFlagsLocation(int wrapVisualFlagsLocation_) noexcept;
	bool SetWrapVisualStartIndent(int wrapVisualStartIndent_) noexcept;
	bool SetWrapIndentMode(int wrapIndentMode_) noexcept;

	bool WhiteSpaceVisible(bool inIndent) const noexcept;

	enum class CaretShape { invisible, line, block, bar };
	bool IsBlockCaretStyle() const noexcept;
	bool IsCaretVisible() const noexcept;
	bool DrawCaretInsideSelection(bool inOverstrike, bool imeCaretBlockOverride) const noexcept;
	CaretShape CaretShapeForMode(bool inOverstrike) const noexcept;

private:
	void AllocStyles(size_t sizeNew);
	void CreateAndAddFont(const FontSpecification &fs);
	FontRealised *Find(const FontSpecification &fs);
	void FindMaxAscentDescent();
};

}

#endif
