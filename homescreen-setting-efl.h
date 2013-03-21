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

#ifndef __HOMESCREEN_SETTING_EFL_H__
#define __HOMESCREEN_SETTING_EFL_H__

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "ug-homescreen-setting"

#include <dlog.h>

#define HOMESET_ERR(fmt, arg...)  LOGE(" "fmt, ##arg)
#define HOMESET_WARN(fmt, arg...) LOGW(" "fmt, ##arg)
#define HOMESET_DBG(fmt, arg...)  LOGD(" "fmt, ##arg)

#include <Elementary.h>
#include <libintl.h>
#include <ui-gadget-module.h>

#define PKGNAME "ug-homescreen-setting-efl"

#define HOMESET_DOMAIN "ug-setting-homescreen-efl"
#define HOMESET_TEXT(str) dgettext(HOMESET_DOMAIN, str)
#define _S(str)	dgettext("sys_string", str)

/* User created ug data */
struct ug_data
{
	Evas_Object *base;
	ui_gadget_h ug;

	Evas_Object *naviframe;

	// for main view
	Evas_Object *genlist_main;
};

#endif /* __HOMESCREEN_SETTING_EFL_H__ */
