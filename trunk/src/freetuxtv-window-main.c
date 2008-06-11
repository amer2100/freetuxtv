/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8-*- */
/*
 * freetuxtv
 * Copyright (C) FreetuxTV Team's 2008
 * Project homepage : http://code.google.com/p/freetuxtv/
 * 
 * freetuxtv is free software.
 * 
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <glade/glade.h>
#include <sqlite3.h>

#include "freetuxtv-app.h"
#include "freetuxtv-window-main.h"
#include "freetuxtv-channels-list.h"
#include "freetuxtv-player.h"

void
on_windowmain_destroy (GtkWidget *widget, gpointer *data)
{
	gtk_main_quit();
}

void 
on_windowmain_menuitemquit_activate (GtkMenuItem *menuitem,
				     gpointer user_data)
{
	gtk_main_quit();
}

void
on_windowmain_menuitemaboutdialog_activate (GtkMenuItem *menuitem,
					    gpointer user_data)
{
	GladeXML *gladexml;	
	gladexml = glade_xml_new (FREETUXTV_GLADEXML,
				  "aboutdialog", NULL);
	glade_xml_signal_autoconnect (gladexml);
}

void
on_windowmain_buttonclearfilter_clicked (GtkButton *button,
					 gpointer user_data)
{
	FreetuxTVApp *app = (FreetuxTVApp *) user_data;
	GtkWidget *entryfilter;
	entryfilter = glade_xml_get_widget(app->windowmain,
					   "windowmain_entryfilter");
	gtk_entry_set_text(GTK_ENTRY(entryfilter), "");
	channels_list_apply_filter (app);
}


void
on_windowmain_buttonstop_clicked (GtkButton *button,
				  gpointer user_data)
{
	FreetuxTVApp *app = (FreetuxTVApp *) user_data;
	freetuxtv_player_stop (app->player);
}

void
on_windowmain_entryfilter_changed (GtkEntry *entry, gpointer user_data)
{
	FreetuxTVApp *app = (FreetuxTVApp *) user_data;
	channels_list_apply_filter (app);
}

void
on_windowmain_volumecontrol_value_changed (GtkRange *range, gpointer user_data)
{
	FreetuxTVApp *app = (FreetuxTVApp *) user_data;
	freetuxtv_player_set_volume (app->player,
				     gtk_range_get_value (range));
}


void
on_windowmain_menuitemgroupsadd_activate (GtkMenuItem *menuitem,
					  gpointer user_data)
{
	FreetuxTVApp *app = (FreetuxTVApp *) user_data;
	app->dialogaddgroup = glade_xml_new (FREETUXTV_GLADEXML,
					     "dialogaddgroup", NULL);
	glade_xml_signal_autoconnect (app->dialogaddgroup);
	
	/* Connexion des signaux */
	GtkWidget *widget;
	widget = glade_xml_get_widget(app->dialogaddgroup,
				      "dialogaddgroup_add");
	g_signal_connect(G_OBJECT(widget),
			 "clicked",
			 G_CALLBACK(on_dialogaddgroup_add_clicked),
			 app);
}

