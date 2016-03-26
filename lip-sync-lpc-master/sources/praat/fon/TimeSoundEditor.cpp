/* TimeSoundEditor.cpp
 *
 * Copyright (C) 1992-2011,2012 Paul Boersma
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

#include "TimeSoundEditor.h"
#include "Preferences.h"
#include "EditorM.h"
#include "UnicodeData.h"

Thing_implement (TimeSoundEditor, FunctionEditor, 0);

/********** PREFERENCES **********/

static struct {
	struct TimeSoundEditor_sound sound;
	struct {
		bool preserveTimes;
		double bottom, top;
		bool garnish;
	} picture;
	struct {
		enum kSound_windowShape windowShape;
		double relativeWidth;
		bool preserveTimes;
	} extract;
}
	preferences;

void TimeSoundEditor_prefs (void) {
	Preferences_addBool (L"TimeSoundEditor.sound.autoscaling", & preferences.sound.autoscaling, true);
	Preferences_addBool (L"TimeSoundEditor.picture.preserveTimes", & preferences.picture.preserveTimes, true);
	Preferences_addDouble (L"TimeSoundEditor.picture.bottom", & preferences.picture.bottom, 0.0);
	Preferences_addDouble (L"TimeSoundEditor.picture.top", & preferences.picture.top, 0.0);
	Preferences_addBool (L"TimeSoundEditor.picture.garnish", & preferences.picture.garnish, true);
	Preferences_addEnum (L"TimeSoundEditor.extract.windowShape", & preferences.extract.windowShape, kSound_windowShape, DEFAULT);
	Preferences_addDouble (L"TimeSoundEditor.extract.relativeWidth", & preferences.extract.relativeWidth, 1.0);
	Preferences_addBool (L"TimeSoundEditor.extract.preserveTimes", & preferences.extract.preserveTimes, true);
}

/********** Thing methods **********/

void structTimeSoundEditor :: v_destroy () {
	if (d_ownSound)
		forget (d_sound.data);
	TimeSoundEditor_Parent :: v_destroy ();
}

void structTimeSoundEditor :: v_info () {
	TimeSoundEditor_Parent :: v_info ();
	/* Sound flag: */
	MelderInfo_writeLine2 (L"Sound autoscaling: ", Melder_boolean (d_sound.autoscaling));
}

/***** FILE MENU *****/

