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


#include <glib.h>
#include <gtk/gtk.h>
#include <glade/glade.h>

#include "freetuxtv-channels-group.h"
#include "freetuxtv-channel.h"

G_DEFINE_TYPE (FreetuxTVChannelsGroup, freetuxtv_channels_group, GTK_TYPE_VBOX);

static void
on_arrow_clicked (GtkWidget *widget, GdkEventButton *event, 
		  gpointer user_data);

static void
on_grouphead_clicked (GtkWidget *widget, GdkEventButton *event, 
		      gpointer user_data);

static void
on_menudeletegroup_activate (GtkMenuItem *menuitem, gpointer user_data);

static void
on_menudeletechannels_activate (GtkMenuItem *menuitem, gpointer user_data);

static void
on_menurefresh_activate (GtkMenuItem *menuitem, gpointer user_data);

static void
show_popup_on_widget (FreetuxTVChannelsGroup *self, GtkWidget *widget, GdkEventButton *event);

enum {
  MENU_DELETE_GROUP,
  MENU_DELETE_CHANNELS,
  MENU_REFRESH_GROUP,
  LAST_SIGNAL
};

static guint channelsgroup_signals[LAST_SIGNAL];

GtkWidget *
freetuxtv_channels_group_new (gchar *id, gchar *name, gchar *uri)
{
	FreetuxTVChannelsGroup *self = NULL;
	
	self = gtk_type_new (freetuxtv_channels_group_get_type ());
	gtk_box_set_homogeneous(GTK_BOX(self),FALSE);
	gtk_box_set_spacing(GTK_BOX(self),0);
	
	self->id = g_strdup(id);
	self->name = g_strdup(name);
	self->uri = g_strdup(uri);

	self->filter = "";
	self->state = FREETUXTV_CHANNELS_GROUP_EXPANDED;
 
	GdkColor color;
	color.pixel = 0;
	color.red   = 0xcf * 0x100;
	color.green = 0xe0 * 0x100;
	color.blue  = 0xf5 * 0x100;

	GtkWidget *eventbox = gtk_event_box_new ();
	gtk_widget_modify_bg(GTK_WIDGET(eventbox), GTK_STATE_NORMAL, &color);
	gtk_box_pack_start (GTK_BOX(self), eventbox, FALSE, FALSE, 0);

	GtkWidget *hbox = gtk_hbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER(eventbox), hbox);
	gtk_widget_set_tooltip_text (GTK_WIDGET(hbox), self->name);
	g_signal_connect(G_OBJECT(eventbox), "button-press-event",
			 G_CALLBACK(on_grouphead_clicked), self);
	
	/* Fleche pour l'expand */
	eventbox = gtk_event_box_new ();
	gtk_widget_modify_bg(GTK_WIDGET(eventbox), GTK_STATE_NORMAL, &color);
	g_signal_connect(G_OBJECT(eventbox), "button-press-event",
			 G_CALLBACK(on_arrow_clicked), self);
	gtk_box_pack_start (GTK_BOX(hbox), eventbox, FALSE, FALSE, 0);
	GtkWidget *arrow = gtk_arrow_new (GTK_ARROW_DOWN, GTK_SHADOW_IN);
	self->arrow = arrow;
	gtk_container_add (GTK_CONTAINER(eventbox), arrow);

	/* Nom du groupe */
	GtkWidget *label = gtk_label_new (self->name);
	gtk_misc_set_padding (GTK_MISC(label),3,3);
	gtk_misc_set_alignment (GTK_MISC(label),0,0.5);
	gtk_label_set_ellipsize (GTK_LABEL(label), PANGO_ELLIPSIZE_END);
	gtk_box_pack_start (GTK_BOX(hbox), label, TRUE, TRUE, 0);

	/* Liste des chaines du groupe */
	self->channels_widget = gtk_vbox_new(FALSE, 1);
	gtk_box_pack_start (GTK_BOX(self), 
			    self->channels_widget, FALSE, FALSE, 0);
	
	gtk_widget_show_all (GTK_WIDGET(self));
	
	return GTK_WIDGET(self);
}

void
freetuxtv_channels_group_add_channel (FreetuxTVChannelsGroup *self,
				      FreetuxTVChannel *channel)
{
	gtk_box_pack_start (GTK_BOX(self->channels_widget),
			    GTK_WIDGET(channel),
			    FALSE, FALSE, 0);
}

void
freetuxtv_channels_group_delete_channels (FreetuxTVChannelsGroup *self)
{
	gtk_widget_destroy (GTK_WIDGET(self->channels_widget));
	self->channels_widget = gtk_vbox_new(FALSE, 1);
	gtk_box_pack_start (GTK_BOX(self), 
			    self->channels_widget, FALSE, FALSE, 0);
}

