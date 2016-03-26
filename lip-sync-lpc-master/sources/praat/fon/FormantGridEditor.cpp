/* FormantGridEditor.cpp
 *
 * Copyright (C) 2008-2011 Paul Boersma & David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "FormantGridEditor.h"
#include "Preferences.h"
#include "EditorM.h"
#include "PointProcess_and_Sound.h"

Thing_implement (FormantGridEditor, FunctionEditor, 0);

/********** PREFERENCES **********/

static struct {
	double formantFloor, formantCeiling, bandwidthFloor, bandwidthCeiling;
	struct FormantGridEditor_Play play;
	struct FormantGridEditor_Source source;
} preferences;

#define DEFAULT_F0_START  150.0
#define DEFAULT_T_MID  0.25
#define DEFAULT_F0_MID  180.0
#define DEFAULT_F0_END  120.0

void FormantGridEditor_prefs (void) {
	Preferences_addDouble (L"FormantGridEditor.formantFloor", & preferences.formantFloor, 0.0);   // Hz
	Preferences_addDouble (L"FormantGridEditor.formantCeiling", & preferences.formantCeiling, 11000.0);   // Hz
	Preferences_addDouble (L"FormantGridEditor.bandwidthFloor", & preferences.bandwidthFloor, 0.0);   // Hz
	Preferences_addDouble (L"FormantGridEditor.bandwidthCeiling", & preferences.bandwidthCeiling, 1000.0);   // Hz
	Preferences_addDouble (L"FormantGridEditor.play.samplingFrequency", & preferences.play.samplingFrequency, 44100.0);   // Hz
	Preferences_addDouble (L"FormantGridEditor.source.pitch.tStart", & preferences.source.pitch.tStart, 0.0);   // relative time
	Preferences_addDouble (L"FormantGridEditor.source.pitch.f0Start", & preferences.source.pitch.f0Start, DEFAULT_F0_START);   // Hz
	Preferences_addDouble (L"FormantGridEditor.source.pitch.tMid", & preferences.source.pitch.tMid, DEFAULT_T_MID);   // relative time
	Preferences_addDouble (L"FormantGridEditor.source.pitch.f0Mid", & preferences.source.pitch.f0Mid, DEFAULT_F0_MID);   // Hz
	Preferences_addDouble (L"FormantGridEditor.source.pitch.tEnd", & preferences.source.pitch.tEnd, 1.0);   // relative time
	Preferences_addDouble (L"FormantGridEditor.source.pitch.f0End", & preferences.source.pitch.f0End, DEFAULT_F0_END);   // Hz
	Preferences_addDouble (L"FormantGridEditor.source.pitch.f0End", & preferences.source.pitch.f0End, DEFAULT_F0_END);   // Hz
	Preferences_addDouble (L"FormantGridEditor.source.phonation.adaptFactor", & preferences.source.phonation.adaptFactor, PointProcess_to_Sound_phonation_DEFAULT_ADAPT_FACTOR);
	Preferences_addDouble (L"FormantGridEditor.source.phonation.maximumPeriod", & preferences.source.phonation.maximumPeriod, PointProcess_to_Sound_phonation_DEFAULT_MAXIMUM_PERIOD);
	Preferences_addDouble (L"FormantGridEditor.source.phonation.openPhase", & preferences.source.phonation.openPhase, PointProcess_to_Sound_phonation_DEFAULT_OPEN_PHASE);
	Preferences_addDouble (L"FormantGridEditor.source.phonation.collisionPhase", & preferences.source.phonation.collisionPhase, PointProcess_to_Sound_phonation_DEFAULT_COLLISION_PHASE);
	Preferences_addDouble (L"FormantGridEditor.source.phonation.power1", & preferences.source.phonation.power1, PointProcess_to_Sound_phonation_DEFAULT_POWER_1);
	Preferences_addDouble (L"FormantGridEditor.source.phonation.power2", & preferences.source.phonation.power2, PointProcess_to_Sound_phonation_DEFAULT_POWER_2);
}

