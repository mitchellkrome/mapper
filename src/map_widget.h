/*
 *    Copyright 2012-2014 Thomas Schöps
 *    Copyright 2013-2015 Kai Pastor
 *
 *    This file is part of OpenOrienteering.
 *
 *    OpenOrienteering is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    OpenOrienteering is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with OpenOrienteering.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef _OPENORIENTEERING_MAP_WIDGET_H_
#define _OPENORIENTEERING_MAP_WIDGET_H_

#include <type_traits>

#include <QImage>
#include <QPixmap>
#include <QTime>
#include <QWidget>

#include "core/map_view.h"
#include "map.h"

QT_BEGIN_NAMESPACE
class QGestureEvent;
class QLabel;
QT_END_NAMESPACE

class MapEditorActivity;
class MapEditorTool;
class MapView;
class PieMenu;
class TouchCursor;
class GPSDisplay;
class CompassDisplay;
class GPSTemporaryMarkers;

/**
 * QWidget for displaying a map. Needs a pointer to a MapView which defines
 * the view properties.
 * 
 * For faster display, the widget keeps some cached image internally which
 * are of the same size as the widget area. If then for example the map changes,
 * the other caches do not need to be redrawn.
 * <ul>
 * <li>The <b>map cache</b> contains the currently visible part of the map</li>
 * <li>The <b>below template cache</b> contains the currently
 *     visible part of all templates below the map</li>
 * <li>The <b>above template cache</b> contains the currently
 *     visible part of all templates above the map</li>
 * </ul>
 */
class MapWidget : public QWidget
{
Q_OBJECT
friend class MapView;
public:
	/** Describes different display formats for coordinates. */
	enum CoordsType
	{
		/** Map coordinates: millimeters on map paper */
		MAP_COORDS,
		/** Projected coordinates, e.g. UTM */
		PROJECTED_COORDS,
		/** Geographic WGS84 coordinates */
		GEOGRAPHIC_COORDS,
		/** Geographic WGS84 coordinates in degrees, minutes, seconds */
		GEOGRAPHIC_COORDS_DMS
	};
	
	/** Describes how a zoom level can be determined. */
	enum ZoomOption
	{
		ContinuousZoom, ///< Allow any zoom value in the valid range.
		DiscreteZoom,   ///< Adjust the zoom to the closes valid step.
	};
	
	/**
	 * Constructs a new MapWidget.
	 * 
	 * @param show_help If set to true, the map widget shows help texts for
	 *     empty maps.
	 * @param force_antialiasing If set to true, the map widget uses antialiasing
	 *     for display, even if it is disabled in the program settings.
	 *     Useful for the symbol editor.
	 * @param parent Optional QWidget parent.
	 */
	MapWidget(bool show_help, bool force_antialiasing, QWidget* parent = nullptr);
	
	/** Destructs the MapWidget. */
	~MapWidget();
	
	/** Sets the map view to use for display. Does not take ownership of the view. */
	void setMapView(MapView* view);
	
	/** Returns the map view used for display. */
	MapView* getMapView() const;
	
	
	/** Sets the tool to use in this widget. Does not take ownership of the tool. */
	void setTool(MapEditorTool* tool);
	
	/** Sets the activity to use in this widget. Does not take ownership of the activity. */
	void setActivity(MapEditorActivity* activity);
	
	
	/**
	 * @brief Enables or disables gesture recognition.
	 * 
	 * MapWidget can recognize gestures, such as two-finger gestures for panning
	 * and zooming. However, this may disturb the work with editing tools. So gestures
	 * may be disabled.
	 * 
	 * @param enabled If true, enables gesture recognition. Otherwise gestures are disabled.
	 */
	void setGesturesEnabled(bool enabled);
	
	/**
	 * @brief Returns true if gesture recognition is enabled.
	 * 
	 */
	bool gesturesEnabled() const;
	
	
	/**
	 * Applies the complete transform to the painter which enables to draw
	 * map objects with map coordinates and have them correctly displayed in
	 * the widget with the settings of the used MapView.
	 */
	void applyMapTransform(QPainter* painter) const;
	