void
freetuxtv_channels_group_change_collapsed (FreetuxTVChannelsGroup *self)
{
	if(self->state == FREETUXTV_CHANNELS_GROUP_EXPANDED) {
		freetuxtv_channels_group_set_collapsed (self,
							FREETUXTV_CHANNELS_GROUP_COLLAPSED);
	}else{
		freetuxtv_channels_group_set_collapsed (self,
							FREETUXTV_CHANNELS_GROUP_EXPANDED);	
	}	
}

void
freetuxtv_channels_group_set_collapsed (FreetuxTVChannelsGroup *self,
					FreetuxTVChannelsGroupState state)
{
	GtkWidget *entryfilter;
	gchar *filter;

	self->state = state;
	if(self->state == FREETUXTV_CHANNELS_GROUP_EXPANDED) {
		gtk_arrow_set (GTK_ARROW(self->arrow), 
			       GTK_ARROW_DOWN,
			       GTK_SHADOW_NONE);

		freetuxtv_channels_group_set_filter (self, self->filter);
	}else{
		gtk_arrow_set (GTK_ARROW(self->arrow), 
			       GTK_ARROW_RIGHT,
			       GTK_SHADOW_NONE);
		gtk_widget_hide_all (GTK_WIDGET(self->channels_widget));	
	}
}

int
freetuxtv_channels_group_set_filter (FreetuxTVChannelsGroup *self,
				     gchar *filter)
{
	int count = 0;
	GList* tmp;

	self->filter = filter;

	tmp = g_list_first (gtk_container_get_children (GTK_CONTAINER(self->channels_widget)));
	gtk_widget_show_all (GTK_WIDGET(self));
	while (tmp != NULL){
		FreetuxTVChannel *channel;
		channel = FREETUXTV_CHANNEL (tmp->data);
		count += freetuxtv_channel_show_if_filter (channel, self->filter);
		tmp = g_list_next (tmp); 
	}
	if( count == 0 && g_ascii_strcasecmp(filter, "") != 0){
		self->state = FREETUXTV_CHANNELS_GROUP_COLLAPSED;
		gtk_arrow_set (GTK_ARROW(self->arrow), 
			       GTK_ARROW_RIGHT,
			       GTK_SHADOW_NONE);
		gtk_widget_hide_all (GTK_WIDGET(self));
	}else{
		self->state = FREETUXTV_CHANNELS_GROUP_EXPANDED;
		gtk_arrow_set (GTK_ARROW(self->arrow), 
			       GTK_ARROW_DOWN,
			       GTK_SHADOW_NONE);	
	}
	return count;
}

gchar *
freetuxtv_channels_group_get_filter (FreetuxTVChannelsGroup *self)
{
	return self->filter;
}

FreetuxTVChannelsGroup *
freetuxtv_channels_group_get_from_channel (FreetuxTVChannel *channel)
{
	GtkWidget *parent;
	
	g_return_val_if_fail(channel != NULL, NULL);
	g_return_val_if_fail(FREETUXTV_IS_CHANNEL(channel), NULL);
	
	parent = gtk_widget_get_parent(GTK_WIDGET(channel));
	parent = gtk_widget_get_parent(parent);
	
	return FREETUXTV_CHANNELS_GROUP(parent);

}

static void
show_popup_on_widget (FreetuxTVChannelsGroup *self, GtkWidget *widget, GdkEventButton *event)
{
	GtkWidget *menu;
	GtkWidget *menuitem;
	GtkWidget *image;
	int button, event_time;

	menu = gtk_menu_new ();
	
	self->popup_menu = menu;

	menuitem = gtk_image_menu_item_new_with_label("Actualiser depuis la playlist");
	gtk_widget_set_tooltip_text (GTK_WIDGET(menuitem), self->uri);
	image = gtk_image_new_from_stock (GTK_STOCK_REFRESH, GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menuitem), image);
	gtk_menu_append (GTK_MENU (menu), menuitem);
	g_signal_connect(G_OBJECT(menuitem), "activate",
			 G_CALLBACK(on_menurefresh_activate), self);
	gtk_widget_show (menuitem);
	
	menuitem = gtk_image_menu_item_new_with_label("Supprimer les chaînes du groupe");
	image = gtk_image_new_from_stock (GTK_STOCK_DELETE, GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menuitem), image);
	gtk_menu_append (GTK_MENU (menu), menuitem);
	g_signal_connect(G_OBJECT(menuitem), "activate",
			 G_CALLBACK(on_menudeletechannels_activate), self);
	gtk_widget_show (menuitem);
	
	menuitem = gtk_image_menu_item_new_with_label("Supprimer le groupe");
	image = gtk_image_new_from_stock (GTK_STOCK_DELETE, GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menuitem), image);
	gtk_menu_append (GTK_MENU (menu), menuitem);
	g_signal_connect(G_OBJECT(menuitem), "activate",
			 G_CALLBACK(on_menudeletegroup_activate), self);
	gtk_widget_show (menuitem);

	gtk_widget_show (menu);
	
	if (event){
		button = event->button;
		event_time = event->time;
	}else{
		button = 0;
		event_time = gtk_get_current_event_time ();
	}

	gtk_menu_attach_to_widget (GTK_MENU (menu), widget, NULL);
	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 
			button, event_time);
}