void
on_dialogaddgroup_add_clicked (GtkButton *button,
			       gpointer user_data)
{
	GtkWidget *windowmain;
	GtkWidget *channelslist;
	GtkWidget *groupname;
	GtkWidget *groupuri;
	GtkWidget *dialog;
	
	gchar *user_db;
		
	struct sqlite3 *db;
	int res;
	char *sqlerr = 0;

	gchar *errmsg = NULL;
	gchar *query;

	FreetuxTVApp *app = (FreetuxTVApp *) user_data;

	gchar * sgroupid;
	gchar * sgroupname;
	gchar * sgroupuri;

	user_db = g_strdup_printf("%s/FreetuxTV/freetuxtv.db",
				  g_get_user_config_dir());
	
	groupname = glade_xml_get_widget(app->dialogaddgroup,
					 "dialogaddgroup_name");
	groupuri = glade_xml_get_widget(app->dialogaddgroup,
					"dialogaddgroup_uri");
	
	/* Verification des champs */
	if(g_ascii_strcasecmp(gtk_entry_get_text(GTK_ENTRY(groupname)),"") == 0
	   && errmsg==NULL){
		errmsg = g_strdup_printf("Veuillez remplir le nom du groupe !");
	}
	if(g_ascii_strcasecmp(gtk_entry_get_text(GTK_ENTRY(groupuri)),"") == 0
	   && errmsg==NULL){
		errmsg = g_strdup_printf("Veuillez remplir l'URL du groupe !");
	}else{
		if(g_ascii_strncasecmp(gtk_entry_get_text(GTK_ENTRY(groupuri)),"file://",7) != 0
		   && g_ascii_strncasecmp(gtk_entry_get_text(GTK_ENTRY(groupuri)),"http://",7) != 0
		   && errmsg==NULL){
			errmsg = g_strdup_printf("L'accès à la playlist doit utiliser le protocole file:// ou http://.\nEx: http://mafreebox.freebox.fr/freeboxtv/playlist.m3u\n    file:///home/freetuxtv/playlist.m3u");
		}
	}
	
	/* Ouverture de la BDD */
	if(errmsg==NULL){
		res = sqlite3_open(user_db,&db);
		if(res != SQLITE_OK){
			sqlite3_close(db);
			errmsg = g_strdup_printf("Sqlite3 : %s\nFreetuxTV : Cannot open database %s\n", sqlite3_errmsg(db), user_db);
		}
	}
	
	if(errmsg == NULL){
		sgroupname = g_strdup(gtk_entry_get_text(GTK_ENTRY(groupname)));
		sgroupuri = g_strdup(gtk_entry_get_text(GTK_ENTRY(groupuri)));
		query = sqlite3_mprintf("INSERT INTO channels_group (name_channelsgroup, uri_channelsgroup) VALUES ('%q','%q');", 
					sgroupname,
					sgroupuri);
		res=sqlite3_exec(db, query, NULL, 0, &sqlerr);
		sqlite3_free(query);
		if(res != SQLITE_OK){
			errmsg = g_strdup_printf("Sqlite3 : %s\nFreetuxTV : Cannot add groups '%s' into database.\n", sqlite3_errmsg(db), gtk_entry_get_text(GTK_ENTRY(groupname)));
			g_free(sgroupname);
			g_free(sgroupuri);
		}
		sqlite3_free(sqlerr);
		sqlite3_close(db);
	}
	
	if(errmsg != NULL){
		windowmain_show_error (app, errmsg);
	}else{
		g_print("sqlite : %d", sqlite3_last_insert_rowid(db));
		FreetuxTVChannelsGroup *channels_group;
		
		sgroupid = g_strdup_printf("%d", sqlite3_last_insert_rowid(db));
		channels_group = FREETUXTV_CHANNELS_GROUP (freetuxtv_channels_group_new (sgroupid,
											 sgroupname,
											 sgroupuri));
		channels_list_add_channels_group (app, channels_group);

		g_free(sgroupid);
		g_free(sgroupname);
		g_free(sgroupuri);

		channels_list_refresh_group (app, channels_group);
		//channels_group_reload_channels (channels_group, app);
		
		gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
	}

	g_free(errmsg);
}

void
on_dialogaddgroup_cancel_clicked (GtkButton *button,
				  gpointer user_data)
{
	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

void
on_aboutdialog_response (GtkDialog *dialog,
			 gpointer user_data)
{
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

void
windowmain_show_error (FreetuxTVApp *app, gchar *msg)
{
	GtkWidget *windowmain;
	GtkWidget* dialog;
	
	windowmain = glade_xml_get_widget (app->windowmain,
					   "windowmain");
	
	dialog = gtk_message_dialog_new(GTK_WINDOW(windowmain),
					GTK_DIALOG_MODAL, 
					GTK_MESSAGE_ERROR,
					GTK_BUTTONS_OK,
					msg);
	
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}
