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

#include <vconf.h>
#include <ail.h>
#include <pkgmgr-info.h>

#include "homescreen-setting-data.h"

static int gAppCount = 0;
static homescreen_setting_data_list_t *gDataList = NULL;

static int _homescreen_setting_data_applist_callback(const pkgmgrinfo_appinfo_h handle, void *user_data)
{
	int ret = PMINFO_R_ERROR;
	char *appid = NULL;
	char *name = NULL;

	// get application id
	ret = pkgmgrinfo_appinfo_get_appid(handle, &appid);
	if (ret != PMINFO_R_OK)
	{
		HOMESET_DBG("Failed[%d] pkgmgrinfo_appinfo_get_appid", ret);
		return 0;
	}

	if (!appid)
	{
		HOMESET_DBG("Invalid data appid[%s]", appid);
		return 0;
	}

	if (!strcmp(HOMESCREEN_SETTING_DEFAULT_PKGNAME, appid))
	{
		HOMESET_DBG("Ignore default pakcage[%s]", appid);
		return 0;
	}

	name = homescreen_setting_data_get_name(appid);
	if (!name)
	{
		HOMESET_DBG("Failed to get name from appid[%s]", appid);
		return 0;
	}

	gAppCount++;

	// create data list
	homescreen_setting_data_list_t *data = (homescreen_setting_data_list_t *) malloc(sizeof(homescreen_setting_data_list_t));
	data->id = 0;
	data->index = gAppCount;
	data->appid = strdup(appid);
	data->name = name;
	data->next = NULL;

	HOMESET_DBG("index[%d] appid[%s] name[%s]", data->index, data->appid, data->name);

	if (gDataList)
	{
		// attach to tail
		homescreen_setting_data_list_t *temp = gDataList;
		while (temp->next)
		{
			temp = temp->next;
		}

		temp->next = data;
	}
	else
	{
		// first data
		gDataList = data;
	}

	return 0;
}

void homescreen_setting_data_set_selected_homeapp(const char *appid)
{
	int ret = -1;
	if (appid)
	{
		ret = vconf_set_str(VCONFKEY_SETAPPL_SELECTED_PACKAGE_NAME, appid);
		if (ret < 0)
		{
			HOMESET_ERR("Failed[%d] vconf_set_str(VCONFKEY_SETAPPL_SELECTED_PACKAGE_NAME, %s)", ret, appid);
		}
	}
}

char *homescreen_setting_data_get_selected_homeapp(void)
{
	return vconf_get_str(VCONFKEY_SETAPPL_SELECTED_PACKAGE_NAME);
}

char *homescreen_setting_data_get_name(const char *appid)
{
	int ret = AIL_ERROR_FAIL;
	char *name = NULL;
	ail_appinfo_h ai;

	if (!appid)
	{
		HOMESET_ERR("invalid appid");
		return NULL;
	}

	ret = ail_get_appinfo(appid, &ai);
	if (ret != AIL_ERROR_OK)
	{
		HOMESET_ERR("Failed[%d] ail_get_appinfo(%s)", ret, appid);
		return NULL;
	}

	ret = ail_appinfo_get_str(ai, AIL_PROP_NAME_STR, &name);
	if (ret != AIL_ERROR_OK)
	{
		HOMESET_ERR("Failed ail_appinfo_get_str(%s) : %d", appid, ret);
		ail_destroy_appinfo(ai);
		return NULL;
	}

	if (!name)
	{
		ail_destroy_appinfo(ai);
		return NULL;
	}

	char *ret_name = strdup(name);
	ail_destroy_appinfo(ai);

	return ret_name;
}

homescreen_setting_data_list_t *homescreen_setting_data_get_homeapp_list(int *count)
{
	int ret = PMINFO_R_ERROR;
	char *value = "http://tizen.org/category/homeapp";
	pkgmgrinfo_appinfo_filter_h handle;

	// reset application count and list
	gAppCount = 0;
	gDataList = NULL;

	// create filter
	ret = pkgmgrinfo_appinfo_filter_create(&handle);
	if (ret != PMINFO_R_OK)
	{
		HOMESET_DBG("Failed[%d] pkgmgrinfo_appinfo_filter_create", ret);
		return NULL;
	}

	// set filter
	ret = pkgmgrinfo_appinfo_filter_add_string(handle, PMINFO_APPINFO_PROP_APP_CATEGORY, value);
	if (ret != PMINFO_R_OK)
	{
		HOMESET_DBG("Failed[%d] pkgmgrinfo_appinfo_filter_add_string", ret);
		pkgmgrinfo_appinfo_filter_destroy(handle);
		return NULL;
	}

	// get list
	ret = pkgmgrinfo_appinfo_filter_foreach_appinfo(handle, _homescreen_setting_data_applist_callback, NULL);
	if (ret != PMINFO_R_OK)
	{
		HOMESET_DBG("Failed[%d] pkgmgrinfo_appinfo_filter_foreach_appinfo", ret);
		pkgmgrinfo_appinfo_filter_destroy(handle);
		return NULL;
	}

	pkgmgrinfo_appinfo_filter_destroy(handle);

	// set application count
	*count = gAppCount;

	return gDataList;
}
