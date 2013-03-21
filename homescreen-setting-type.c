/*
 * Copyright 2012  Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.tizenopensource.org/license
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <vconf.h>

#include "homescreen-setting-type.h"

#define HOMESET_DEFAULT_HOMESCREEN "org.tizen.menu-screen"

static Elm_Genlist_Item_Class itc_seperator, itc;

enum
{
	TYPE_INDEX_DEFAULT = 0,
	TYPE_INDEX_DOWNLOADED
};

typedef struct _homescreen_setting_type homescreen_setting_type_t;
struct _homescreen_setting_type
{
	homescreen_setting_type_t *next;
	int id;
	int index;
	char *text;
	char *pkgname;
};

/* type data list should indicate head of the list */
static Evas_Object *radio_group = NULL;

static char *_homescreen_setting_type_gl_text_get(void *data, Evas_Object *obj, const char *part)
{
	homescreen_setting_type_t *type_data = (homescreen_setting_type_t *) data;

	if (type_data == NULL)
	{
		HOMESET_ERR("invalid data");
		return NULL;
	}

	if (!strcmp(part, "elm.text"))
	{
		if (type_data->text != NULL)
		{
			return strdup(type_data->text);
		}
	}

	return NULL;
}

static Evas_Object *_homescreen_setting_type_gl_content_get(void *data, Evas_Object *obj, const char *part)
{
	homescreen_setting_type_t *type_data = (homescreen_setting_type_t *) data;

	if (type_data == NULL)
	{
		HOMESET_ERR("invalid data");
		return NULL;
	}

	Evas_Object *radio = NULL;
	if (!strcmp(part, "elm.icon.1"))
	{
		radio = elm_radio_add(obj);
		elm_radio_state_value_set(radio, type_data->id);

		if (type_data->id != 0)
		{
			elm_radio_group_add(radio, radio_group);
		}
		else
		{
			radio_group = radio;
		}

		/* Check current menu screen */
		char *current_homescreen = vconf_get_str(VCONFKEY_SETAPPL_SELECTED_PACKAGE_NAME);
		HOMESET_DBG("Current[%s], this[%d][%s]", current_homescreen, type_data->id, type_data->pkgname);
		if (current_homescreen != NULL && type_data->pkgname != NULL)
		{
			if (!strcmp(current_homescreen, type_data->pkgname))
			{
				elm_radio_value_set(radio_group, type_data->id);
			}
		}

		if (current_homescreen != NULL)
		{
			free(current_homescreen);
		}

		return radio;
	}
	else if (!strcmp(part, "elm.icon.2"))
	{
		if (type_data->index == TYPE_INDEX_DOWNLOADED)
		{
			// TODO: Add next icon
			return NULL;
		}
	}
	return NULL;
}

static void _homescreen_setting_type_gl_del(void *data, Evas_Object *obj)
{
	HOMESET_DBG("");
	homescreen_setting_type_t *type_data = (homescreen_setting_type_t *) data;

	if (type_data == NULL)
	{
		HOMESET_ERR("invalid data");
		return;
	}

	/* Release data */
	if (type_data->text != NULL)
	{
		free(type_data->text);
	}

	if (type_data->pkgname != NULL)
	{
		free(type_data->pkgname);
	}

	free(type_data);
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

	homescreen_setting_type_t *type_data = (homescreen_setting_type_t *) elm_object_item_data_get(item);
	if (type_data == NULL)
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

	/* Check current menu screen and set as home screen if different */
	char *current_homescreen = vconf_get_str(VCONFKEY_SETAPPL_SELECTED_PACKAGE_NAME);
	HOMESET_DBG("Current[%s], this[%d][%s]", current_homescreen, type_data->id, type_data->pkgname);
	if (current_homescreen != NULL && type_data->pkgname != NULL)
	{
		if (!strcmp(current_homescreen, type_data->pkgname))
		{
			HOMESET_DBG("already set homescreen");
		}
		else
		{
			HOMESET_DBG("TODO: set[%s]--> [%s]", current_homescreen, type_data->pkgname);
			//vconf_set_str(VCONFKEY_SETAPPL_SELECTED_PACKAGE_NAME, type_data->pkgname);
		}
	}

	if (current_homescreen != NULL)
	{
		free(current_homescreen);
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

	int id = 0;
	homescreen_setting_type_t *type_data = (homescreen_setting_type_t *) malloc(sizeof(homescreen_setting_type_t));
	if (type_data != NULL)
	{
		type_data->id = id++;
		type_data->index = TYPE_INDEX_DEFAULT;
		type_data->text = strdup(HOMESET_TEXT("IDS_ST_BODY_DEFAULT_HOME_SCREEN"));
		type_data->pkgname = strdup(HOMESET_DEFAULT_HOMESCREEN);
		elm_genlist_item_append(genlist, &itc, (void *) type_data, NULL, ELM_GENLIST_ITEM_NONE, _homescreen_setting_type_gl_sel, ugd);
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