static void
on_menudeletegroup_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	FreetuxTVChannelsGroup *self;
	self = (FreetuxTVChannelsGroup *) user_data;
	
	gtk_widget_destroy (self->popup_menu);
	
	/* Envoi du signal delete-group */
	g_signal_emit (G_OBJECT (self),
		       channelsgroup_signals [MENU_DELETE_GROUP],
		       0);
}

static void
on_menudeletechannels_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	FreetuxTVChannelsGroup *self;
	self = (FreetuxTVChannelsGroup *) user_data;
	
	gtk_widget_destroy (self->popup_menu);
	
	/* Envoi du signal delete-channels */
	g_signal_emit (G_OBJECT (self),
		       channelsgroup_signals [MENU_DELETE_CHANNELS],
		       0
		       );

	freetuxtv_channels_group_delete_channels (self);

}

static void
on_menurefresh_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	FreetuxTVChannelsGroup *self;
	self = (FreetuxTVChannelsGroup *) user_data;
	
	gtk_widget_destroy (self->popup_menu);
	
	/* Envoi du signal refresh-group */
	g_signal_emit (G_OBJECT (self),
		       channelsgroup_signals [MENU_REFRESH_GROUP],
		       0
		       );

}

static void
on_arrow_clicked (GtkWidget *widget, GdkEventButton *event,
		  gpointer user_data)
{
	freetuxtv_channels_group_change_collapsed ((FreetuxTVChannelsGroup *) user_data);
}

static void
on_grouphead_clicked (GtkWidget *widget, GdkEventButton *event,
		      gpointer user_data)
{
	if(event->button == 3){
		show_popup_on_widget ((FreetuxTVChannelsGroup *) user_data, widget, event);	
	}else{
		if (event->type==GDK_2BUTTON_PRESS){
			freetuxtv_channels_group_change_collapsed ((FreetuxTVChannelsGroup *) user_data);
		}
	}
}

static void
freetuxtv_channels_group_init (FreetuxTVChannelsGroup *object)
{
	object->id=0;
	object->name="";
	object->uri="";
	object->filter="";
	object->state = FREETUXTV_CHANNELS_GROUP_EXPANDED;

	object->channels_widget = NULL;
	object->arrow = NULL;
	object->popup_menu = NULL;
}

static void
freetuxtv_channels_group_finalize (GObject *object)
{
	
}

static void
freetuxtv_channels_group_class_init (FreetuxTVChannelsGroupClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->finalize = freetuxtv_channels_group_finalize;

	/* Enregistrements des signaux du widget */
	channelsgroup_signals [MENU_DELETE_GROUP] = g_signal_new ("menu-delete-group",
							     G_TYPE_FROM_CLASS (klass),
							     G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
							     G_STRUCT_OFFSET (FreetuxTVChannelsGroupClass, menu_delete_group),
							     NULL, NULL,
							     g_cclosure_marshal_VOID__VOID,
							     G_TYPE_NONE,
							     0
							     );
	channelsgroup_signals [MENU_DELETE_CHANNELS] = g_signal_new ("menu-delete-channels",
								G_TYPE_FROM_CLASS (klass),
								G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
								G_STRUCT_OFFSET (FreetuxTVChannelsGroupClass, menu_delete_channels),
								NULL, NULL,
								g_cclosure_marshal_VOID__VOID,
								G_TYPE_NONE,
								0
								);
	channelsgroup_signals [MENU_REFRESH_GROUP] = g_signal_new ("menu-refresh-group",
							      G_TYPE_FROM_CLASS (klass),
							      G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
							      G_STRUCT_OFFSET (FreetuxTVChannelsGroupClass, menu_refresh_group),
							      NULL, NULL,
							      g_cclosure_marshal_VOID__VOID,
							      G_TYPE_NONE,
							      0
							      );

}
