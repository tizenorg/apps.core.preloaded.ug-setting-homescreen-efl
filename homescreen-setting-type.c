/*
 * Copyright 2012  Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://floralicense.org
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <vconf.h>
#include <ail.h>
#include <pkgmgr-info.h>

#include "homescreen-setting-data.h"
#include "homescreen-setting-type.h"

#define HOMESET_DEFAULT_HOMESCREEN "org.tizen.menu-screen"

static Elm_Genlist_Item_Class itc_seperator, itc;

enum
{
	INDEX_DEFAULT = 0,
	INDEX_DOWNLOADED,
	INDEX_DOWNLOADED_LAST,
};

static Elm_Genlist_Item_Class itc_seperator, itc;
static Evas_Object *radio_group = NULL;

static char *_homescreen_setting_type_gl_text_get(void *data, Evas_Object *obj, const char *part)
{
	homescreen_setting_data_list_t *data_list = (homescreen_setting_data_list_t *) data;

	if (data_list == NULL)
	{
		HOMESET_ERR("invalid data");
		return NULL;
	}

	if (!strcmp(part, "elm.text"))
	{
		if (data_list->name != NULL)
		{
			return strdup(data_list->name);
		}
	}

	return NULL;
}

static Evas_Object *_homescreen_setting_type_gl_content_get(void *data, Evas_Object *obj, const char *part)
{
	homescreen_setting_data_list_t *data_list = (homescreen_setting_data_list_t *) data;

	if (data_list == NULL)
	{
		HOMESET_ERR("invalid data");
		return NULL;
	}

	Evas_Object *radio = NULL;
	if (!strcmp(part, "elm.icon.1"))
	{
		radio = elm_radio_add(obj);
		elm_radio_state_value_set(radio, data_list->id);

		if (data_list->id != 0)
		{
			elm_radio_group_add(radio, radio_group);
		}
		else
		{
			radio_group = radio;
		}

		// check current homeapp
		char *homeapp = homescreen_setting_data_get_selected_homeapp();
		if (homeapp && data_list->appid)
		{
			if (!strcmp(homeapp, data_list->appid))
			{
				elm_radio_value_set(radio_group, data_list->id);
			}
		}

		if (homeapp)
		{
			free(homeapp);
		}

		return radio;
	}
	else if (!strcmp(part, "elm.icon.2"))
	{
		if (data_list->index != INDEX_DEFAULT)
		{
			// TODO: Add next icon
			return NULL;
		}
	}
	return NULL;
}

static void _homescreen_setting_type_gl_del(void *data, Evas_Object *obj)
{
	homescreen_setting_data_list_t *data_list = (homescreen_setting_data_list_t *) data;

	if (data_list)
	{
		if (data_list->appid)
	{
			free(data_list->appid);
	}
		if (data_list->name)
	{
			free(data_list->name);
	}

		free(data_list);
	}
}

static void _homescreen_setting_type_gl_sel(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	if (item == NULL)
	{
		HOMESET_ERR("invalid item data");
		return;
	}

	elm_genlist_item_selected_set(item, EINA_FALSE);

	homescreen_setting_data_list_t *data_list = (homescreen_setting_data_list_t *) elm_object_item_data_get(item);
	if (data_list == NULL)
	{
		HOMESET_ERR("invalid type data");
		return;
	}

	struct ug_data *ugd = (struct ug_data *) data;
	if (ugd == NULL)
	{
		HOMESET_ERR("invalid ug data");
		return;
	}

	char *homeapp = homescreen_setting_data_get_selected_homeapp();
	if (homeapp && data_list->appid)
	{
		if (!strcmp(homeapp, data_list->appid))
		{
			HOMESET_DBG("Already set homeapp[%s]", homeapp);
		}
		else
		{
			HOMESET_DBG("Change homeapp[%s]=>[%s]", homeapp, data_list->appid);
			homescreen_setting_data_set_selected_homeapp(data_list->appid);

			elm_radio_value_set(radio_group, data_list->id);

			elm_naviframe_item_pop(ugd->naviframe);
		}
	}

	if (homeapp)
	{
		free(homeapp);
		}
	}

void _homescreen_setting_type_gl_realized_cb(void *data, Evas_Object *obj, void *event_info)
	{
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	if (!item)
	{
		HOMESET_DBG("invalid item");
		return;
	}

	homescreen_setting_data_list_t *data_list = (homescreen_setting_data_list_t *) elm_object_item_data_get(item);
	if (!data_list)
	{
		HOMESET_DBG("invalid data");
		return;
	}

	if (data_list->index == INDEX_DEFAULT)
	{
		elm_object_item_signal_emit(item, "elm,state,top", "");
	}
	else if (data_list->index == INDEX_DOWNLOADED)
	{
		elm_object_item_signal_emit(item, "elm,state,center", "");
	}
	else if (data_list->index == INDEX_DOWNLOADED_LAST)
	{
		elm_object_item_signal_emit(item, "elm,state,bottom", "");
	}
}

static Evas_Object* _homescreen_setting_type_add_genlist(struct ug_data *ugd)
{
	Evas_Object *genlist;
	Elm_Object_Item *it;

	genlist = elm_genlist_add(ugd->naviframe);
	elm_object_style_set(genlist, "dialogue");

	/* dialogue for seperator */
	itc_seperator.item_style = "dialogue/separator";
	itc_seperator.func.text_get = NULL;
	itc_seperator.func.content_get = NULL;
	itc_seperator.func.state_get = NULL;
	itc_seperator.func.del = NULL;

	it = elm_genlist_item_append(genlist, &itc_seperator, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(it, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	/* dialogue for type list */
	itc.item_style = "dialogue/1text.2icon.2";
	itc.func.text_get = _homescreen_setting_type_gl_text_get;
	itc.func.content_get = _homescreen_setting_type_gl_content_get;
	itc.func.state_get = NULL;
	itc.func.del = _homescreen_setting_type_gl_del;

	// first item should be default home
	int id = 0;
	homescreen_setting_data_list_t *data_list = (homescreen_setting_data_list_t *) malloc(sizeof(homescreen_setting_data_list_t));
	if (data_list)
	{
		data_list->id = id++;
		data_list->index = INDEX_DEFAULT;
		data_list->appid = strdup(HOMESET_DEFAULT_HOMESCREEN);
		data_list->name = strdup(HOMESET_TEXT("IDS_ST_BODY_DEFAULT_HOME_SCREEN"));
		elm_genlist_item_append(genlist, &itc, (void *) data_list, NULL, ELM_GENLIST_ITEM_NONE, _homescreen_setting_type_gl_sel, ugd);
	}

	int count = 0;
	int i = 0;
	data_list = homescreen_setting_data_get_homeapp_list(&count);
	for (i = 0; i < count; i++)
	{
		if (data_list)
		{
			data_list->id = id++;
			if (i == count - 1)
			{
				data_list->index = INDEX_DOWNLOADED_LAST;
			}
			else
			{
				data_list->index = INDEX_DOWNLOADED;
			}
			elm_genlist_item_append(genlist, &itc, (void *) data_list, NULL, ELM_GENLIST_ITEM_NONE, _homescreen_setting_type_gl_sel, ugd);

			data_list = data_list->next;
		}
	}

	if (count > 0)
	{
		evas_object_smart_callback_add(genlist, "realized", _homescreen_setting_type_gl_realized_cb, NULL);
	}

	return genlist;
}

void homescreen_setting_type_create_view(void *data)
{
	HOMESET_DBG("create type view");
	struct ug_data *ugd = (struct ug_data *) data;

	/* Create genlist */
	Evas_Object *genlist = _homescreen_setting_type_add_genlist(ugd);

	/* Push to naviframe */
	elm_naviframe_item_push(ugd->naviframe, HOMESET_TEXT("IDS_ST_BODY_HOME_SCREEN_TYPE"), NULL, NULL, genlist, NULL);
}
