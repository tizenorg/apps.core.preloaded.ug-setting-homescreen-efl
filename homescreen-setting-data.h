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

#ifndef __HOMESCREEN_SETTING_DATA_H__
#define __HOMESCREEN_SETTING_DATA_H__

#include "homescreen-setting-efl.h"

#define HOMESCREEN_SETTING_DEFAULT_PKGNAME	"org.tizen.menu-screen"

typedef struct _homescreen_setting_data_list homescreen_setting_data_list_t;
struct _homescreen_setting_data_list
{
	homescreen_setting_data_list_t *next;
	int id;
	int index;
	char *appid;
	char *name;
};

void homescreen_setting_data_set_selected_homeapp(const char *appid);
char *homescreen_setting_data_get_selected_homeapp(void);
char *homescreen_setting_data_get_name(const char *appid);

homescreen_setting_data_list_t *homescreen_setting_data_get_homeapp_list(int *count);

#endif /* __HOMESCREEN_SETTING_DATA_H__ */