	// Coordinate transformations
	
	/** Maps viewport (GUI) coordinates to view coordinates (see MapView). */
	QRectF viewportToView(const QRect& input) const;
	/** Maps viewport (GUI) coordinates to view coordinates (see MapView). */
	QPointF viewportToView(QPoint input) const;
	/** Maps viewport (GUI) coordinates to view coordinates (see MapView). */
	QPointF viewportToView(QPointF input) const;
	/** Maps view coordinates (see MapView) to viewport (GUI) coordinates. */
	QRectF viewToViewport(const QRectF& input) const;
	/** Maps view coordinates (see MapView) to viewport (GUI) coordinates. */
	QRectF viewToViewport(const QRect& input) const;
	/** Maps view coordinates (see MapView) to viewport (GUI) coordinates. */
	QPointF viewToViewport(QPoint input) const;
	/** Maps view coordinates (see MapView) to viewport (GUI) coordinates. */
	QPointF viewToViewport(QPointF input) const;
	
	/** Maps viewport (GUI) coordinates to map coordinates. */
	MapCoord viewportToMap(QPoint input) const;
	/** Maps viewport (GUI) coordinates to map coordinates. */
	MapCoordF viewportToMapF(QPoint input) const;
	/** Maps viewport (GUI) coordinates to map coordinates. */
	MapCoordF viewportToMapF(QPointF input) const;
	/** Maps map coordinates to viewport (GUI) coordinates. */
	QPointF mapToViewport(MapCoord input) const;
	/** Maps map coordinates to viewport (GUI) coordinates. */
	QPointF mapToViewport(MapCoordF input) const;
	/** Maps map coordinates to viewport (GUI) coordinates. */
	QPointF mapToViewport(QPointF input) const;
	/** Maps map coordinates to viewport (GUI) coordinates. */
	QRectF mapToViewport(const QRectF& input) const;
	
	
	/** Notifies the MapWidget of the view having zoomed, moved or rotated. */
	void viewChanged(MapView::ChangeFlags changes);
	
	
	/** 
	 * Returns the current offset (in pixel) during a map pan operation.
	 */
	QPoint panOffset() const;
	
	/** 
	 * Sets the current offset (in pixel) during a map pan operation.
	 */
	void setPanOffset(QPoint offset);
	
	
	/**
	 * Adjusts the viewport so the given rect is inside the view.
	 */
	void ensureVisibilityOfRect(const QRectF& map_rect, ZoomOption zoom_option);
	
	/**
	 * Sets the view so the rect is centered and zooomed to fill the widget.
	 */
	void adjustViewToRect(const QRectF& map_rect, ZoomOption zoom_option);
	
	/**
	 * Mark a rectangular region of a template cache as "dirty", i.e. redraw needed.
	 * This rect is united with possible previous dirty rects of that cache.
	 * @param view_rect Affected rect in view coordinates.
	 * @param pixel_border Additional affected extent around the view rect in
	 *     pixels. Allows to specify zoom-independent extents.
	 * @param front_cache If set to true, invalidates the cache for templates
	 *     in front of the map, else invalidates the cache for templates behind the map.
	 */
	void markTemplateCacheDirty(QRectF view_rect, int pixel_border, bool front_cache);
	
	/**
	 * Mark a rectangular region given in map coordinates of the map cache
	 * as dirty, i.e. redraw needed.
	 * This rect is united with possible previous dirty rects of that cache.
	 */
	void markObjectAreaDirty(QRectF map_rect);
	
	/**
	 * Set the given rect as bounding box for the current drawing, i.e. the
	 * graphical display of the active tool.
	 * NOTE: Unlike for markTemplateCacheDirty(), multiple calls to
	 * these methodsdo not result in uniting all given rects,
	 * instead only the last rect is used!
	 * Pass QRect() to disable the current drawing.
	 * @param map_rect Affected rect in map coordinates.
	 * @param pixel_border Additional affected extent around the map rect in
	 *     pixels. Allows to specify zoom-independent extents.
	 * @param do_update If set to true, triggers a redraw of the widget.
	 */
	void setDrawingBoundingBox(QRectF map_rect, int pixel_border, bool do_update);
	/**
	 * Removes the area set with setDrawingBoundingBox() and triggers a redraw
	 * of the widget, if needed.
	 */
	void clearDrawingBoundingBox();
	