static void menu_cb_DrawVisibleSound (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM (L"Draw visible sound", 0)
		my v_form_pictureWindow (cmd);
		LABEL (L"", L"Sound:")
		BOOLEAN (L"Preserve times", 1);
		REAL (L"left Vertical range", L"0.0")
		REAL (L"right Vertical range", L"0.0 (= auto)")
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (L"Garnish", 1);
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		SET_INTEGER (L"Preserve times", preferences.picture.preserveTimes);
		SET_REAL (L"left Vertical range", preferences.picture.bottom);
		SET_REAL (L"right Vertical range", preferences.picture.top);
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_INTEGER (L"Garnish", preferences.picture.garnish);
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		preferences.picture.preserveTimes = GET_INTEGER (L"Preserve times");
		preferences.picture.bottom = GET_REAL (L"left Vertical range");
		preferences.picture.top = GET_REAL (L"right Vertical range");
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		preferences.picture.garnish = GET_INTEGER (L"Garnish");
		if (my d_longSound.data == NULL && my d_sound.data == NULL)
			Melder_throw ("There is no sound to draw.");
		autoSound publish = my d_longSound.data ?
			LongSound_extractPart (my d_longSound.data, my startWindow, my endWindow, preferences.picture.preserveTimes) :
			Sound_extractPart (my d_sound.data, my startWindow, my endWindow, kSound_windowShape_RECTANGULAR, 1.0, preferences.picture.preserveTimes);
		Editor_openPraatPicture (me);
		Sound_draw (publish.peek(), my pictureGraphics, 0.0, 0.0, preferences.picture.bottom, preferences.picture.top,
			preferences.picture.garnish, L"Curve");
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

static void menu_cb_DrawSelectedSound (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM (L"Draw selected sound", 0)
		my v_form_pictureWindow (cmd);
		LABEL (L"", L"Sound:")
		BOOLEAN (L"Preserve times", 1);
		REAL (L"left Vertical range", L"0.0")
		REAL (L"right Vertical range", L"0.0 (= auto)")
		my v_form_pictureMargins (cmd);
		BOOLEAN (L"Garnish", 1);
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		SET_INTEGER (L"Preserve times", preferences.picture.preserveTimes);
		SET_REAL (L"left Vertical range", preferences.picture.bottom);
		SET_REAL (L"right Vertical range", preferences.picture.top);
		my v_ok_pictureMargins (cmd);
		SET_INTEGER (L"Garnish", preferences.picture.garnish);
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		preferences.picture.preserveTimes = GET_INTEGER (L"Preserve times");
		preferences.picture.bottom = GET_REAL (L"left Vertical range");
		preferences.picture.top = GET_REAL (L"right Vertical range");
		my v_do_pictureMargins (cmd);
		preferences.picture.garnish = GET_INTEGER (L"Garnish");
		if (my d_longSound.data == NULL && my d_sound.data == NULL)
			Melder_throw ("There is no sound to draw.");
		autoSound publish = my d_longSound.data ?
			LongSound_extractPart (my d_longSound.data, my startSelection, my endSelection, preferences.picture.preserveTimes) :
			Sound_extractPart (my d_sound.data, my startSelection, my endSelection, kSound_windowShape_RECTANGULAR, 1.0, preferences.picture.preserveTimes);
		Editor_openPraatPicture (me);
		Sound_draw (publish.peek(), my pictureGraphics, 0.0, 0.0, preferences.picture.bottom, preferences.picture.top,
			preferences.picture.garnish, L"Curve");
		Editor_closePraatPicture (me);
	EDITOR_END
}

static void do_ExtractSelectedSound (TimeSoundEditor me, bool preserveTimes) {
	autoSound extract = NULL;
	if (my endSelection <= my startSelection)
		Melder_throw ("No selection.");
	if (my d_longSound.data) {
		extract.reset (LongSound_extractPart (my d_longSound.data, my startSelection, my endSelection, preserveTimes));
	} else if (my d_sound.data) {
		extract.reset (Sound_extractPart (my d_sound.data, my startSelection, my endSelection, kSound_windowShape_RECTANGULAR, 1.0, preserveTimes));
	}
	my broadcastPublication (extract.transfer());
}

static void menu_cb_ExtractSelectedSound_timeFromZero (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	do_ExtractSelectedSound (me, FALSE);
}

static void menu_cb_ExtractSelectedSound_preserveTimes (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	do_ExtractSelectedSound (me, TRUE);
}

static void menu_cb_ExtractSelectedSound_windowed (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM (L"Extract selected sound (windowed)", 0)
		WORD (L"Name", L"slice")
		OPTIONMENU_ENUM (L"Window shape", kSound_windowShape, HANNING)
		POSITIVE (L"Relative width", L"1.0")
		BOOLEAN (L"Preserve times", 1)
	EDITOR_OK
		SET_ENUM (L"Window shape", kSound_windowShape, preferences.extract.windowShape)
		SET_REAL (L"Relative width", preferences.extract.relativeWidth)
		SET_INTEGER (L"Preserve times", preferences.extract.preserveTimes)
	EDITOR_DO
		Sound sound = my d_sound.data;
		Melder_assert (sound != NULL);
		preferences.extract.windowShape = GET_ENUM (kSound_windowShape, L"Window shape");
		preferences.extract.relativeWidth = GET_REAL (L"Relative width");
		preferences.extract.preserveTimes = GET_INTEGER (L"Preserve times");
		autoSound extract = Sound_extractPart (sound, my startSelection, my endSelection, preferences.extract.windowShape,
			preferences.extract.relativeWidth, preferences.extract.preserveTimes);
		Thing_setName (extract.peek(), GET_STRING (L"Name"));
		my broadcastPublication (extract.transfer());
	EDITOR_END
}

static void do_write (TimeSoundEditor me, MelderFile file, int format, int numberOfBitsPersamplePoint) {
	if (my startSelection >= my endSelection)
		Melder_throw ("No samples selected.");
	if (my d_longSound.data) {
		LongSound_writePartToAudioFile (my d_longSound.data, format, my startSelection, my endSelection, file);
	} else if (my d_sound.data) {
		Sound sound = my d_sound.data;
		double margin = 0.0;
		long nmargin = margin / sound -> dx;
		long first, last, numberOfSamples = Sampled_getWindowSamples (sound,
			my startSelection, my endSelection, & first, & last) + nmargin * 2;
		first -= nmargin;
		last += nmargin;
		if (numberOfSamples) {
			autoSound save = Sound_create (sound -> ny, 0.0, numberOfSamples * sound -> dx, numberOfSamples, sound -> dx, 0.5 * sound -> dx);
			long offset = first - 1;
			if (first < 1) first = 1;
			if (last > sound -> nx) last = sound -> nx;
			for (long channel = 1; channel <= sound -> ny; channel ++) {
				for (long i = first; i <= last; i ++) {
					save -> z [channel] [i - offset] = sound -> z [channel] [i];
				}
			}
			Sound_writeToAudioFile (save.peek(), file, format, numberOfBitsPersamplePoint);
		}
	}
}

static void menu_cb_WriteWav (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM_WRITE (L"Save selected sound as WAV file", 0)
		swprintf (defaultName, 300, L"%ls.wav", my d_longSound.data ? my d_longSound.data -> name : my d_sound.data -> name);
	EDITOR_DO_WRITE
		do_write (me, file, Melder_WAV, 16);
	EDITOR_END
}

static void menu_cb_SaveAs24BitWav (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM_WRITE (L"Save selected sound as 24-bit WAV file", 0)
		Melder_assert (my d_longSound.data == NULL && my d_sound.data != NULL);
		swprintf (defaultName, 300, L"%ls.wav", my d_sound.data -> name);
	EDITOR_DO_WRITE
		do_write (me, file, Melder_WAV, 24);
	EDITOR_END
}

static void menu_cb_SaveAs32BitWav (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM_WRITE (L"Save selected sound as 32-bit WAV file", 0)
		Melder_assert (my d_longSound.data == NULL && my d_sound.data != NULL);
		swprintf (defaultName, 300, L"%ls.wav", my d_sound.data -> name);
	EDITOR_DO_WRITE
		do_write (me, file, Melder_WAV, 32);
	EDITOR_END
}

static void menu_cb_WriteAiff (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM_WRITE (L"Save selected sound as AIFF file", 0)
		swprintf (defaultName, 300, L"%ls.aiff", my d_longSound.data ? my d_longSound.data -> name : my d_sound.data -> name);
	EDITOR_DO_WRITE
		do_write (me, file, Melder_AIFF, 16);
	EDITOR_END
}

static void menu_cb_WriteAifc (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM_WRITE (L"Save selected sound as AIFC file", 0)
		swprintf (defaultName, 300, L"%ls.aifc", my d_longSound.data ? my d_longSound.data -> name : my d_sound.data -> name);
	EDITOR_DO_WRITE
		do_write (me, file, Melder_AIFC, 16);
	EDITOR_END
}

static void menu_cb_WriteNextSun (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM_WRITE (L"Save selected sound as NeXT/Sun file", 0)
		swprintf (defaultName, 300, L"%ls.au", my d_longSound.data ? my d_longSound.data -> name : my d_sound.data -> name);
	EDITOR_DO_WRITE
		do_write (me, file, Melder_NEXT_SUN, 16);
	EDITOR_END
}

static void menu_cb_WriteNist (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM_WRITE (L"Save selected sound as NIST file", 0)
		swprintf (defaultName, 300, L"%ls.nist", my d_longSound.data ? my d_longSound.data -> name : my d_sound.data -> name);
	EDITOR_DO_WRITE
		do_write (me, file, Melder_NIST, 16);
	EDITOR_END
}

static void menu_cb_WriteFlac (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM_WRITE (L"Save selected sound as FLAC file", 0)
		swprintf (defaultName, 300, L"%ls.flac", my d_longSound.data ? my d_longSound.data -> name : my d_sound.data -> name);
	EDITOR_DO_WRITE
		do_write (me, file, Melder_FLAC, 16);
	EDITOR_END
}

void structTimeSoundEditor :: v_createMenuItems_file_draw (EditorMenu menu) {
	EditorMenu_addCommand (menu, L"Draw to picture window:", GuiMenu_INSENSITIVE, menu_cb_DrawVisibleSound /* dummy */);
	if (d_sound.data || d_longSound.data) {
		EditorMenu_addCommand (menu, L"Draw visible sound...", 0, menu_cb_DrawVisibleSound);
		drawButton = EditorMenu_addCommand (menu, L"Draw selected sound...", 0, menu_cb_DrawSelectedSound);
	}
}

void structTimeSoundEditor :: v_createMenuItems_file_extract (EditorMenu menu) {
	EditorMenu_addCommand (menu, L"Extract to objects window:", GuiMenu_INSENSITIVE, menu_cb_ExtractSelectedSound_preserveTimes /* dummy */);
	if (d_sound.data || d_longSound.data) {
		publishPreserveButton = EditorMenu_addCommand (menu, L"Extract selected sound (preserve times)", 0, menu_cb_ExtractSelectedSound_preserveTimes);
			EditorMenu_addCommand (menu, L"Extract sound selection (preserve times)", Editor_HIDDEN, menu_cb_ExtractSelectedSound_preserveTimes);
			EditorMenu_addCommand (menu, L"Extract selection (preserve times)", Editor_HIDDEN, menu_cb_ExtractSelectedSound_preserveTimes);
		publishButton = EditorMenu_addCommand (menu, L"Extract selected sound (time from 0)", 0, menu_cb_ExtractSelectedSound_timeFromZero);
			EditorMenu_addCommand (menu, L"Extract sound selection (time from 0)", Editor_HIDDEN, menu_cb_ExtractSelectedSound_timeFromZero);
			EditorMenu_addCommand (menu, L"Extract selection (time from 0)", Editor_HIDDEN, menu_cb_ExtractSelectedSound_timeFromZero);
			EditorMenu_addCommand (menu, L"Extract selection", Editor_HIDDEN, menu_cb_ExtractSelectedSound_timeFromZero);
		if (d_sound.data) {
			publishWindowButton = EditorMenu_addCommand (menu, L"Extract selected sound (windowed)...", 0, menu_cb_ExtractSelectedSound_windowed);
				EditorMenu_addCommand (menu, L"Extract windowed sound selection...", Editor_HIDDEN, menu_cb_ExtractSelectedSound_windowed);
				EditorMenu_addCommand (menu, L"Extract windowed selection...", Editor_HIDDEN, menu_cb_ExtractSelectedSound_windowed);
		}
	}
}

void structTimeSoundEditor :: v_createMenuItems_file_write (EditorMenu menu) {
	EditorMenu_addCommand (menu, L"Save to disk:", GuiMenu_INSENSITIVE, menu_cb_WriteWav /* dummy */);
	if (d_sound.data || d_longSound.data) {
		writeWavButton = EditorMenu_addCommand (menu, L"Save selected sound as WAV file...", 0, menu_cb_WriteWav);
			EditorMenu_addCommand (menu, L"Write selected sound to WAV file...", Editor_HIDDEN, menu_cb_WriteWav);
			EditorMenu_addCommand (menu, L"Write sound selection to WAV file...", Editor_HIDDEN, menu_cb_WriteWav);
			EditorMenu_addCommand (menu, L"Write selection to WAV file...", Editor_HIDDEN, menu_cb_WriteWav);
		if (d_sound.data) {
			d_saveAs24BitWavButton = EditorMenu_addCommand (menu, L"Save selected sound as 24-bit WAV file...", 0, menu_cb_SaveAs24BitWav);
			d_saveAs32BitWavButton = EditorMenu_addCommand (menu, L"Save selected sound as 32-bit WAV file...", 0, menu_cb_SaveAs32BitWav);
		}
		writeAiffButton = EditorMenu_addCommand (menu, L"Save selected sound as AIFF file...", 0, menu_cb_WriteAiff);
			EditorMenu_addCommand (menu, L"Write selected sound to AIFF file...", Editor_HIDDEN, menu_cb_WriteAiff);
			EditorMenu_addCommand (menu, L"Write sound selection to AIFF file...", Editor_HIDDEN, menu_cb_WriteAiff);
			EditorMenu_addCommand (menu, L"Write selection to AIFF file...", Editor_HIDDEN, menu_cb_WriteAiff);
		writeAifcButton = EditorMenu_addCommand (menu, L"Save selected sound as AIFC file...", 0, menu_cb_WriteAifc);
			EditorMenu_addCommand (menu, L"Write selected sound to AIFC file...", Editor_HIDDEN, menu_cb_WriteAifc);
			EditorMenu_addCommand (menu, L"Write sound selection to AIFC file...", Editor_HIDDEN, menu_cb_WriteAifc);
			EditorMenu_addCommand (menu, L"Write selection to AIFC file...", Editor_HIDDEN, menu_cb_WriteAifc);
		writeNextSunButton = EditorMenu_addCommand (menu, L"Save selected sound as Next/Sun file...", 0, menu_cb_WriteNextSun);
			EditorMenu_addCommand (menu, L"Write selected sound to Next/Sun file...", Editor_HIDDEN, menu_cb_WriteNextSun);
			EditorMenu_addCommand (menu, L"Write sound selection to Next/Sun file...", Editor_HIDDEN, menu_cb_WriteNextSun);
			EditorMenu_addCommand (menu, L"Write selection to Next/Sun file...", Editor_HIDDEN, menu_cb_WriteNextSun);
		writeNistButton = EditorMenu_addCommand (menu, L"Save selected sound as NIST file...", 0, menu_cb_WriteNist);
			EditorMenu_addCommand (menu, L"Write selected sound to NIST file...", Editor_HIDDEN, menu_cb_WriteNist);
			EditorMenu_addCommand (menu, L"Write sound selection to NIST file...", Editor_HIDDEN, menu_cb_WriteNist);
			EditorMenu_addCommand (menu, L"Write selection to NIST file...", Editor_HIDDEN, menu_cb_WriteNist);
		writeFlacButton = EditorMenu_addCommand (menu, L"Save selected sound as FLAC file...", 0, menu_cb_WriteFlac);
			EditorMenu_addCommand (menu, L"Write selected sound to FLAC file...", Editor_HIDDEN, menu_cb_WriteFlac);
			EditorMenu_addCommand (menu, L"Write sound selection to FLAC file...", Editor_HIDDEN, menu_cb_WriteFlac);
	}
}

void structTimeSoundEditor :: v_createMenuItems_file (EditorMenu menu) {
	TimeSoundEditor_Parent :: v_createMenuItems_file (menu);
	v_createMenuItems_file_draw (menu);
	EditorMenu_addCommand (menu, L"-- after file draw --", 0, NULL);
	v_createMenuItems_file_extract (menu);
	EditorMenu_addCommand (menu, L"-- after file extract --", 0, NULL);
	v_createMenuItems_file_write (menu);
	EditorMenu_addCommand (menu, L"-- after file write --", 0, NULL);
}

/********** QUERY MENU **********/

static void menu_cb_SoundInfo (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	Thing_info (my d_sound.data);
}

static void menu_cb_LongSoundInfo (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	Thing_info (my d_longSound.data);
}

void structTimeSoundEditor :: v_createMenuItems_query_info (EditorMenu menu) {
	TimeSoundEditor_Parent :: v_createMenuItems_query_info (menu);
	if (d_sound.data != NULL && d_sound.data != data) {
		EditorMenu_addCommand (menu, L"Sound info", 0, menu_cb_SoundInfo);
	} else if (d_longSound.data != NULL && d_longSound.data != data) {
		EditorMenu_addCommand (menu, L"LongSound info", 0, menu_cb_LongSoundInfo);
	}
}

/********** VIEW MENU **********/

static void menu_cb_autoscaling (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	preferences.sound.autoscaling = my d_sound.autoscaling = ! my d_sound.autoscaling;
	FunctionEditor_redraw (me);
}

void structTimeSoundEditor :: v_createMenuItems_view (EditorMenu menu) {
	if (d_sound.data || d_longSound.data)
		v_createMenuItems_view_sound (menu);
	TimeSoundEditor_Parent :: v_createMenuItems_view (menu);
}

void structTimeSoundEditor :: v_createMenuItems_view_sound (EditorMenu menu) {
	EditorMenu_addCommand (menu, L"Sound autoscaling", GuiMenu_CHECKBUTTON | (preferences.sound.autoscaling ? GuiMenu_TOGGLE_ON : 0), menu_cb_autoscaling);
	EditorMenu_addCommand (menu, L"-- sound view --", 0, 0);
}

void structTimeSoundEditor :: v_updateMenuItems_file () {
	Sampled sound = d_sound.data != NULL ? (Sampled) d_sound.data : (Sampled) d_longSound.data;
	if (sound == NULL) return;
	long first, last, selectedSamples = Sampled_getWindowSamples (sound, startSelection, endSelection, & first, & last);
	if (drawButton) {
		GuiObject_setSensitive (drawButton, selectedSamples != 0);
		GuiObject_setSensitive (publishButton, selectedSamples != 0);
		GuiObject_setSensitive (publishPreserveButton, selectedSamples != 0);
		if (publishWindowButton) GuiObject_setSensitive (publishWindowButton, selectedSamples != 0);
	}
	GuiObject_setSensitive (writeWavButton, selectedSamples != 0);
	if (d_saveAs24BitWavButton)
		GuiObject_setSensitive (d_saveAs24BitWavButton, selectedSamples != 0);
	if (d_saveAs32BitWavButton)
		GuiObject_setSensitive (d_saveAs32BitWavButton, selectedSamples != 0);
	GuiObject_setSensitive (writeAiffButton, selectedSamples != 0);
	GuiObject_setSensitive (writeAifcButton, selectedSamples != 0);
	GuiObject_setSensitive (writeNextSunButton, selectedSamples != 0);
	GuiObject_setSensitive (writeNistButton, selectedSamples != 0);
	GuiObject_setSensitive (writeFlacButton, selectedSamples != 0);
}

void TimeSoundEditor_draw_sound (TimeSoundEditor me, double globalMinimum, double globalMaximum) {
	Sound sound = my d_sound.data;
	LongSound longSound = my d_longSound.data;
	Melder_assert ((sound == NULL) != (longSound == NULL));
	int nchan = sound ? sound -> ny : longSound -> numberOfChannels;
	int cursorVisible = my startSelection == my endSelection && my startSelection >= my startWindow && my startSelection <= my endWindow;
	Graphics_setColour (my graphics, Graphics_BLACK);
	int fits;
	try {
		fits = sound ? TRUE : LongSound_haveWindow (longSound, my startWindow, my endWindow);
	} catch (MelderError) {
		int outOfMemory = wcsstr (Melder_getError (), L"memory") != NULL;
		if (Melder_debug == 9) Melder_flushError (NULL); else Melder_clearError ();
		Graphics_setWindow (my graphics, 0, 1, 0, 1);
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (my graphics, 0.5, 0.5, outOfMemory ? L"(out of memory)" : L"(cannot read sound file)");
		return;
	}
	if (! fits) {
		Graphics_setWindow (my graphics, 0, 1, 0, 1);
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (my graphics, 0.5, 0.5, L"(window too large; zoom in to see the data)");
		return;
	}
	long first, last;
	if (Sampled_getWindowSamples (sound ? (Sampled) sound : (Sampled) longSound, my startWindow, my endWindow, & first, & last) <= 1) {
		Graphics_setWindow (my graphics, 0, 1, 0, 1);
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (my graphics, 0.5, 0.5, L"(zoom out to see the data)");
		return;
	}
	const int numberOfVisibleChannels = nchan > 8 ? 8 : nchan;
	const int firstVisibleChannel = my d_sound.channelOffset + 1;
	int lastVisibleChannel = my d_sound.channelOffset + numberOfVisibleChannels;
	if (lastVisibleChannel > nchan) lastVisibleChannel = nchan;
	double maximumExtent = 0.0;
	if (my d_sound.autoscaling && nchan > 2) {
		double visibleChannelMinimum, visibleChannelMaximum;
		if (longSound)
			LongSound_getWindowExtrema (longSound, my startWindow, my endWindow, firstVisibleChannel, & visibleChannelMinimum, & visibleChannelMaximum);
		else
			Matrix_getWindowExtrema (sound, first, last, firstVisibleChannel, firstVisibleChannel, & visibleChannelMinimum, & visibleChannelMaximum);
		maximumExtent = visibleChannelMaximum - visibleChannelMinimum;
		for (int ichan = firstVisibleChannel + 1; ichan <= lastVisibleChannel; ichan ++) {
			if (longSound)
				LongSound_getWindowExtrema (longSound, my startWindow, my endWindow, ichan, & visibleChannelMinimum, & visibleChannelMaximum);
			else
				Matrix_getWindowExtrema (sound, first, last, ichan, ichan, & visibleChannelMinimum, & visibleChannelMaximum);
			double channelExtent = visibleChannelMaximum - visibleChannelMinimum;
			if (channelExtent > maximumExtent)
				maximumExtent = channelExtent;
		}
	}
	for (int ichan = firstVisibleChannel; ichan <= lastVisibleChannel; ichan ++) {
		double cursorFunctionValue = longSound ? 0.0 :
			Vector_getValueAtX (sound, 0.5 * (my startSelection + my endSelection), ichan, 70);
		/*
		 * BUG: this will only work for mono or stereo, until Graphics_function16 handles quadro.
		 */
		double ymin = (double) (numberOfVisibleChannels - ichan + my d_sound.channelOffset) / numberOfVisibleChannels;
		double ymax = (double) (numberOfVisibleChannels + 1 - ichan + my d_sound.channelOffset) / numberOfVisibleChannels;
		Graphics_Viewport vp = Graphics_insetViewport (my graphics, 0, 1, ymin, ymax);
		bool horizontal = false;
		double minimum = sound ? globalMinimum : -1.0, maximum = sound ? globalMaximum : 1.0;
		if (my d_sound.autoscaling) {
			if (longSound) {
				LongSound_getWindowExtrema (longSound, my startWindow, my endWindow, ichan, & minimum, & maximum);
			} else {
				Matrix_getWindowExtrema (sound, first, last, ichan, ichan, & minimum, & maximum);
			}
			if (maximumExtent > 0.0) {
				double middle = 0.5 * (minimum + maximum);
				minimum = middle - 0.5 * maximumExtent;
				maximum = middle + 0.5 * maximumExtent;
			}
		}
		if (minimum == maximum) { horizontal = true; minimum -= 1; maximum += 1;}
		Graphics_setWindow (my graphics, my startWindow, my endWindow, minimum, maximum);
		if (horizontal) {
			Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_HALF);
			double mid = 0.5 * (minimum + maximum);
			Graphics_text1 (my graphics, my startWindow, mid, Melder_float (Melder_half (mid)));
		} else {
			if (! cursorVisible || Graphics_dyWCtoMM (my graphics, cursorFunctionValue - minimum) > 5.0) {
				Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_BOTTOM);
				Graphics_text1 (my graphics, my startWindow, minimum, Melder_float (Melder_half (minimum)));
			}
			if (! cursorVisible || Graphics_dyWCtoMM (my graphics, maximum - cursorFunctionValue) > 5.0) {
				Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_TOP);
				Graphics_text1 (my graphics, my startWindow, maximum, Melder_float (Melder_half (maximum)));
			}
		}
		if (minimum < 0 && maximum > 0 && ! horizontal) {
			Graphics_setWindow (my graphics, 0, 1, minimum, maximum);
			if (! cursorVisible || fabs (Graphics_dyWCtoMM (my graphics, cursorFunctionValue - 0.0)) > 3.0) {
				Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_HALF);
				Graphics_text (my graphics, 0, 0, L"0");
			}
			Graphics_setColour (my graphics, Graphics_CYAN);
			Graphics_setLineType (my graphics, Graphics_DOTTED);
			Graphics_line (my graphics, 0, 0, 1, 0);
			Graphics_setLineType (my graphics, Graphics_DRAWN);
		}
		/*
		 * Garnish the drawing area of each channel.
		 */
		Graphics_setWindow (my graphics, 0, 1, 0, 1);
		Graphics_setColour (my graphics, Graphics_CYAN);
		Graphics_innerRectangle (my graphics, 0, 1, 0, 1);
		Graphics_setColour (my graphics, Graphics_BLACK);
		if (nchan > 1) {
			Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_HALF);
			static MelderString channelLabel;
			const wchar *channelName = my v_getChannelName (ichan);
			MelderString_copy (& channelLabel, channelName ? L"ch" : L"Channel ");
			MelderString_append (& channelLabel, Melder_integer (ichan));
			if (channelName)
				MelderString_append (& channelLabel, L": ", channelName);
			if (ichan > 8 && ichan - my d_sound.channelOffset == 1) {
				MelderString_append (& channelLabel, L" " UNITEXT_UPWARDS_ARROW);
			} else if (ichan >= 8 && ichan - my d_sound.channelOffset == 8 && ichan < nchan) {
				MelderString_append (& channelLabel, L" " UNITEXT_DOWNWARDS_ARROW);
			}
			Graphics_text1 (my graphics, 1, 0.5, channelLabel.string);
		}
		/*
		 * Draw a very thin separator line underneath.
		 */
		if (ichan < nchan) {
			/*Graphics_setColour (my graphics, Graphics_BLACK);*/
			Graphics_line (my graphics, 0, 0, 1, 0);
		}
		/*
		 * Draw the samples.
		 */
		/*if (ichan == 1) FunctionEditor_SoundAnalysis_drawPulses (me);*/
		if (sound) {
			Graphics_setWindow (my graphics, my startWindow, my endWindow, minimum, maximum);
			if (cursorVisible)
				FunctionEditor_drawCursorFunctionValue (me, cursorFunctionValue, Melder_float (Melder_half (cursorFunctionValue)), L"");
			Graphics_setColour (my graphics, Graphics_BLACK);
			Graphics_function (my graphics, sound -> z [ichan], first, last,
				Sampled_indexToX (sound, first), Sampled_indexToX (sound, last));
		} else {
			Graphics_setWindow (my graphics, my startWindow, my endWindow, minimum * 32768, maximum * 32768);
			Graphics_function16 (my graphics,
				longSound -> buffer - longSound -> imin * nchan + (ichan - 1), nchan - 1, first, last,
				Sampled_indexToX (longSound, first), Sampled_indexToX (longSound, last));
		}
		Graphics_resetViewport (my graphics, vp);
	}
	Graphics_setWindow (my graphics, 0, 1, 0, 1);
	Graphics_rectangle (my graphics, 0, 1, 0, 1);
}

