/* GuiObject.cpp
 *
 * Copyright (C) 1993-2011,2012 Paul Boersma
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

/*
 * pb 2007/12/26 abstraction from motif
 * pb 2007/12/28 _GuiObject_position: allow the combination of fixed height and automatic position
 * sdk 2008/03/24 GTK
 * sdk 2008/07/01 GTK get sizes
 * fb 2010/02/23 GTK
 * pb 2010/11/28 removed Motif
 * pb 2011/04/06 C++
 */

#include "GuiP.h"
#include "machine.h"

static int _Gui_defaultHeight (GuiObject me) {
	#if win || mac && useCarbon
		WidgetClass klas = XtClass (me);
		if (klas == xmLabelWidgetClass) return Gui_LABEL_HEIGHT;
		if (klas == xmPushButtonWidgetClass) return Gui_PUSHBUTTON_HEIGHT;
		if (klas == xmTextWidgetClass) return Gui_TEXTFIELD_HEIGHT;
		if (klas == xmToggleButtonWidgetClass) return
			#ifdef UNIX
				Gui_CHECKBUTTON_HEIGHT;   // BUG
			#else
				my isRadioButton ? Gui_RADIOBUTTON_HEIGHT : Gui_CHECKBUTTON_HEIGHT;
			#endif
	#endif
	return 100;
}

void _GuiObject_position (GuiObject me, int left, int right, int top, int bottom) {
	#if gtk
		// TODO: ...nog even te creatief
	#elif win || mac && useCarbon
		if (left >= 0) {
			if (right > 0) {
				XtVaSetValues (me, XmNx, left, XmNwidth, right - left, NULL);
			} else if (right == Gui_AUTOMATIC) {
				XtVaSetValues (me, XmNx, left, NULL);
			} else {
				XtVaSetValues (me, XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, left,
					XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, - right, NULL);
			}
		} else if (left == Gui_AUTOMATIC) {
			Melder_assert (right <= 0);
			if (right > Gui_AUTOMATIC + 3000)
				XtVaSetValues (me, XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, - right, NULL);
			else if (right != Gui_AUTOMATIC)
				XtVaSetValues (me, XmNwidth, right - Gui_AUTOMATIC, NULL);
		} else {
			Melder_assert (right <= 0);
			XtVaSetValues (me, XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, - right, XmNwidth, right - left, NULL);
		}
		if (top >= 0) {
			if (bottom > 0) {
				XtVaSetValues (me, XmNy, top, XmNheight, bottom - top, NULL);
			} else if (bottom == Gui_AUTOMATIC) {
				XtVaSetValues (me, XmNy, top, XmNheight, _Gui_defaultHeight (me), NULL);
			} else {
				XtVaSetValues (me, XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, top,
					XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, - bottom, NULL);
			}
		} else if (top == Gui_AUTOMATIC) {
			Melder_assert (bottom <= 0);
			if (bottom > Gui_AUTOMATIC + 3000)
				XtVaSetValues (me, XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, - bottom, XmNheight, _Gui_defaultHeight (me), NULL);
			else if (bottom != Gui_AUTOMATIC)
				XtVaSetValues (me, XmNheight, bottom - Gui_AUTOMATIC, NULL);
		} else {
			Melder_assert (bottom <= 0);
			XtVaSetValues (me, XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, - bottom, XmNheight, bottom - top, NULL);
		}
	#endif
}

void * _GuiObject_getUserData (GuiObject me) {
	void *userData = NULL;
	#if gtk
		userData = (void *) g_object_get_data (G_OBJECT (me), "praat");
	#elif win
		XtVaGetValues (me, XmNuserData, & userData, NULL);
	#elif mac
		#if useCarbon
			XtVaGetValues (me, XmNuserData, & userData, NULL);
		#else
		#endif
	#endif
	return userData;
}

void _GuiObject_setUserData (GuiObject me, void *userData) {
	#if gtk
		g_object_set_data (G_OBJECT (me), "praat", userData);
	#elif win
		XtVaSetValues (me, XmNuserData, userData, NULL);
	#elif mac
		#if useCarbon
			XtVaSetValues (me, XmNuserData, userData, NULL);
		#else
		#endif
	#endif
}

void GuiObject_destroy (GuiObject me) {
	#if gtk
		gtk_widget_destroy (GTK_WIDGET (me));
	#elif win
		XtDestroyWidget (me);
	#elif mac
		#if useCarbon
			XtDestroyWidget (me);
		#else
		#endif
	#endif
}

long GuiObject_getHeight (GuiObject me) {
	long height = 0;
	#if gtk
		height = GTK_WIDGET (me) -> allocation.height;
	#elif win
		height = my height;
	#elif mac
		#if useCarbon
			height = my height;
		#else
		#endif
	#endif
	return height;
}

long GuiObject_getWidth (GuiObject me) {
	long width = 0;
	#if gtk
		width = GTK_WIDGET (me) -> allocation.width;
	#elif win
		width = my width;
	#elif mac
		#if useCarbon
			width = my width;
		#else
		#endif
	#endif
	return width;
}