	/** Analogon to setDrawingBoundingBox() for activities. */
	void setActivityBoundingBox(QRectF map_rect, int pixel_border, bool do_update);
	/** Analogon to clearDrawingBoundingBox() for activities. */
	void clearActivityBoundingBox();
	
	/**
	 * Triggers a redraw of the MapWidget at the given area.
	 * @param map_rect Affected rect in map coordinates.
	 * @param pixel_border Additional affected extent around the map rect in
	 *     pixels. Allows to specify zoom-independent extents.
	 */
	void updateDrawing(const QRectF& map_rect, int pixel_border);
	/**
	 * Triggers a redraw of the MapWidget at the given area.
	 */
	void updateMapRect(const QRectF& map_rect, int pixel_border, QRect& cache_dirty_rect);
	/**
	 * Triggers a redraw of the MapWidget at the given area.
	 */
	void updateViewportRect(QRect viewport_rect, QRect& cache_dirty_rect);
	/**
	 * Variant of updateDrawing() which waits for some milliseconds before
	 * calling update() in order to avoid excessive redraws.
	 */
	void updateDrawingLater(QRectF map_rect, int pixel_border);
	
	/**
	 * Invalidates all caches and redraws the whole widget. Very slow, try to
	 * avoid this.
	 */
	void updateEverything();
	/**
	 * Sets all "dirty" region markers to the given rect in viewport coordinates
	 * and triggers a redraw of the MapWidget there.
	 */
	void updateEverythingInRect(const QRect& dirty_rect);
	
	/** Specify the label where the MapWidget will display zoom information. */
	void setZoomLabel(QLabel* zoom_label);
	/** Specify the label where the MapWidget will display cursor position information. */
	void setCursorposLabel(QLabel* cursorpos_label);
	/** Specify the label where the MapWidget will display object tag information. */
	void setObjectTagLabel(QLabel* objecttag_label);
	/**
	 * Specify the system and format for displaying coordinates in
	 * the cursorpos label. See CoordsType for the available types.
	 */
	void setCoordsDisplay(CoordsType type);
	/** Returns the coordinate display type set by setCoordsDisplay(). */
	inline CoordsType getCoordsDisplay() const;
	
	/** Returns the time in milliseconds since the last user interaction
	 *  (mouse press or drag) with the widget. */
	int getTimeSinceLastInteraction();
	
	/** Sets the GPS display to use. This is called internally by the GPSDisplay constructor. */
	void setGPSDisplay(GPSDisplay* gps_display);
	/** Sets the GPS temporary markers display to use. This is called internally by the GPSTemporaryMarkers constructor. */
	void setTemporaryMarkerDisplay(GPSTemporaryMarkers* marker_display);
	
	/** Returns the widget's context menu widget. */
	QWidget* getContextMenu();
	
	/** Returns the widget's preferred size. */
	virtual QSize sizeHint() const;
	
	/**
	 * @copybrief MainWindowController::keyPressEventFilter
	 * Delegates the keyPress to the active tool, or handles some shortcuts itself.
	 */
	bool keyPressEventFilter(QKeyEvent* event);
	
	/**
	 * @copybrief MainWindowController::keyPressEventFilter
	 * Delegates the keyRelease to the active tool.
	 */
	bool keyReleaseEventFilter(QKeyEvent* event);

public slots:
	/** Enables or disables the touch cursor. */
	void enableTouchCursor(bool enabled);

private slots:
	void updateObjectTagLabel();
	void updateDrawingLaterSlot();
	
protected:
	virtual bool event(QEvent *event);
	
	virtual void gestureEvent(QGestureEvent* event);
	
