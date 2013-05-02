/*
 * Copyright 2012  Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.1 (the "License");
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

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

#include "homescreen-setting-efl.h"
#include "homescreen-setting-main.h"

static Evas_Object *_create_naviframe(Evas_Object *parent)
{
	Evas_Object *naviframe = NULL;

	if (parent == NULL)
	{
		HOMESET_ERR("Parent is null.");
		return NULL;
	}

	naviframe = elm_naviframe_add(parent);
	if (naviframe == NULL)
	{
		HOMESET_ERR("Cannot add naviframe.");
		return NULL;
	}

	elm_object_part_content_set(parent, "elm.swallow.content", naviframe);

	evas_object_show(naviframe);

	return naviframe;
}

static Evas_Object *_create_background(Evas_Object *parent)
{
	Evas_Object *bg = elm_bg_add(parent);

	evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_style_set(bg, "group_list");

	elm_object_part_content_set(parent, "elm.swallow.bg", bg);
	evas_object_show(bg);

	return bg;
}

static Evas_Object *_create_layout(Evas_Object *parent)
{
	Evas_Object *layout = NULL;

	if (parent == NULL)
	{
		HOMESET_ERR("Parent is null.");
		return NULL;
	}

	layout = elm_layout_add(parent);
	if (layout == NULL)
	{
		HOMESET_ERR("Cannot add layout.");
		return NULL;
	}

	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	evas_object_show(layout);

	return layout;
}

static Evas_Object *create_fullview(Evas_Object *parent, struct ug_data *ugd)
{
	HOMESET_DBG("");
	if (ugd == NULL)
	{
		HOMESET_ERR("Invalid ug data");
		return NULL;
	}

	/* Create Full view */
	ugd->base = _create_layout(parent);

	elm_layout_theme_set(ugd->base, "layout", "application", "default");
	elm_win_indicator_mode_set(parent, ELM_WIN_INDICATOR_SHOW);

	/* Create background */
	_create_background(ugd->base);

	/* Create navigation bar */
	ugd->naviframe = _create_naviframe(ugd->base);

	/* Create main view */
	homescreen_setting_main_create_view(ugd);

	return ugd->base;
}

static void *on_create(ui_gadget_h ug, enum ug_mode mode, service_h service, void *priv)
{
	HOMESET_DBG("");
	Evas_Object *parent;
	struct ug_data *ugd;

	if (!ug || !priv)
		return NULL;

	bindtextdomain(HOMESET_DOMAIN, "/usr/ug/res/locale");

	ugd = priv;
	ugd->ug = ug;

	parent = ug_get_parent_layout(ug);
	if (!parent)
		return NULL;

	/* always make full view */
	ugd->base = create_fullview(parent, ugd);

	return ugd->base;
}

static void on_start(ui_gadget_h ug, service_h service, void *priv)
{
}

static void on_pause(ui_gadget_h ug, service_h service, void *priv)
{

}

static void on_resume(ui_gadget_h ug, service_h service, void *priv)
{

}

static void on_destroy(ui_gadget_h ug, service_h service, void *priv)
{
	HOMESET_DBG("");
	struct ug_data *ugd;

	if (!ug || !priv)
		return;

	ugd = priv;
	evas_object_del(ugd->base);
	ugd->base = NULL;
}

static void on_message(ui_gadget_h ug, service_h msg, service_h service, void *priv)
{
}

static void on_event(ui_gadget_h ug, enum ug_event event, service_h service, void *priv)
{
	switch (event)
	{
	case UG_EVENT_LOW_MEMORY:
		break;
	case UG_EVENT_LOW_BATTERY:
		break;
	case UG_EVENT_LANG_CHANGE:
		break;
	case UG_EVENT_ROTATE_PORTRAIT:
		break;
	case UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN:
		break;
	case UG_EVENT_ROTATE_LANDSCAPE:
		break;
	case UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN:
		break;
	case UG_EVENT_REGION_CHANGE:
		break;
	default:
		break;
	}
}

UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	HOMESET_DBG("ug init");
	struct ug_data *ugd;

	if (!ops)
		return -1;

	ugd = calloc(1, sizeof(struct ug_data));
	if (!ugd)
		return -1;

	ops->create = on_create;
	ops->start = on_start;
	ops->pause = on_pause;
	ops->resume = on_resume;
	ops->destroy = on_destroy;
	ops->message = on_message;
	ops->event = on_event;
	ops->priv = ugd;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	HOMESET_DBG("ug exit");
	struct ug_data *ugd;

	if (!ops)
		return;

	ugd = ops->priv;
	if (ugd)
		free(ugd);
}