long GuiObject_getX (GuiObject me) {
	long x = 0;
	#if gtk
		x = GTK_WIDGET (me) -> allocation.x;
	#elif win
		x = my x;
	#elif mac
		#if useCarbon
			x = my x;
		#else
		#endif
	#endif
	return x;
}

long GuiObject_getY (GuiObject me) {
	long y = 0;
	#if gtk
		y = GTK_WIDGET (me) -> allocation.y;
	#elif win
		y = my y;
	#elif mac
		#if useCarbon
			y = my y;
		#else
		#endif
	#endif
	return y;
}

void GuiObject_move (GuiObject me, long x, long y) {
	#if gtk
	#elif win || mac && useCarbon
		if (x != Gui_AUTOMATIC) {
			if (y != Gui_AUTOMATIC) {
				XtVaSetValues (me, XmNx, (Position) x, XmNy, (Position) y, NULL);   // 64-bit-compatible
			} else {
				XtVaSetValues (me, XmNx, (Position) x, NULL);   // 64-bit-compatible
			}
		} else if (y != Gui_AUTOMATIC) {
			XtVaSetValues (me, XmNy, (Position) y, NULL);   // 64-bit-compatible
		}
	#endif
}

void GuiObject_hide (GuiObject me) {
	#if gtk
		GuiObject parent = gtk_widget_get_parent (GTK_WIDGET (me));
		if (parent != NULL && GTK_IS_DIALOG (parent)) {   // I am the top vbox of a dialog
			gtk_widget_hide (GTK_WIDGET (parent));
		} else {
			gtk_widget_hide (GTK_WIDGET (me));
		}
	#elif win
		XtUnmanageChild (me);
		// nothing, because the scrolled window is not a widget
	#elif mac
		#if useCarbon
			XtUnmanageChild (me);
			if (my widgetClass == xmListWidgetClass) {
				XtUnmanageChild (my parent);   // the containing scrolled window; BUG if created with XmScrolledList?
			}
		#else
		#endif
	#endif
}

GuiObject GuiObject_parent (GuiObject me) {
	#if gtk
		return gtk_widget_get_parent (GTK_WIDGET (me));
	#elif win
		return my parent;
	#elif mac
		#if useCarbon
			return my parent;
		#else
			return NULL;   // TODO
		#endif
	#endif
}

void GuiObject_setSensitive (GuiObject me, bool sensitive) {
	#if gtk
		gtk_widget_set_sensitive (GTK_WIDGET (me), sensitive);
	#elif win
		XtSetSensitive (me, sensitive);
	#elif mac
		#if useCarbon
			XtSetSensitive (me, sensitive);
		#else
		#endif
	#endif
}

void GuiObject_show (GuiObject me) {
	#if gtk
		GuiObject parent = gtk_widget_get_parent (GTK_WIDGET (me));
		if (GTK_IS_WINDOW (parent)) {
			// I am a window's vbox
			gtk_widget_show (GTK_WIDGET (me));
			gtk_window_present (GTK_WINDOW (parent));
		} else if (GTK_IS_DIALOG (parent)) {
			// I am a dialog's vbox, and therefore automatically shown
			gtk_window_present (GTK_WINDOW (parent));
		} else {
			gtk_widget_show (GTK_WIDGET (me));
		}
	#elif win
		XtManageChild (me);
		GuiObject parent = my parent;
		if (parent -> widgetClass == xmShellWidgetClass) {
			XMapRaised (XtDisplay (parent), XtWindow (parent));
		} else if (mac && my widgetClass == xmListWidgetClass) {
			XtManageChild (parent);   // the containing scrolled window; BUG if created with XmScrolledList?
		}
	#elif mac
		#if useCarbon
			XtManageChild (me);
			GuiObject parent = my parent;
			if (parent -> widgetClass == xmShellWidgetClass) {
				XMapRaised (XtDisplay (parent), XtWindow (parent));
			} else if (mac && my widgetClass == xmListWidgetClass) {
				XtManageChild (parent);   // the containing scrolled window; BUG if created with XmScrolledList?
			}
		#else
		#endif
	#endif
}

void GuiObject_size (GuiObject me, long width, long height) {
	#if gtk
		if (width == Gui_AUTOMATIC || width <= 0) width = -1;
		if (height == Gui_AUTOMATIC || height <= 0) height = -1;
		gtk_widget_set_size_request (GTK_WIDGET (me), width, height);
	#elif win || mac && useCarbon
		if (width != Gui_AUTOMATIC) {
			if (height != Gui_AUTOMATIC) {
				XtVaSetValues (me, XmNwidth, (Dimension) width, XmNheight, (Dimension) height, NULL);   // 64-bit-compatible
			} else {
				XtVaSetValues (me, XmNwidth, (Dimension) width, NULL);   // 64-bit-compatible
			}
		} else if (height != Gui_AUTOMATIC) {
			XtVaSetValues (me, XmNheight, (Dimension) height, NULL);   // 64-bit-compatible
		}
	#endif
}

/* End of file GuiObject.cpp */
