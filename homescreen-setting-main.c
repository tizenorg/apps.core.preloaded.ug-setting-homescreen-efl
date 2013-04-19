/*
 * Copyright 2012  Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <vconf.h>
#include <ui-gadget.h>

#include "homescreen-setting-main.h"
#include "homescreen-setting-data.h"
#include "homescreen-setting-type.h"

static Elm_Genlist_Item_Class itc_seperator, itc_type;

struct ug_data *g_ug_data = NULL;

enum
{
	MAIN_INDEX_GROUP = 0,
	MAIN_INDEX_TYPE,
};

enum
{
	MAIN_GROUP_HOMESCREEN = 0,
};

typedef struct _homescreen_setting_main homescreen_setting_main_t;
struct _homescreen_setting_main
{
	int index;
	int group;
	char *text1;
	char *text2;
};

static void _homescreen_setting_main_back_cb(void *data, Evas_Object * obj, void *event_info)
{
	HOMESET_DBG("");
	struct ug_data *ugd = (struct ug_data *) data;

	if (ugd == NULL)
		return;

	ug_destroy_me(ugd->ug);
}

static char *_homescreen_setting_main_gl_text_get(void *data, Evas_Object *obj, const char *part)
{
	homescreen_setting_main_t *main_data = (homescreen_setting_main_t *) data;

	if (main_data == NULL)
	{
		HOMESET_ERR("invalid data");
		return NULL;
	}

	if (!strcmp(part, "elm.text.1"))
	{
		if (main_data->index == MAIN_INDEX_TYPE && main_data->text1 != NULL)
		{
			return strdup(main_data->text1);
		}
	}
	else if (!strcmp(part, "elm.text.2"))
	{
		if (main_data->index == MAIN_INDEX_TYPE)
		{
			if (main_data->text2)
			{
				free(main_data->text2);
			}

			char *homeapp = homescreen_setting_data_get_selected_homeapp();
			if (homeapp)
			{
				if (!strcmp(homeapp, HOMESCREEN_SETTING_DEFAULT_PKGNAME))
				{
					main_data->text2 = strdup(HOMESET_TEXT("IDS_ST_BODY_DEFAULT_HOME_SCREEN"));
				}
				else
				{
					main_data->text2 = homescreen_setting_data_get_name(homeapp);
				}

				free(homeapp);
			}
			else
			{
				main_data->text2 = strdup(HOMESET_TEXT("IDS_ST_BODY_DEFAULT_HOME_SCREEN"));
			}

			return strdup(main_data->text2);
		}
	}

	return NULL;
}

static void _homescreen_setting_main_gl_del(void *data, Evas_Object *obj)
{
	HOMESET_DBG("");
	homescreen_setting_main_t *main_data = (homescreen_setting_main_t *) data;

	if (main_data == NULL)
	{
		HOMESET_ERR("invalid data");
		return;
	}

	/* Release data */
	if (main_data->text1 != NULL)
	{
		free(main_data->text1);
	}

	if (main_data->text2 != NULL)
	{
		free(main_data->text2);
	}

	free(main_data);
}

static void _homescreen_setting_main_gl_sel(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	if (item == NULL)
	{
		HOMESET_ERR("invalid item data");
		return;
	}

	elm_genlist_item_selected_set(item, EINA_FALSE);

	homescreen_setting_main_t *main_data = (homescreen_setting_main_t *) elm_object_item_data_get(item);
	if (main_data == NULL)
	{
		HOMESET_ERR("invalid main data");
		return;
	}

	struct ug_data *ugd = (struct ug_data *) data;
	if (ugd == NULL)
	{
		HOMESET_ERR("invalid ug data");
		return;
	}

	if (main_data->index == MAIN_INDEX_TYPE)
	{
		/* change home screen type view */
		homescreen_setting_type_create_view(ugd);
	}
}

static Evas_Object* _homescreen_setting_main_add_genlist(struct ug_data *ugd)
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

	/* dialogue for type */
	itc_type.item_style = "dialogue/2text.3";
	itc_type.func.text_get = _homescreen_setting_main_gl_text_get;
	itc_type.func.content_get = NULL;
	itc_type.func.state_get = NULL;
	itc_type.func.del = _homescreen_setting_main_gl_del;

	homescreen_setting_main_t *main_data = (homescreen_setting_main_t *) malloc(sizeof(homescreen_setting_main_t));
	if (main_data != NULL)
	{
		main_data->index = MAIN_INDEX_TYPE;
		main_data->group = MAIN_GROUP_HOMESCREEN;
		main_data->text1 = strdup(HOMESET_TEXT("IDS_ST_BODY_HOME_SCREEN_TYPE"));
		main_data->text2 = strdup(HOMESET_TEXT("IDS_ST_BODY_DEFAULT_HOME_SCREEN"));

		elm_genlist_item_append(genlist, &itc_type, (void *) main_data, NULL, ELM_GENLIST_ITEM_NONE, _homescreen_setting_main_gl_sel, ugd);
	}

	return genlist;
}

void homescreen_setting_main_create_view(void *data)
{
	HOMESET_DBG("create main view");
	struct ug_data *ugd = (struct ug_data *) data;
	if (!ugd)
	{
		HOMESET_ERR("invalid ugd");
		return;
	}

	g_ug_data = ugd;

	/* Create genlist */
	Evas_Object *genlist = _homescreen_setting_main_add_genlist(ugd);
	g_ug_data->genlist_main = genlist;

	/* Back button */
	Evas_Object *button = elm_button_add(ugd->naviframe);
	elm_object_style_set(button, "naviframe/end_btn/default");
	evas_object_smart_callback_add(button, "clicked", _homescreen_setting_main_back_cb, ugd);

	/* Push to naviframe */
	elm_naviframe_item_push(ugd->naviframe, HOMESET_TEXT("IDS_ST_HEADER_HOME_SCREEN"), button, NULL, genlist, NULL);
}