int structTimeSoundEditor :: v_click (double xbegin, double ybegin, bool shiftKeyPressed) {
	Sound sound = d_sound.data;
	LongSound longSound = d_longSound.data;
	if ((sound == NULL) != (longSound == NULL)) {
		ybegin = (ybegin - v_getBottomOfSoundArea ()) / (1.0 - v_getBottomOfSoundArea ());
		int nchan = sound ? sound -> ny : longSound -> numberOfChannels;
		if (nchan > 8) {
			//Melder_casual ("%f %f %d %d", xbegin, ybegin, (int) nchan, (int) this -> sound.channelOffset);
			if (xbegin >= endWindow && ybegin > 0.875 && ybegin <= 1.000 && d_sound.channelOffset > 0) {
				d_sound.channelOffset -= 8;
				return 1;
			}
			if (xbegin >= endWindow && ybegin > 0.000 && ybegin <= 0.125 && d_sound.channelOffset < nchan - 8) {
				d_sound.channelOffset += 8;
				return 1;
			}
		}
	}
	return TimeSoundEditor_Parent :: v_click (xbegin, ybegin, shiftKeyPressed);
}

void TimeSoundEditor_init (TimeSoundEditor me, GuiObject parent, const wchar *title, Function data, Sampled sound, bool ownSound) {
	my d_ownSound = ownSound;
	if (sound != NULL) {
		if (ownSound) {
			Melder_assert (Thing_member (sound, classSound));
			my d_sound.data = Data_copy ((Sound) sound);   // deep copy; ownership transferred
			Matrix_getWindowExtrema (sound, 1, my d_sound.data -> nx, 1, my d_sound.data -> ny, & my d_sound.minimum, & my d_sound.maximum);
		} else if (Thing_member (sound, classSound)) {
			my d_sound.data = (Sound) sound;   // reference copy; ownership not transferred
			Matrix_getWindowExtrema (sound, 1, my d_sound.data -> nx, 1, my d_sound.data -> ny, & my d_sound.minimum, & my d_sound.maximum);
		} else if (Thing_member (sound, classLongSound)) {
			my d_longSound.data = (LongSound) sound;
			my d_sound.minimum = -1.0, my d_sound.maximum = 1.0;
		} else {
			Melder_fatal ("Invalid sound class in TimeSoundEditor_init.");
		}
	}
	FunctionEditor_init (me, parent, title, data);
	my d_sound.autoscaling = preferences.sound.autoscaling;
}

/* End of file TimeSoundEditor.cpp */