	virtual void paintEvent(QPaintEvent* event);
	virtual void resizeEvent(QResizeEvent* event);
	
	// Mouse input
	virtual void mousePressEvent(QMouseEvent* event);
	void _mousePressEvent(QMouseEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	void _mouseMoveEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	void _mouseReleaseEvent(QMouseEvent* event);
	virtual void mouseDoubleClickEvent(QMouseEvent* event);
	void _mouseDoubleClickEvent(QMouseEvent* event);
	virtual void wheelEvent(QWheelEvent* event);
	virtual void leaveEvent(QEvent* event);
	
	// Key input (see also slots)
	virtual void focusOutEvent(QFocusEvent* event);
	
	virtual void contextMenuEvent(QContextMenuEvent* event);
	
private:
	/** Checks if there is a visible template in the range
	 *  from first_template to last_template. */
	bool containsVisibleTemplate(int first_template, int last_template) const;
	/** Checks if there is any visible template above the map. */
	bool isAboveTemplateVisible() const;
	/** Checks if there is any visible template below the map. */
	bool isBelowTemplateVisible() const;
	/**
	 * Redraws the template cache.
	 * @param cache Reference to pointer to the cache.
	 * @param dirty_rect Rectangle of the cache to redraw, in viewport coordinates.
	 * @param first_template Lowest template index to draw.
	 * @param last_template Highest template index to draw.
	 * @param use_background If set to true, fills the cache with white before
	 *     drawing the templates, else makes it transparent.
	 */
	void updateTemplateCache(QImage& cache, QRect& dirty_rect, int first_template, int last_template, bool use_background);
	/**
	 * Redraws the map cache in the map cache dirty rect.
	 * @param use_background If set to true, fills the cache with white before
	 *     drawing the map, else makes it transparent.
	 */
	void updateMapCache(bool use_background);
	/** Redraws all dirty caches. */
	void updateAllDirtyCaches();
	/** Shifts the content in the cache by the given amount of pixels. */
	void shiftCache(int sx, int sy, QImage& cache);
	void shiftCache(int sx, int sy, QPixmap& cache);
	
	/**
	 * Calculates the bounding box of the given map coordinates rect and
	 * additional pixel extent in integer viewport coordinates.
	 */
	QRect calculateViewportBoundingBox(QRectF map_rect, int pixel_border);
	/** Internal method for setting a part of a cache as dirty. */
	void setDynamicBoundingBox(QRectF map_rect, int pixel_border, QRect& dirty_rect_old, QRectF& dirty_rect_new, int& dirty_rect_new_border, bool do_update);
	/** Internal method for removing the dirty state of a cache. */
	void clearDynamicBoundingBox(QRect& dirty_rect_old, QRectF& dirty_rect_new, int& dirty_rect_new_border);
	
	/** Moves the dirty rect by the given amount of pixels. */
	void moveDirtyRect(QRect& dirty_rect, qreal x, qreal y);
	
	/** Starts a dragging interaction at the given cursor position. */
	void startDragging(QPoint cursor_pos);
	/** Submits a new cursor position during a dragging interaction. */
	void updateDragging(QPoint cursor_pos);
	/** Ends a dragging interaction at the given cursor position. */
	void finishDragging(QPoint cursor_pos);
	/** Cancels a dragging interaction. */
	void cancelDragging();
	
	/** Starts a pinching interaction at the given cursor position.
	 *  Returns the initial zoom factor. */
	qreal startPinching(QPoint center);
	/** Updates a pinching interaction at the given cursor position. */
	void updatePinching(QPoint center, qreal factor);
	/** Ends a pinching interaction at the given cursor position. */
	void finishPinching(QPoint center, qreal factor);
	/** Cancels a pinching interaction. */
	void cancelPinching();
	
	/** Moves the map a given number of big "steps" in x and/or y direction. */
	void moveMap(int steps_x, int steps_y);
	
	/** Draws a help message at the center of the MapWidget. */
	void showHelpMessage(QPainter* painter, const QString& text) const;
	
	/** Updates the content of the zoom label, set by setZoomLabel(). */
	void updateZoomLabel();
	/** Updates the content of the cursorpos label, set by setCursorposLabel(). */
	void updateCursorposLabel(const MapCoordF pos);
	/** Updates the content of the object tag label, set by setObjectTagLabel(). */
	void updateObjectTagLabel(const MapCoordF pos);
	
	MapView* view;
	MapEditorTool* tool;
	MapEditorActivity* activity;
	
	CoordsType coords_type;
	
	QLabel* zoom_label;
	QLabel* cursorpos_label;
	QLabel* objecttag_label;
	MapCoordF last_cursor_pos;
	
	bool show_help;
	bool force_antialiasing;
	
	// Dragging (interaction)
	bool dragging;
	QPoint drag_start_pos;
	/** Cursor used when not dragging */
	QCursor normal_cursor;
	
	// Pinching (interaction)
	bool pinching;
	qreal pinching_factor;
	QPoint pinching_center;
	
	// Panning (operation)
	QPoint pan_offset;
	
	// Template caches
	/** Cache for templates below map layer */
	QImage below_template_cache;
	QRect below_template_cache_dirty_rect;
	
	/** Cache for templates above map layer */
	QImage above_template_cache;
	QRect above_template_cache_dirty_rect;
	
	/** Map layer cache  */
	QImage map_cache;
	QRect map_cache_dirty_rect;
	
	// Dirty regions for drawings (tools) and activities
	/** Dirty rect for the current tool, in viewport coordinates (pixels). */
	QRect drawing_dirty_rect;
	
	/** Dirty rect for the current tool, in map coordinates. */
	QRectF drawing_dirty_rect_map;
	
	/** Additional pixel border for the tool dirty rect, in pixels. */
	int drawing_dirty_rect_border;
	
	/** Dirty rect for the current activity, in viewport coordinates (pixels). */
	QRect activity_dirty_rect;
	
	/** Dirty rect for the current activity, in map coordinates. */
	QRectF activity_dirty_rect_map;
	
	/** Additional pixel border for the activity dirty rect, in pixels. */
	int activity_dirty_rect_border;
	
	/** Cached updates */
	QRect cached_update_rect;
	
	/** Right-click menu */
	PieMenu* context_menu;
	
	/** Optional touch cursor for mobile devices */
	QScopedPointer<TouchCursor> touch_cursor;
	
	/** For checking for interaction with the widget: the last QTime where
	 *  a mouse release event happened. Check for current_pressed_buttons == 0
	 *  and a last_mouse_release_time a given time interval in the past to check
	 *  whether the user interacts or recently interacted with the widget. */
	QTime last_mouse_release_time;
	int current_pressed_buttons;
	
	/** Optional GPS display */
	GPSDisplay* gps_display;
	/** Optional temporary GPS marker display. */
	GPSTemporaryMarkers* marker_display;
	
	/** @brief Indicates whether gesture recognition is enabled. */
	bool gestures_enabled;
};



// ### MapWidget inline code ###

inline
MapView* MapWidget::getMapView() const
{
	return view;
}

inline
bool MapWidget::gesturesEnabled() const
{
	return gestures_enabled;
}

inline
QPointF MapWidget::mapToViewport(QPointF input) const
{
	// This is a convenience method for situations when we have got a plain QPointF.
	// We rely on MapCoordF adding nothing but functions to its base, QPointF.
	static_assert(std::is_base_of<QPointF, MapCoordF>::value,
	              "MapCoordF must be derived from QPointF");
	static_assert(!std::has_virtual_destructor<MapCoordF>::value,
	              "MapCoordF and its base must not have virtual members");
	static_assert(sizeof(QPointF) == sizeof(MapCoordF),
	              "MapCoordF must have the same size as QPointF");
	return mapToViewport(static_cast<MapCoordF>(input));
}

inline
QPoint MapWidget::panOffset() const
{
	return pan_offset;
}

inline
MapWidget::CoordsType MapWidget::getCoordsDisplay() const
{
	return coords_type;
}

#endif
