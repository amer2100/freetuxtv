/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8-*- */
/*
 * FreetuxTV is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version.
 *
 * FreetuxTV is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Glade; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef FREETUXTV_UTILS_H
#define FREETUXTV_UTILS_H

#include <gtk/gtk.h>

#include "freetuxtv-app.h"

gchar*
format_time(gint seconds);

gchar*
format_time2(gint seconds);

gchar*
format_size(glong size);

gchar*
get_recording_options(FreetuxTVApp *app, gchar* base_filename, gboolean from_preview, gchar **out_filename);

void
g_time_val_add_seconds (GTimeVal *timeval, glong seconds);

gint
g_time_val_compare (GTimeVal *timeval1, GTimeVal *timeval2);

void
g_time_val_copy (GTimeVal *timeval_src, GTimeVal *timeval_dest);

gchar*
g_time_val_to_string(GTimeVal *timeval, const gchar* format);


#endif /* FREETUXTV_UTILS_H */