/********** MENU COMMANDS **********/

static void menu_cb_removePoints (EDITOR_ARGS) {
	EDITOR_IAM (FormantGridEditor);
	Editor_save (me, L"Remove point(s)");
	FormantGrid grid = (FormantGrid) my data;
	Ordered tiers = my editingBandwidths ? grid -> bandwidths : grid -> formants;
	RealTier tier = (RealTier) tiers -> item [my selectedFormant];
	if (my startSelection == my endSelection)
		AnyTier_removePointNear (tier, my startSelection);
	else
		AnyTier_removePointsBetween (tier, my startSelection, my endSelection);
	FunctionEditor_redraw (me);
	my broadcastDataChanged ();
}

static void menu_cb_addPointAtCursor (EDITOR_ARGS) {
	EDITOR_IAM (FormantGridEditor);
	Editor_save (me, L"Add point");
	FormantGrid grid = (FormantGrid) my data;
	Ordered tiers = my editingBandwidths ? grid -> bandwidths : grid -> formants;
	RealTier tier = (RealTier) tiers -> item [my selectedFormant];
	RealTier_addPoint (tier, 0.5 * (my startSelection + my endSelection), my ycursor);
	FunctionEditor_redraw (me);
	my broadcastDataChanged ();
}

static void menu_cb_addPointAt (EDITOR_ARGS) {
	EDITOR_IAM (FormantGridEditor);
	EDITOR_FORM (L"Add point", 0)
		REAL (L"Time (s)", L"0.0")
		POSITIVE (L"Frequency (Hz)", L"200.0")
	EDITOR_OK
		SET_REAL (L"Time", 0.5 * (my startSelection + my endSelection))
		SET_REAL (L"Frequency", my ycursor)
	EDITOR_DO
		Editor_save (me, L"Add point");
		FormantGrid grid = (FormantGrid) my data;
		Ordered tiers = my editingBandwidths ? grid -> bandwidths : grid -> formants;
		RealTier tier = (RealTier) tiers -> item [my selectedFormant];
		RealTier_addPoint (tier, GET_REAL (L"Time"), GET_REAL (L"Frequency"));
		FunctionEditor_redraw (me);
		my broadcastDataChanged ();
	EDITOR_END
}

