/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * freetuxtv
 * Copyright (C) Eric Beuque 2010 <eric.beuque@gmail.com>
 * 
 * freetuxtv is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * freetuxtv is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "freetuxtv-channel-infos.h"

G_DEFINE_TYPE (FreetuxTVChannelInfos, freetuxtv_channel_infos, G_TYPE_OBJECT);

FreetuxTVChannelInfos*
freetuxtv_channel_infos_new(gchar *name, gchar *url)
{
	FreetuxTVChannelInfos *channel_infos;
	channel_infos = g_object_new (FREETUXTV_TYPE_CHANNEL_INFOS, NULL);

	channel_infos->name = g_strdup(name);
	channel_infos->url = g_strdup(url);	

	channel_infos->logo_name = NULL;

	channel_infos->vlc_options = NULL;

	return channel_infos;
}

void
freetuxtv_channel_infos_set_id(FreetuxTVChannelInfos* self, int id)
{
	self->id = id;
}

void
freetuxtv_channel_infos_set_position(FreetuxTVChannelInfos* self, int position)
{
	self->position = position;
}

void
freetuxtv_channel_infos_set_logo(FreetuxTVChannelInfos* self,
                                 gchar *logo_name)
{
	if(self->logo_name != NULL){
		g_free(self->logo_name);
	}
	self->logo_name = g_strdup(logo_name);
}

void
freetuxtv_channel_infos_set_vlcoptions(FreetuxTVChannelInfos* self,
                                       gchar **options)
{
	if(self->vlc_options){
		g_strfreev (self->vlc_options);
	} 
	self->vlc_options = g_strdupv(options);
}

void
freetuxtv_channel_infos_set_deinterlace_mode(FreetuxTVChannelInfos* self,
                                             gchar *mode)
{
	if(self->deinterlace_mode){
		g_free(self->deinterlace_mode);
	} 
	self->deinterlace_mode = g_strdup(mode);
}

void
freetuxtv_channel_infos_set_channels_group(FreetuxTVChannelInfos* self,
                                           FreetuxTVChannelsGroupInfos *channels_group)
{
	if(self->channels_group != channels_group){
		if(self->channels_group != NULL){
			g_object_unref(self->channels_group);
		}
		g_object_ref(channels_group);
		self->channels_group = channels_group;
	}
}

static GObject *
freetuxtv_channel_infos_constructor (GType                  gtype,
                                     guint                  n_properties,
                                     GObjectConstructParam *properties)
{
	GObject *obj;
	GObjectClass *parent_class;  
	parent_class = G_OBJECT_CLASS (freetuxtv_channel_infos_parent_class);
	obj = parent_class->constructor (gtype, n_properties, properties);
	return obj;
}

static void
freetuxtv_channel_infos_dispose (GObject *object)
{
	FreetuxTVChannelInfos *self;

	g_return_if_fail(object != NULL);
	g_return_if_fail(FREETUXTV_IS_CHANNEL_INFOS(object));

	self = FREETUXTV_CHANNEL_INFOS(object);

	if(self->name != NULL){
		g_free(self->name);
		self->name = NULL;
	}

	if(self->url != NULL){
		g_free(self->url);
		self->url = NULL;
	}

	if(self->channels_group != NULL){
		g_object_unref(self->channels_group);
		self->channels_group = NULL;
	}

	if(self->vlc_options != NULL){
		g_strfreev(self->vlc_options);
		self->vlc_options = NULL;
	}

	G_OBJECT_CLASS (freetuxtv_channel_infos_parent_class)->dispose (object);

}

static void
freetuxtv_channel_infos_class_init (FreetuxTVChannelInfosClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->constructor = freetuxtv_channel_infos_constructor;
	gobject_class->dispose = freetuxtv_channel_infos_dispose;
}

static void
freetuxtv_channel_infos_init (FreetuxTVChannelInfos *self)
{
	self->id = -1;
	self->name=NULL;
	self->url=NULL;

	self->logo_name = NULL;
	self->vlc_options = NULL;
	self->deinterlace_mode = NULL;

	self->channels_group = NULL;
}