static void menu_cb_setFormantRange (EDITOR_ARGS) {
	EDITOR_IAM (FormantGridEditor);
	EDITOR_FORM (L"Set formant range", 0)
		REAL (L"Minimum formant (Hz)", L"0.0")
		REAL (L"Maximum formant (Hz)", L"11000.0")
	EDITOR_OK
		SET_REAL (L"Minimum formant", my formantFloor)
		SET_REAL (L"Maximum formant", my formantCeiling)
	EDITOR_DO
		preferences.formantFloor = my formantFloor = GET_REAL (L"Minimum formant");
		preferences.formantCeiling = my formantCeiling = GET_REAL (L"Maximum formant");
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void menu_cb_setBandwidthRange (EDITOR_ARGS) {
	EDITOR_IAM (FormantGridEditor);
	EDITOR_FORM (L"Set bandwidth range", 0)
		REAL (L"Minimum bandwidth (Hz)", L"0.0")
		REAL (L"Maximum bandwidth (Hz)", L"1000.0")
	EDITOR_OK
		SET_REAL (L"Minimum bandwidth", my bandwidthFloor)
		SET_REAL (L"Maximum bandwidth", my bandwidthCeiling)
	EDITOR_DO
		preferences.bandwidthFloor = my bandwidthFloor = GET_REAL (L"Minimum bandwidth");
		preferences.bandwidthCeiling = my bandwidthCeiling = GET_REAL (L"Maximum bandwidth");
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void menu_cb_showBandwidths (EDITOR_ARGS) {
	EDITOR_IAM (FormantGridEditor);
	my editingBandwidths = ! my editingBandwidths;
	FunctionEditor_redraw (me);
}

static void selectFormantOrBandwidth (FormantGridEditor me, long iformant) {
	FormantGrid grid = (FormantGrid) my data;
	long numberOfFormants = grid -> formants -> size;
	if (iformant > numberOfFormants)
		Melder_throw ("Cannot select formant ", iformant, ", because the FormantGrid has only ", numberOfFormants, " formants.");
	my selectedFormant = iformant;
	FunctionEditor_redraw (me);
}

static void menu_cb_selectFirst (EDITOR_ARGS) { EDITOR_IAM (FormantGridEditor); selectFormantOrBandwidth (me, 1); }
static void menu_cb_selectSecond (EDITOR_ARGS) { EDITOR_IAM (FormantGridEditor); selectFormantOrBandwidth (me, 2); }
static void menu_cb_selectThird (EDITOR_ARGS) { EDITOR_IAM (FormantGridEditor); selectFormantOrBandwidth (me, 3); }
static void menu_cb_selectFourth (EDITOR_ARGS) { EDITOR_IAM (FormantGridEditor); selectFormantOrBandwidth (me, 4); }
static void menu_cb_selectFifth (EDITOR_ARGS) { EDITOR_IAM (FormantGridEditor); selectFormantOrBandwidth (me, 5); }
static void menu_cb_selectSixth (EDITOR_ARGS) { EDITOR_IAM (FormantGridEditor); selectFormantOrBandwidth (me, 6); }
static void menu_cb_selectSeventh (EDITOR_ARGS) { EDITOR_IAM (FormantGridEditor); selectFormantOrBandwidth (me, 7); }
static void menu_cb_selectEighth (EDITOR_ARGS) { EDITOR_IAM (FormantGridEditor); selectFormantOrBandwidth (me, 8); }
static void menu_cb_selectNinth (EDITOR_ARGS) { EDITOR_IAM (FormantGridEditor); selectFormantOrBandwidth (me, 9); }
static void menu_cb_selectFormantOrBandwidth (EDITOR_ARGS) {
	EDITOR_IAM (FormantGridEditor);
	EDITOR_FORM (L"Select formant or bandwidth", 0)
		NATURAL (L"Formant number", L"1")
	EDITOR_OK
		SET_INTEGER (L"Formant number", my selectedFormant)
	EDITOR_DO
		selectFormantOrBandwidth (me, GET_INTEGER (L"Formant number"));
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void menu_cb_pitchSettings (EDITOR_ARGS) {
	EDITOR_IAM (FormantGridEditor);
	EDITOR_FORM (L"Source pitch settings", 0)
		LABEL (L"", L"These settings apply to the pitch curve")
		LABEL (L"", L"that you hear when playing the FormantGrid.")
		REAL (L"Starting time", L"0.0%")
		POSITIVE (L"Starting pitch (Hz)", L"150.0")
		REAL (L"Mid time", L"25.0%")
		POSITIVE (L"Mid pitch (Hz)", L"180.0")
		REAL (L"End time", L"100.0%")
		POSITIVE (L"End pitch (Hz)", L"120")
	EDITOR_OK
		SET_REAL (L"Starting time", my source.pitch.tStart)
		SET_REAL (L"Starting pitch", my source.pitch.f0Start)
		SET_REAL (L"Mid time", my source.pitch.tMid)
		SET_REAL (L"Mid pitch", my source.pitch.f0Mid)
		SET_REAL (L"End time", my source.pitch.tEnd)
		SET_REAL (L"End pitch", my source.pitch.f0End)
	EDITOR_DO
		preferences.source.pitch.tStart = my source.pitch.tStart = GET_REAL (L"Starting time");
		preferences.source.pitch.f0Start = my source.pitch.f0Start = GET_REAL (L"Starting pitch");
		preferences.source.pitch.tMid = my source.pitch.tMid = GET_REAL (L"Mid time");
		preferences.source.pitch.f0Mid = my source.pitch.f0Mid = GET_REAL (L"Mid pitch");
		preferences.source.pitch.tEnd = my source.pitch.tEnd = GET_REAL (L"End time");
		preferences.source.pitch.f0End = my source.pitch.f0End = GET_REAL (L"End pitch");
	EDITOR_END
}

void structFormantGridEditor :: v_createMenus () {
	FormantGridEditor_Parent :: v_createMenus ();
	EditorMenu menu = Editor_addMenu (this, L"Formant", 0);
	EditorMenu_addCommand (menu, L"Show bandwidths", GuiMenu_CHECKBUTTON + 'B', menu_cb_showBandwidths);
	EditorMenu_addCommand (menu, L"Set formant range...", 0, menu_cb_setFormantRange);
	EditorMenu_addCommand (menu, L"Set bandwidth range...", 0, menu_cb_setBandwidthRange);
	EditorMenu_addCommand (menu, L"-- select formant --", 0, NULL);
	EditorMenu_addCommand (menu, L"Select first", '1', menu_cb_selectFirst);
	EditorMenu_addCommand (menu, L"Select second", '2', menu_cb_selectSecond);
	EditorMenu_addCommand (menu, L"Select third", '3', menu_cb_selectThird);
	EditorMenu_addCommand (menu, L"Select fourth", '4', menu_cb_selectFourth);
	EditorMenu_addCommand (menu, L"Select fifth", '5', menu_cb_selectFifth);
	EditorMenu_addCommand (menu, L"Select sixth", '6', menu_cb_selectSixth);
	EditorMenu_addCommand (menu, L"Select seventh", '7', menu_cb_selectSeventh);
	EditorMenu_addCommand (menu, L"Select eighth", '8', menu_cb_selectEighth);
	EditorMenu_addCommand (menu, L"Select ninth", '9', menu_cb_selectNinth);
	EditorMenu_addCommand (menu, L"Select formant or bandwidth...", 0, menu_cb_selectFormantOrBandwidth);
	menu = Editor_addMenu (this, L"Point", 0);
	EditorMenu_addCommand (menu, L"Add point at cursor", 'T', menu_cb_addPointAtCursor);
	EditorMenu_addCommand (menu, L"Add point at...", 0, menu_cb_addPointAt);
	EditorMenu_addCommand (menu, L"-- remove point --", 0, NULL);
	EditorMenu_addCommand (menu, L"Remove point(s)", GuiMenu_OPTION + 'T', menu_cb_removePoints);
	if (v_hasSourceMenu ()) {
		menu = Editor_addMenu (this, L"Source", 0);
		EditorMenu_addCommand (menu, L"Pitch settings...", 0, menu_cb_pitchSettings);
		//EditorMenu_addCommand (menu, L"Phonation settings...", 0, menu_cb_phonationSettings);
	}
}

/********** DRAWING AREA **********/

void structFormantGridEditor :: v_draw () {
	FormantGrid grid = (FormantGrid) data;
	Ordered tiers = editingBandwidths ? grid -> bandwidths : grid -> formants;
	RealTier selectedTier = (RealTier) tiers -> item [selectedFormant];
	double ymin = editingBandwidths ? bandwidthFloor : formantFloor;
	double ymax = editingBandwidths ? bandwidthCeiling : formantCeiling;
	Graphics_setColour (graphics, Graphics_WHITE);
	Graphics_setWindow (graphics, 0, 1, 0, 1);
	Graphics_fillRectangle (graphics, 0, 1, 0, 1);
	Graphics_setWindow (graphics, startWindow, endWindow, ymin, ymax);
	Graphics_setColour (graphics, Graphics_RED);
	Graphics_line (graphics, startWindow, ycursor, endWindow, ycursor);
	Graphics_setTextAlignment (graphics, Graphics_RIGHT, Graphics_HALF);
	Graphics_text1 (graphics, startWindow, ycursor, Melder_float (Melder_half (ycursor)));
	Graphics_setColour (graphics, Graphics_BLUE);
	Graphics_setTextAlignment (graphics, Graphics_LEFT, Graphics_TOP);
	Graphics_text2 (graphics, endWindow, ymax, Melder_float (Melder_half (ymax)), L" Hz");
	Graphics_setTextAlignment (graphics, Graphics_LEFT, Graphics_HALF);
	Graphics_text2 (graphics, endWindow, ymin, Melder_float (Melder_half (ymin)), L" Hz");
	Graphics_setLineWidth (graphics, 1);
	Graphics_setColour (graphics, Graphics_GREY);
	for (long iformant = 1; iformant <= grid -> formants -> size; iformant ++) if (iformant != selectedFormant) {
		RealTier tier = (RealTier) tiers -> item [iformant];
		long imin = AnyTier_timeToHighIndex (tier, startWindow);
		long imax = AnyTier_timeToLowIndex (tier, endWindow);
		long n = tier -> points -> size;
		if (n == 0) {
		} else if (imax < imin) {
			double yleft = RealTier_getValueAtTime (tier, startWindow);
			double yright = RealTier_getValueAtTime (tier, endWindow);
			Graphics_line (graphics, startWindow, yleft, endWindow, yright);
		} else for (long i = imin; i <= imax; i ++) {
			RealPoint point = (RealPoint) tier -> points -> item [i];
			double t = point -> number, y = point -> value;
			Graphics_fillCircle_mm (graphics, t, y, 2);
			if (i == 1)
				Graphics_line (graphics, startWindow, y, t, y);
			else if (i == imin)
				Graphics_line (graphics, t, y, startWindow, RealTier_getValueAtTime (tier, startWindow));
			if (i == n)
				Graphics_line (graphics, t, y, endWindow, y);
			else if (i == imax)
				Graphics_line (graphics, t, y, endWindow, RealTier_getValueAtTime (tier, endWindow));
			else {
				RealPoint pointRight = (RealPoint) tier -> points -> item [i + 1];
				Graphics_line (graphics, t, y, pointRight -> number, pointRight -> value);
			}
		}
	}
	Graphics_setColour (graphics, Graphics_BLUE);
	long ifirstSelected = AnyTier_timeToHighIndex (selectedTier, startSelection);
	long ilastSelected = AnyTier_timeToLowIndex (selectedTier, endSelection);
	long n = selectedTier -> points -> size;
	long imin = AnyTier_timeToHighIndex (selectedTier, startWindow);
	long imax = AnyTier_timeToLowIndex (selectedTier, endWindow);
	Graphics_setLineWidth (graphics, 2);
	if (n == 0) {
		Graphics_setTextAlignment (graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (graphics, 0.5 * (startWindow + endWindow),
			0.5 * (ymin + ymax), L"(no points in selected formant tier)");
	} else if (imax < imin) {
		double yleft = RealTier_getValueAtTime (selectedTier, startWindow);
		double yright = RealTier_getValueAtTime (selectedTier, endWindow);
		Graphics_line (graphics, startWindow, yleft, endWindow, yright);
	} else for (long i = imin; i <= imax; i ++) {
		RealPoint point = (RealPoint) selectedTier -> points -> item [i];
		double t = point -> number, y = point -> value;
		if (i >= ifirstSelected && i <= ilastSelected)
			Graphics_setColour (graphics, Graphics_RED);	
		Graphics_fillCircle_mm (graphics, t, y, 3);
		Graphics_setColour (graphics, Graphics_BLUE);
		if (i == 1)
			Graphics_line (graphics, startWindow, y, t, y);
		else if (i == imin)
			Graphics_line (graphics, t, y, startWindow, RealTier_getValueAtTime (selectedTier, startWindow));
		if (i == n)
			Graphics_line (graphics, t, y, endWindow, y);
		else if (i == imax)
			Graphics_line (graphics, t, y, endWindow, RealTier_getValueAtTime (selectedTier, endWindow));
		else {
			RealPoint pointRight = (RealPoint) selectedTier -> points -> item [i + 1];
			Graphics_line (graphics, t, y, pointRight -> number, pointRight -> value);
		}
	}
	Graphics_setLineWidth (graphics, 1);
	Graphics_setColour (graphics, Graphics_BLACK);
}

static void drawWhileDragging (FormantGridEditor me, double xWC, double yWC, long first, long last, double dt, double dy) {
	FormantGrid grid = (FormantGrid) my data;
	Ordered tiers = my editingBandwidths ? grid -> bandwidths : grid -> formants;
	RealTier tier = (RealTier) tiers -> item [my selectedFormant];
	double ymin = my editingBandwidths ? my bandwidthFloor : my formantFloor;
	double ymax = my editingBandwidths ? my bandwidthCeiling : my formantCeiling;
	(void) xWC;
	(void) yWC;

	/*
	 * Draw all selected points as magenta empty circles, if inside the window.
	 */
	for (long i = first; i <= last; i ++) {
		RealPoint point = (RealPoint) tier -> points -> item [i];
		double t = point -> number + dt, y = point -> value + dy;
		if (t >= my startWindow && t <= my endWindow)
			Graphics_circle_mm (my graphics, t, y, 3);
	}

	if (last == first) {
		/*
		 * Draw a crosshair with time and y.
		 */
		RealPoint point = (RealPoint) tier -> points -> item [first];
		double t = point -> number + dt, y = point -> value + dy;
		Graphics_line (my graphics, t, ymin, t, ymax - Graphics_dyMMtoWC (my graphics, 4.0));
		Graphics_setTextAlignment (my graphics, kGraphics_horizontalAlignment_CENTRE, Graphics_TOP);
		Graphics_text1 (my graphics, t, ymax, Melder_fixed (t, 6));
		Graphics_line (my graphics, my startWindow, y, my endWindow, y);
		Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_BOTTOM);
		Graphics_text1 (my graphics, my startWindow, y, Melder_fixed (y, 6));
	}
}

int structFormantGridEditor :: v_click (double xWC, double yWC, bool shiftKeyPressed) {
	FormantGrid grid = (FormantGrid) data;
	Ordered tiers = editingBandwidths ? grid -> bandwidths : grid -> formants;
	RealTier tier = (RealTier) tiers -> item [selectedFormant];
	double ymin = editingBandwidths ? bandwidthFloor : formantFloor;
	double ymax = editingBandwidths ? bandwidthCeiling : formantCeiling;
	long inearestPoint, ifirstSelected, ilastSelected;
	RealPoint nearestPoint;
	double dt = 0, df = 0;
	int draggingSelection;

	/*
	 * Perform the default action: move cursor.
	 */
	startSelection = endSelection = xWC;
	ycursor = (1.0 - yWC) * ymin + yWC * ymax;
	Graphics_setWindow (graphics, startWindow, endWindow, ymin, ymax);
	yWC = ycursor;

	/*
	 * Clicked on a point?
	 */
	inearestPoint = AnyTier_timeToNearestIndex (tier, xWC);
	if (inearestPoint == 0) {
		return FormantGridEditor_Parent :: v_click (xWC, yWC, shiftKeyPressed);
	}
	nearestPoint = (RealPoint) tier -> points -> item [inearestPoint];
	if (Graphics_distanceWCtoMM (graphics, xWC, yWC, nearestPoint -> number, nearestPoint -> value) > 1.5) {
		return FormantGridEditor_Parent :: v_click (xWC, yWC, shiftKeyPressed);
	}

	/*
	 * Clicked on a selected point?
	 */
	draggingSelection = shiftKeyPressed &&
		nearestPoint -> number > startSelection && nearestPoint -> number < endSelection;
	if (draggingSelection) {
		ifirstSelected = AnyTier_timeToHighIndex (tier, startSelection);
		ilastSelected = AnyTier_timeToLowIndex (tier, endSelection);
		Editor_save (this, L"Drag points");
	} else {
		ifirstSelected = ilastSelected = inearestPoint;
		Editor_save (this, L"Drag point");
	}

	/*
	 * Drag.
	 */
	Graphics_xorOn (graphics, Graphics_MAROON);
	drawWhileDragging (this, xWC, yWC, ifirstSelected, ilastSelected, dt, df);
	while (Graphics_mouseStillDown (graphics)) {
		double xWC_new, yWC_new;
		Graphics_getMouseLocation (graphics, & xWC_new, & yWC_new);
		if (xWC_new != xWC || yWC_new != yWC) {
			drawWhileDragging (this, xWC, yWC, ifirstSelected, ilastSelected, dt, df);
			dt += xWC_new - xWC, df += yWC_new - yWC;
			xWC = xWC_new, yWC = yWC_new;
			drawWhileDragging (this, xWC, yWC, ifirstSelected, ilastSelected, dt, df);
		}
	}
	Graphics_xorOff (graphics);

	/*
	 * Dragged inside window?
	 */
	if (xWC < startWindow || xWC > endWindow) return 1;

	/*
	 * Points not dragged past neighbours?
	 */
	RealPoint *points = (RealPoint *) tier -> points -> item;
	double newTime = points [ifirstSelected] -> number + dt;
	if (newTime < tmin) return 1;   // outside domain
	if (ifirstSelected > 1 && newTime <= points [ifirstSelected - 1] -> number)
		return 1;   // past left neighbour
	newTime = points [ilastSelected] -> number + dt;
	if (newTime > tmax) return 1;   // outside domain
	if (ilastSelected < tier -> points -> size && newTime >= points [ilastSelected + 1] -> number)
		return 1;   // past right neighbour

	/*
	 * Drop.
	 */
	for (long i = ifirstSelected; i <= ilastSelected; i ++) {
		RealPoint point = (RealPoint) tier -> points -> item [i];
		point -> number += dt;
		point -> value += df;
	}

	/*
	 * Make sure that the same points are still selected (a problem with Undo...).
	 */

	if (draggingSelection) startSelection += dt, endSelection += dt;
	if (ifirstSelected == ilastSelected) {
		/*
		 * Move crosshair to only selected formant point.
		 */
		RealPoint point = (RealPoint) tier -> points -> item [ifirstSelected];
		startSelection = endSelection = point -> number;
		ycursor = point -> value;
	} else {
		/*
		 * Move crosshair to mouse location.
		 */
		/*cursor += dt;*/
		ycursor += df;
	}

	broadcastDataChanged ();
	return 1;   // update needed
}

void structFormantGridEditor :: v_play (double tmin, double tmax) {
	FormantGrid_playPart ((FormantGrid) data, tmin, tmax, play.samplingFrequency,
		source.pitch.tStart, source.pitch.f0Start,
		source.pitch.tMid, source.pitch.f0Mid,
		source.pitch.tEnd, source.pitch.f0End,
		source.phonation.adaptFactor, source.phonation.maximumPeriod,
		source.phonation.openPhase, source.phonation.collisionPhase,
		source.phonation.power1, source.phonation.power2,
		theFunctionEditor_playCallback, this);
}

void FormantGridEditor_init (FormantGridEditor me, GuiObject parent, const wchar *title, FormantGrid data) {
	Melder_assert (data != NULL);
	Melder_assert (Thing_member (data, classFormantGrid));
	FunctionEditor_init (me, parent, title, data);
	my formantFloor = preferences.formantFloor;
	my formantCeiling = preferences.formantCeiling;
	my bandwidthFloor = preferences.bandwidthFloor;
	my bandwidthCeiling = preferences.bandwidthCeiling;
	my play = preferences.play;
	my source = preferences.source;
	my ycursor = 0.382 * my formantFloor + 0.618 * my formantCeiling;
	my selectedFormant = 1;
}

FormantGridEditor FormantGridEditor_create (GuiObject parent, const wchar *title, FormantGrid data) {
	try {
		autoFormantGridEditor me = Thing_new (FormantGridEditor);
		FormantGridEditor_init (me.peek(), parent, title, data);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("FormantGrid window not created.");
	}
}

/* End of file FormantGridEditor.cpp */
